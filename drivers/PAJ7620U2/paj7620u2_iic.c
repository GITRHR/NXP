#include "paj7620u2_iic.h"
#include "paj7620u2.h"
#include "pin_mux.h"
#include "fsl_common.h"
#include "fsl_iocon.h"

 const uint32_t port_up_config = (/* Pin is configured as PIO0_5 */
                                        IOCON_PIO_FUNC0 |
                                        /* Selects pull-down function */
                                        IOCON_MODE_PULLUP |
                                        /* Standard mode, output slew rate control is enabled */
                                        IOCON_PIO_SLEW_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN5 (coords: 88) is configured as PIO0_5 */
 
const uint32_t port_output_config = (/* Pin is configured as PIO1_4 */
                                        IOCON_PIO_FUNC0 |
                                        /* Selects pull-up function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Standard mode, output slew rate control is enabled */
                                        IOCON_PIO_SLEW_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN4 (coords: 1) is configured as PIO1_4 */

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

//PAJ2670U2 I2C初始化
void GS_i2c_init(void)
{
	 gpio_pin_config_t GPIO_config = {
        kGPIO_DigitalOutput,
        1,
    };
	 
	GPIO_PinInit(GPIO, 0U, 15u, &GPIO_config);
	GPIO_PinInit(GPIO, 1U, 10u, &GPIO_config);
}

//产生IIC起始信号
static void GS_IIC_Start(void)
{
	GS_SDA_OUT();//sda线输出
	GS_IIC_SDA_HIGH;	  	  
	GS_IIC_SCL_HIGH;
	delay_us(4);
 	GS_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	GS_IIC_SCL_LOW;//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号
static void GS_IIC_Stop(void)
{
	GS_SDA_OUT();//sda线输出
	GS_IIC_SCL_LOW;
	GS_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	GS_IIC_SCL_HIGH; 
	GS_IIC_SDA_HIGH;//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static uint8_t GS_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	GS_SDA_IN();  //SDA设置为输入  
	GS_IIC_SDA_HIGH;delay_us(3);	   
	GS_IIC_SCL_HIGH;delay_us(3);	 
	while(GS_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			GS_IIC_Stop();
			return 1;
		}
	}
	GS_IIC_SCL_LOW;//时钟输出0 	   
	return 0;  
}

//产生ACK应答
static void GS_IIC_Ack(void)
{
	GS_IIC_SCL_LOW;
	GS_SDA_OUT();
	GS_IIC_SDA_LOW;
	delay_us(3);
	GS_IIC_SCL_HIGH;
	delay_us(3);
	GS_IIC_SCL_LOW;
}

//不产生ACK应答		    
static void GS_IIC_NAck(void)
{
	GS_IIC_SCL_LOW;
	GS_SDA_OUT();
	GS_IIC_SDA_HIGH;
	delay_us(2);
	GS_IIC_SCL_HIGH;
	delay_us(2);
	GS_IIC_SCL_LOW;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
static void GS_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	GS_SDA_OUT(); 	    
    GS_IIC_SCL_LOW;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			if((txd&0x80)>>7)
			{
				GS_IIC_SDA_HIGH;
			}
			else
			{
				GS_IIC_SDA_LOW;
			}
			txd<<=1; 	  
			delay_us(5);  
			GS_IIC_SCL_HIGH;
			delay_us(5); 
			GS_IIC_SCL_LOW;	
			delay_us(5);
    }	 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
static uint8_t GS_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	GS_SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		GS_IIC_SCL_LOW; 
		delay_us(4);
	  GS_IIC_SCL_HIGH;
		receive<<=1;
		if(GS_READ_SDA)receive++;   
	  delay_us(4); 
	}					 
	if (!ack)
		GS_IIC_NAck();//发送nACK
	else
		GS_IIC_Ack(); //发送ACK   
	return receive;
}

//PAJ7620U2写一个字节数据
uint8_t GS_Write_Byte(uint8_t REG_Address,uint8_t REG_data)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);
	if(GS_IIC_Wait_Ack())
	{
		GS_IIC_Stop();//释放总线
		return 1;//没应答则退出

	}
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();	
	GS_IIC_Send_Byte(REG_data);
	GS_IIC_Wait_Ack();	
	GS_IIC_Stop();

	return 0;
}

//PAJ7620U2读一个字节数据
uint8_t GS_Read_Byte(uint8_t REG_Address)
{
	uint8_t REG_data;
	
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//发写命令
	if(GS_IIC_Wait_Ack())
	{
		 GS_IIC_Stop();//释放总线
		 return 0;//没应答则退出
	}		
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();
	GS_IIC_Start(); 
	GS_IIC_Send_Byte(PAJ7620_ID|0x01);//发读命令
	GS_IIC_Wait_Ack();
	REG_data = GS_IIC_Read_Byte(0);
	GS_IIC_Stop();

	return REG_data;
}
//PAJ7620U2读n个字节数据
uint8_t GS_Read_nByte(uint8_t REG_Address,uint16_t len,uint8_t *buf)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//发写命令
	if(GS_IIC_Wait_Ack()) 
	{
		GS_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();

	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID|0x01);//发读命令
	GS_IIC_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = GS_IIC_Read_Byte(0);
		}
		else
		{
			*buf = GS_IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	GS_IIC_Stop();//释放总线

	return 0;
	
}
//PAJ7620唤醒
void GS_WakeUp(void)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//发写命令
	GS_IIC_Stop();//释放总线
}




