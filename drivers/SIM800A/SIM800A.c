#include "SIM800A.h"
#include "board.h"
#include "fsl_ctimer.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "QDTFT_demo.h"

char Uart2_Buf[Buf2_Max];//串口2接收缓存
uint8_t Timer0_start;	//定时器0延时启动计数器
/*************	本地常量声明	**************/
static char *content="0891683110801105F011000D91688187127856F90008AA0C62115F88970089815E2E52A9";//发送短信内容
uint8_t Times=0,First_Int = 0, shijian=0;



int Send_Chinese_Message()
{
	Wait_CREG();    //查询等待模块注册成功

	Set_Pdu_Mode();//设置短信为PDU模式
  Send_Pdu_Sms();//发送一条短消息
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
* 函数名 : CLR_Buf2
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf2(void)
{
	uint16_t k;
	for(k=0;k<Buf2_Max;k++)      //将缓存内容清零
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
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
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
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
		UART2_SendString("AT+CREG?");   //查找模块是否注册成功
		UART2_SendLR();
		delay_ms(5000);  						
	    for(k=0;k<Buf2_Max;k++)      			
    	{
			if(Uart2_Buf[k] == ':')
			{
				if((Uart2_Buf[k+4] == '1')||(Uart2_Buf[k+4] == '5'))  //说明模块已经注册成功
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
		Uart2_Buf[First_Int] = res;  	  //将接收到的字符串存到缓存中
		First_Int++;                			//缓存指针向后移动
		if(First_Int > Buf2_Max)       		//如果缓存满,将缓存指针指向缓存的首地址
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
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、希望接收到的应答、发送等待时间(单位：S)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

void Second_AT_Command(char *b,char *a,uint8_t wait_time)         
{
	uint8_t i;
	char *c;
	c = b;										//保存字符串地址到c
	CLR_Buf2(); 
  i = 0;
	while(i == 0)                    
	{
		if(!Find(a)) 
		{
			if(Timer0_start == 0)
			{
				b = c;							//将字符串地址给b
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
* 函数名 : Set_Pdu_Mode
* 描述   : 设置短信为PDU文本模式
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Set_Pdu_Mode(void)
{
	Second_AT_Command("ATE0","OK",3);										  //取消回显	
	Second_AT_Command("AT+CMGF=0","OK",3);								//设置PDU模式	
	Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);//所有操作都在SIM卡中进行
}

/*******************************************************************************
* 函数名 : Send_Pdu_Sms
* 描述   : 发送PDU文本短信
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Send_Pdu_Sms(void)
{
	Second_AT_Command("AT+CMGS=27",">",3); //发送数据长度：27（具体的计算方法看串口调试比较）接收到“>”才发送短信内容
	UART2_SendString(content);     //发送短信内容
	USART_WriteByte(USART2, 0X1A);
	while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(USART2)));
  UART2_SendLR();
}







