#include "paj7620u2.h"
#include "Lcd_Driver.h"
#include "paj7620u2_cfg.h"
#include "GUI.h"
#include "QDTFT_demo.h"
#include "board.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniV3 STM32������
//PAJ7620U2 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/7/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//ѡ��PAJ7620U2 BANK����
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0�Ĵ�������
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1�Ĵ�������
	}	
}

//PAJ7620U2����
uint8_t paj7620u2_wakeup(void)
{ 
	uint8_t data=0x0a;
	GS_WakeUp();//����PAJ7620U2
	delay_ms(5);//����ʱ��>400us
	GS_WakeUp();//����PAJ7620U2
	delay_ms(5);//����ʱ��>400us
	paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	data = GS_Read_Byte(0x00);//��ȡ״̬
	if(data!=0x20) return 0; //����ʧ��
	
	return 1;
}

//PAJ7620U2��ʼ��
//����ֵ��0:ʧ�� 1:�ɹ�
uint8_t paj7620u2_init(void)
{
	uint8_t i;
	uint8_t status;
	
	GS_i2c_init();//IIC��ʼ��
    status = paj7620u2_wakeup();//����PAJ7620U2
	if(!status) return 0;
	paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//��ʼ��PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//�л���BANK0�Ĵ�������
	
	return 1;
}

//���˵�
void paj7620u2_test_ui(void)
{
	Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,"Handle:");
}

void Gesture_init()
{
	int i=0;
	paj7620u2_selectBank(0);//����BANK0�Ĵ�������
	for(i=0;i<sizeof(gesture_arry)/2;i++)
	{
		GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//����ʶ��ģʽ��ʼ��
	}
	paj7620u2_selectBank(0);//�л���BANK0�Ĵ�������
}

//����ʶ�����
void Gesture_test(void)
{
	uint8_t i;
  uint8_t status;
	uint8_t data[2]={0x00};
	uint16_t gesture_data;
	i=0;
   status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);//��ȡ����״̬			
		if(!status)
		{   
			gesture_data =(uint16_t)data[1]<<8 | data[0];
			if(gesture_data) 
			{
				switch(gesture_data)
				{
					case GES_UP:              GUI_NUM--; if(GUI_NUM<=0)GUI_NUM=0;
					                                        break; //����
					case GES_DOWM:             GUI_NUM++;  if(GUI_NUM>=5)GUI_NUM=5;
               						                        break; //��s��
					case GES_LEFT:                     
  						                                   break; //����
					case GES_RIGHT:                
                						                    break; //����
					case GES_FORWARD:      
						                              break; //��ǰ
					case GES_BACKWARD:        
            						                 break; //���
					case GES_CLOCKWISE:             break; //˳ʱ��
					case GES_COUNT_CLOCKWISE:  
                   						           break; //��ʱ��
					case GES_WAVE:           
						             break; //�Ӷ�
					default:   break;
					
				}
			}
	}
}

//PAJ7620U2����������
void paj7620u2_sensor_test(void)
{   
	 uint8_t i=0;
	 uint8_t key;
		Gesture_test();   //��ʽ���
}
