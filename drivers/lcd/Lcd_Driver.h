#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include "fsl_gpio.h"

/////////////////////////////////////�û�������///////////////////////////////////	 
//֧�ֺ��������ٶ����л�
#define USE_HORIZONTAL  		1	//�����Ƿ�ʹ�ú��� 		0,��ʹ��.1,ʹ��.

//-----------------------------SPI ��������--------------------------------------//
#define USE_HARDWARE_SPI     1  //1:Enable Hardware SPI;0:USE Soft SPI

//-------------------------��Ļ������������--------------------------------------//
#define LCD_X_SIZE	        176
#define LCD_Y_SIZE	        220

#if USE_HORIZONTAL//��������˺��� 
#define X_MAX_PIXEL	        LCD_Y_SIZE
#define Y_MAX_PIXEL	        LCD_X_SIZE
#else
#define X_MAX_PIXEL	        LCD_X_SIZE
#define Y_MAX_PIXEL	        LCD_Y_SIZE
#endif
//////////////////////////////////////////////////////////////////////////////////
	 





#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//��ɫ0 3165 00110 001011 00101
#define GRAY1   0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2   0x4208      	//��ɫ2  1111111111011111




//�����Գ���ʹ�õ���ģ��SPI�ӿ�����
//�����ɸ��Ľӿ�IO���ã�ʹ����������4 IO������ɱ���Һ��������ʾ
#define LCD_PORT   	  	1u		//����TFT���ݶ˿�
#define LCD_RS         	1u	//PB11--->>TFT --RS/DC
#define LCD_CS        	8u //MCU_PB11--->>TFT --CS/CE
#define LCD_RST     		5u	//PB10--->>TFT --RST
#define LCD_SCL        	2u	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	3u	//PB15 MOSI--->>TFT --SDA/DIN

//Һ�����ƿ���1�������궨��
#define	LCD_CS_SET  	GPIO_PortSet(GPIO, LCD_PORT, 1u << LCD_CS);    
#define	LCD_RS_SET  	GPIO_PortSet(GPIO, LCD_PORT, 1u << LCD_RS);    
#define	LCD_SDA_SET  	GPIO_PortSet(GPIO, LCD_PORT, 1u << LCD_SDA);    
#define	LCD_SCL_SET  	GPIO_PortSet(GPIO, LCD_PORT, 1u << LCD_SCL);    
#define	LCD_RST_SET  	GPIO_PortSet(GPIO, LCD_PORT, 1u << LCD_RST);      

//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	GPIO_PortClear(GPIO, LCD_PORT, 1u << LCD_CS);   
#define	LCD_RS_CLR  	GPIO_PortClear(GPIO, LCD_PORT, 1u << LCD_RS);    
#define	LCD_SDA_CLR  	GPIO_PortClear(GPIO, LCD_PORT, 1u << LCD_SDA);    
#define	LCD_SCL_CLR  	GPIO_PortClear(GPIO, LCD_PORT, 1u << LCD_SCL);    
#define	LCD_RST_CLR  	GPIO_PortClear(GPIO, LCD_PORT, 1u << LCD_RST);    

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end);
void Lcd_WriteData_16Bit(uint16_t Data);

#endif
