#include "SIM800A.h"
#include "board.h"
#include "fsl_ctimer.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "QDTFT_demo.h"

char Uart2_Buf[Buf2_Max];//����2���ջ���
uint8_t Timer0_start;	//��ʱ��0��ʱ����������
/*************	���س�������	**************/
static char *content="0891683110801105F011000D91688187127856F90008AA0C62115F88970089815E2E52A9";//���Ͷ�������
uint8_t Times=0,First_Int = 0, shijian=0;



int Send_Chinese_Message()
{
	Wait_CREG();    //��ѯ�ȴ�ģ��ע��ɹ�

	Set_Pdu_Mode();//���ö���ΪPDUģʽ
  Send_Pdu_Sms();//����һ������Ϣ
}

void ctimer_match1_callback()
{
	
	if(GUI_NUM==0)
	{
			Sec++;
			if(Sec>=60)
			{
				Sec=0;
				Min++;
				if(Min>=60)
				{
					Min=0;
					Hour++;
					if(Hour>=24)
					{
						Hour=0;
					}
				}
			}
	}
	if(Timer0_start)
		Times++;
		if(Times > shijian)
		{
			Timer0_start = 0;
			Times = 0;
		}
		GPIO_PortToggle(GPIO, 1U, 1U<<7U);
	CTIMER_ClearStatusFlags(CTIMER3, kCTIMER_Match1Flag);
}

/*******************************************************************************
* ������ : CLR_Buf2
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_Buf2(void)
{
	uint16_t k;
	for(k=0;k<Buf2_Max;k++)      //��������������
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

void UART2_SendString(char *s)
{
	while(*s)
	{
		USART_WriteByte(USART2, (uint8_t)*s++);
		while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(USART2)));
	}
}


/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Wait_CREG(void)
{
	uint8_t i;
	uint8_t k;
	i = 0;
	CLR_Buf2();
  while(i == 0)        			
	{
		CLR_Buf2();        
		UART2_SendString("AT+CREG?");   //����ģ���Ƿ�ע��ɹ�
		UART2_SendLR();
		delay_ms(5000);  						
	    for(k=0;k<Buf2_Max;k++)      			
    	{
			if(Uart2_Buf[k] == ':')
			{
				if((Uart2_Buf[k+4] == '1')||(Uart2_Buf[k+4] == '5'))  //˵��ģ���Ѿ�ע��ɹ�
				{
					i = 1;
					if(GUI_NUM==3)
					{
						Gui_DrawFont_GBK16(16,100,RED,GRAY0,"Registered Successfully!!!");
					}
				  break;
				}
			}
		}
	}
}



void FLEXCOMM2_IRQHandler()
{
	uint8_t res;
	if ((kUSART_RxFifoNotEmptyFlag | kUSART_RxError) & USART_GetStatusFlags(USART2))
	{
		res = USART_ReadByte(USART2);
		Uart2_Buf[First_Int] = res;  	  //�����յ����ַ����浽������
		First_Int++;                			//����ָ������ƶ�
		if(First_Int > Buf2_Max)       		//���������,������ָ��ָ�򻺴���׵�ַ
		{
			First_Int = 0;
		}    
	}
}

uint8_t Find(char *a)
{
  if(strstr(Uart2_Buf,a)!=NULL)
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢ϣ�����յ���Ӧ�𡢷��͵ȴ�ʱ��(��λ��S)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/

void Second_AT_Command(char *b,char *a,uint8_t wait_time)         
{
	uint8_t i;
	char *c;
	c = b;										//�����ַ�����ַ��c
	CLR_Buf2(); 
  i = 0;
	while(i == 0)                    
	{
		if(!Find(a)) 
		{
			if(Timer0_start == 0)
			{
				b = c;							//���ַ�����ַ��b
				for (; *b!='\0';b++)
				{
					USART_WriteByte(USART2, (uint8_t)*b);
	        while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(USART2)));
				}
				UART2_SendLR();	
				Times = 0;
				shijian = wait_time;
				Timer0_start = 1;
		   }
    }
 	  else
		{
			i = 1;
			Timer0_start = 0;
		}
	}
	CLR_Buf2(); 
}

/*******************************************************************************
* ������ : Set_Pdu_Mode
* ����   : ���ö���ΪPDU�ı�ģʽ
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Set_Pdu_Mode(void)
{
	Second_AT_Command("ATE0","OK",3);										  //ȡ������	
	Second_AT_Command("AT+CMGF=0","OK",3);								//����PDUģʽ	
	Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);//���в�������SIM���н���
}

/*******************************************************************************
* ������ : Send_Pdu_Sms
* ����   : ����PDU�ı�����
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Send_Pdu_Sms(void)
{
	Second_AT_Command("AT+CMGS=27",">",3); //�������ݳ��ȣ�27������ļ��㷽�������ڵ��ԱȽϣ����յ���>���ŷ��Ͷ�������
	UART2_SendString(content);     //���Ͷ�������
	USART_WriteByte(USART2, 0X1A);
	while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(USART2)));
  UART2_SendLR();
}







