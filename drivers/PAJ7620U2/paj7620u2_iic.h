#ifndef __PAJ7620U2_IIC_H
#define __PAJ7620U2_IIC_H
#include "fsl_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniV3 STM32开发板
//PAJ7620U2 IIC驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/7/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#define GS_SDA_IN()  {IOCON_PinMuxSet(IOCON, 1U, 10U, port_up_config);GPIO->DIR[1U] &= ~(1U << 10U);}   //PIO0_13
#define GS_SDA_OUT() {IOCON_PinMuxSet(IOCON, 1U, 10U, port_output_config);GPIO->DIR[1U] |= 1U << 10U;}

//IO操作函数	 
#define GS_IIC_SCL_HIGH GPIO_PinWrite(GPIO, 0U, 15U, 1U);
#define GS_IIC_SCL_LOW  GPIO_PinWrite(GPIO, 0U, 15U, 0U);

#define GS_IIC_SDA_HIGH GPIO_PinWrite(GPIO, 1U, 10U, 1U);
#define GS_IIC_SDA_LOW  GPIO_PinWrite(GPIO, 1U, 10U, 0U);

#define GS_READ_SDA   GPIO_PinRead(GPIO, 1u, 10u) 		    //输入SDA 

uint8_t GS_Write_Byte(uint8_t REG_Address,uint8_t REG_data);
uint8_t GS_Read_Byte(uint8_t REG_Address);
uint8_t GS_Read_nByte(uint8_t REG_Address,uint16_t len,uint8_t *buf);
void GS_i2c_init(void);
void GS_WakeUp(void);

#endif


