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

//PAJ2670U2 I2C��ʼ��
void GS_i2c_init(void)
{
	 gpio_pin_config_t GPIO_config = {
        kGPIO_DigitalOutput,
        1,
    };
	 
	GPIO_PinInit(GPIO, 0U, 15u, &GPIO_config);
	GPIO_PinInit(GPIO, 1U, 10u, &GPIO_config);
}

//����IIC��ʼ�ź�
static void GS_IIC_Start(void)
{
	GS_SDA_OUT();//sda�����
	GS_IIC_SDA_HIGH;	  	  
	GS_IIC_SCL_HIGH;
	delay_us(4);
 	GS_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	GS_IIC_SCL_LOW;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
static void GS_IIC_Stop(void)
{
	GS_SDA_OUT();//sda�����
	GS_IIC_SCL_LOW;
	GS_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	GS_IIC_SCL_HIGH; 
	GS_IIC_SDA_HIGH;//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
static uint8_t GS_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	GS_SDA_IN();  //SDA����Ϊ����  
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
	GS_IIC_SCL_LOW;//ʱ�����0 	   
	return 0;  
}

//����ACKӦ��
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

//������ACKӦ��		    
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
static void GS_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	GS_SDA_OUT(); 	    
    GS_IIC_SCL_LOW;//����ʱ�ӿ�ʼ���ݴ���
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

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
static uint8_t GS_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	GS_SDA_IN();//SDA����Ϊ����
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
		GS_IIC_NAck();//����nACK
	else
		GS_IIC_Ack(); //����ACK   
	return receive;
}

//PAJ7620U2дһ���ֽ�����
uint8_t GS_Write_Byte(uint8_t REG_Address,uint8_t REG_data)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);
	if(GS_IIC_Wait_Ack())
	{
		GS_IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�

	}
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();	
	GS_IIC_Send_Byte(REG_data);
	GS_IIC_Wait_Ack();	
	GS_IIC_Stop();

	return 0;
}

//PAJ7620U2��һ���ֽ�����
uint8_t GS_Read_Byte(uint8_t REG_Address)
{
	uint8_t REG_data;
	
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//��д����
	if(GS_IIC_Wait_Ack())
	{
		 GS_IIC_Stop();//�ͷ�����
		 return 0;//ûӦ�����˳�
	}		
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();
	GS_IIC_Start(); 
	GS_IIC_Send_Byte(PAJ7620_ID|0x01);//��������
	GS_IIC_Wait_Ack();
	REG_data = GS_IIC_Read_Byte(0);
	GS_IIC_Stop();

	return REG_data;
}
//PAJ7620U2��n���ֽ�����
uint8_t GS_Read_nByte(uint8_t REG_Address,uint16_t len,uint8_t *buf)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//��д����
	if(GS_IIC_Wait_Ack()) 
	{
		GS_IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	GS_IIC_Send_Byte(REG_Address);
	GS_IIC_Wait_Ack();

	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID|0x01);//��������
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
	GS_IIC_Stop();//�ͷ�����

	return 0;
	
}
//PAJ7620����
void GS_WakeUp(void)
{
	GS_IIC_Start();
	GS_IIC_Send_Byte(PAJ7620_ID);//��д����
	GS_IIC_Stop();//�ͷ�����
}




