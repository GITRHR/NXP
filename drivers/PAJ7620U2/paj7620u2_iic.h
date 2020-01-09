#ifndef __PAJ7620U2_IIC_H
#define __PAJ7620U2_IIC_H
#include "fsl_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniV3 STM32������
//PAJ7620U2 IIC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/7/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#define GS_SDA_IN()  {IOCON_PinMuxSet(IOCON, 1U, 10U, port_up_config);GPIO->DIR[1U] &= ~(1U << 10U);}   //PIO0_13
#define GS_SDA_OUT() {IOCON_PinMuxSet(IOCON, 1U, 10U, port_output_config);GPIO->DIR[1U] |= 1U << 10U;}

//IO��������	 
#define GS_IIC_SCL_HIGH GPIO_PinWrite(GPIO, 0U, 15U, 1U);
#define GS_IIC_SCL_LOW  GPIO_PinWrite(GPIO, 0U, 15U, 0U);

#define GS_IIC_SDA_HIGH GPIO_PinWrite(GPIO, 1U, 10U, 1U);
#define GS_IIC_SDA_LOW  GPIO_PinWrite(GPIO, 1U, 10U, 0U);

#define GS_READ_SDA   GPIO_PinRead(GPIO, 1u, 10u) 		    //����SDA 

uint8_t GS_Write_Byte(uint8_t REG_Address,uint8_t REG_data);
uint8_t GS_Read_Byte(uint8_t REG_Address);
uint8_t GS_Read_nByte(uint8_t REG_Address,uint16_t len,uint8_t *buf);
void GS_i2c_init(void);
void GS_WakeUp(void);

#endif


