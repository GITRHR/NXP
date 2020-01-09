#include "paj7620u2.h"
#include "Lcd_Driver.h"
#include "paj7620u2_cfg.h"
#include "GUI.h"
#include "QDTFT_demo.h"
#include "board.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniV3 STM32开发板
//PAJ7620U2 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/7/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//选择PAJ7620U2 BANK区域
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0寄存器区域
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1寄存器区域
	}	
}

//PAJ7620U2唤醒
uint8_t paj7620u2_wakeup(void)
{ 
	uint8_t data=0x0a;
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	data = GS_Read_Byte(0x00);//读取状态
	if(data!=0x20) return 0; //唤醒失败
	
	return 1;
}

//PAJ7620U2初始化
//返回值：0:失败 1:成功
uint8_t paj7620u2_init(void)
{
	uint8_t i;
	uint8_t status;
	
	GS_i2c_init();//IIC初始化
    status = paj7620u2_wakeup();//唤醒PAJ7620U2
	if(!status) return 0;
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//初始化PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
	
	return 1;
}

//主菜单
void paj7620u2_test_ui(void)
{
	Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,"Handle:");
}

void Gesture_init()
{
	int i=0;
	paj7620u2_selectBank(0);//进入BANK0寄存器区域
	for(i=0;i<sizeof(gesture_arry)/2;i++)
	{
		GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//手势识别模式初始化
	}
	paj7620u2_selectBank(0);//切换回BANK0寄存器区域
}

//手势识别测试
void Gesture_test(void)
{
	uint8_t i;
  uint8_t status;
	uint8_t data[2]={0x00};
	uint16_t gesture_data;
	i=0;
   status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);//读取手势状态			
		if(!status)
		{   
			gesture_data =(uint16_t)data[1]<<8 | data[0];
			if(gesture_data) 
			{
				switch(gesture_data)
				{
					case GES_UP:              GUI_NUM--; if(GUI_NUM<=0)GUI_NUM=0;
					                                        break; //向上
					case GES_DOWM:             GUI_NUM++;  if(GUI_NUM>=5)GUI_NUM=5;
               						                        break; //向s下
					case GES_LEFT:                     
  						                                   break; //向左
					case GES_RIGHT:                
                						                    break; //向右
					case GES_FORWARD:      
						                              break; //向前
					case GES_BACKWARD:        
            						                 break; //向后
					case GES_CLOCKWISE:             break; //顺时针
					case GES_COUNT_CLOCKWISE:  
                   						           break; //逆时针
					case GES_WAVE:           
						             break; //挥动
					default:   break;
					
				}
			}
	}
}

//PAJ7620U2传感器测试
void paj7620u2_sensor_test(void)
{   
	 uint8_t i=0;
	 uint8_t key;
		Gesture_test();   //手式检测
}
