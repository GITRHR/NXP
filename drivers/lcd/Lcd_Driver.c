#include "GUI.h"
#include "Lcd_Driver.h"
#include "fsl_gpio.h"


/***************************************************************************************
STM32����ƽ̨����:
�����壺����ԭ��MiniSTM32������
MCU ��STM32_F103_RBT6
���� ��12MHZ
��Ƶ ��72MHZ
����˵��:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10������TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 ������TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12������TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13������TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15������TFT - SDI
//VCC:���Խ�5VҲ���Խ�3.3V
//LED:���Խ�5VҲ���Խ�3.3V����ʹ���������IO����(�ߵ�ƽʹ��)
//GND���ӵ�Դ��
//˵��������Ҫ��������ռ��IO�����Խ�LCD_CS�ӵأ�LCD_LED��3.3V��LCD_RST������Ƭ����λ�ˣ�
//�������ͷ�3������IO
//�ӿڶ�����Lcd_Driver.h�ڶ��壬
//������IO�ӷ������������ʵ�ʽ����޸���ӦIO��ʼ��LCD_GPIO_Init()
//-----------------------------------------------------------------------------------------
���̹���˵����
1.	��ˢ������
2.	Ӣ����ʾ����ʾ��
3.	������ʾ����ʾ��
4.	�����������ʾʾ��
5.	ͼƬ��ʾʾ��
6.	2D�����˵�ʾ��
7.	������֧�ֺ���/�����л�(������USE_HORIZONTAL,���Lcd_Driver.h)
8.	������֧�����ģ��SPI/Ӳ��SPI�л�(������USE_HARDWARE_SPI,���Lcd_Driver.h)
**********************************************************************************************/


//---------------------------------function----------------------------------------------------//

/****************************************************************************
* ��    �ƣ�void LCD_GPIO_Init(void)
* ��    �ܣ�STM32_ģ��SPI���õ���GPIO��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʼ��ģ��SPI���õ�GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{
	gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };
	
//		GPIO_PortInit(GPIO, LCD_PORT);
		GPIO_PinInit(GPIO, LCD_PORT, LCD_RS  , &led_config); 
		GPIO_PinInit(GPIO, LCD_PORT, LCD_CS  , &led_config); 
		GPIO_PinInit(GPIO, LCD_PORT, LCD_RST , &led_config); 
		GPIO_PinInit(GPIO, LCD_PORT, LCD_SCL , &led_config); 
    GPIO_PinInit(GPIO, LCD_PORT, LCD_SDA , &led_config);   
}

/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(uint8_t Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData(uint8_t Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
		{
			LCD_SDA_SET; //�������
		}
    else
		{
			LCD_SDA_CLR;
		}
    LCD_SCL_CLR;       
    LCD_SCL_SET;
    Data<<=1; 
	}
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteIndex(uint8_t Index)
* ��    �ܣ���Һ����дһ��8λָ��
* ��ڲ�����Index   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
****************************************************************************/
void Lcd_WriteIndex(uint8_t Index)
{
   LCD_CS_CLR;
   LCD_RS_CLR;
   SPIv_WriteData(Index);
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteData(uint8_t Data)
* ��    �ܣ���Һ����дһ��8λ����
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ��ڲ�����
****************************************************************************/
void Lcd_WriteData(uint8_t Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
   SPIv_WriteData(Data);
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void LCD_WriteReg(uint8_t Index,uint16_t Data)
* ��    �ܣ�д�Ĵ�������
* ��ڲ�����Index,Data
* ���ڲ�������
* ˵    ����������Ϊ��Ϻ�������Index��ַ�ļĴ���д��Dataֵ
****************************************************************************/
void LCD_WriteReg(uint8_t Index,uint16_t Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_WriteData_16Bit(uint16_t Data)
* ��    �ܣ���Һ����дһ��16λ����
* ��ڲ�����Data
* ���ڲ�������
* ˵    �����������ָ����ַд��һ��16λ����
****************************************************************************/
void Lcd_WriteData_16Bit(uint16_t Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}

/****************************************************************************
* ��    �ƣ�void Lcd_Reset(void)
* ��    �ܣ�Һ��Ӳ��λ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��ǰ��ִ��һ�θ�λ����
****************************************************************************/
void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}
/****************************************************************************
* ��    �ƣ�void Lcd_Init(void)
* ��    �ܣ�Һ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
	//LCD_GPIO_Init();
	
	Lcd_Reset(); //Reset before LCD Init.

	//************* Start Initial Sequence **********//
  Lcd_WriteIndex(0x02); Lcd_WriteData_16Bit(0x0100); // set 1 line inversion
#if USE_HORIZONTAL//��������˺���
	//R01H:SM=0,GS=0,SS=0 (for details,See the datasheet of ILI9225)
	LCD_WriteReg(0x01, 0x001C); // set the display line number and display direction
	//R03H:BGR=1,ID0=1,ID1=1,AM=1 (for details,See the datasheet of ILI9225)
	LCD_WriteReg(0x03, 0x1038); // set GRAM write direction .
#else//����
	//R01H:SM=0,GS=0,SS=1 (for details,See the datasheet of ILI9225)
	LCD_WriteReg(0x01, 0x011C); // set the display line number and display direction 
	//R03H:BGR=1,ID0=1,ID1=1,AM=0 (for details,See the datasheet of ILI9225)
	LCD_WriteReg(0x03, 0x1030); // set GRAM write direction.
#endif

  Lcd_WriteIndex(0x08); Lcd_WriteData_16Bit(0x0808); // set BP and FP
  Lcd_WriteIndex(0x0B); Lcd_WriteData_16Bit(0x1100); //frame cycle
  Lcd_WriteIndex(0x0C); Lcd_WriteData_16Bit(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
  Lcd_WriteIndex(0x0F); Lcd_WriteData_16Bit(0x1401); // Set frame rate----0801
  Lcd_WriteIndex(0x15); Lcd_WriteData_16Bit(0x0000); //set system interface
  Lcd_WriteIndex(0x20); Lcd_WriteData_16Bit(0x0000); // Set GRAM Address
  Lcd_WriteIndex(0x21); Lcd_WriteData_16Bit(0x0000); // Set GRAM Address
  //*************Power On sequence ****************//
    delay_ms(50);                         // Delay 50ms
  Lcd_WriteIndex(0x10); Lcd_WriteData_16Bit(0x0800); // Set SAP,DSTB,STB----0A00
  Lcd_WriteIndex(0x11); Lcd_WriteData_16Bit(0x1F3F); // Set APON,PON,AON,VCI1EN,VC----1038
    delay_ms(50);                         // Delay 50ms
  Lcd_WriteIndex(0x12); Lcd_WriteData_16Bit(0x0121); // Internal reference voltage= Vci;----1121
  Lcd_WriteIndex(0x13); Lcd_WriteData_16Bit(0x006F); // Set GVDD----0066
  Lcd_WriteIndex(0x14); Lcd_WriteData_16Bit(0x4349); // Set VCOMH/VCOML voltage----5F60
  //-------------- Set GRAM area -----------------//
  Lcd_WriteIndex(0x30); Lcd_WriteData_16Bit(0x0000);
  Lcd_WriteIndex(0x31); Lcd_WriteData_16Bit(0x00DB);
  Lcd_WriteIndex(0x32); Lcd_WriteData_16Bit(0x0000);
  Lcd_WriteIndex(0x33); Lcd_WriteData_16Bit(0x0000);
  Lcd_WriteIndex(0x34); Lcd_WriteData_16Bit(0x00DB);
  Lcd_WriteIndex(0x35); Lcd_WriteData_16Bit(0x0000);
  Lcd_WriteIndex(0x36); Lcd_WriteData_16Bit(0x00AF);
  Lcd_WriteIndex(0x37); Lcd_WriteData_16Bit(0x0000);
  Lcd_WriteIndex(0x38); Lcd_WriteData_16Bit(0x00DB);
  Lcd_WriteIndex(0x39); Lcd_WriteData_16Bit(0x0000);
  // ----------- Adjust the Gamma Curve ----------//
  Lcd_WriteIndex(0x50); Lcd_WriteData_16Bit(0x0001);  //0400
  Lcd_WriteIndex(0x51); Lcd_WriteData_16Bit(0x200B);  //060B
  Lcd_WriteIndex(0x52); Lcd_WriteData_16Bit(0x0000);  //0C0A
  Lcd_WriteIndex(0x53); Lcd_WriteData_16Bit(0x0404);  //0105
  Lcd_WriteIndex(0x54); Lcd_WriteData_16Bit(0x0C0C);  //0A0C
  Lcd_WriteIndex(0x55); Lcd_WriteData_16Bit(0x000C);  //0B06
  Lcd_WriteIndex(0x56); Lcd_WriteData_16Bit(0x0101);  //0004
  Lcd_WriteIndex(0x57); Lcd_WriteData_16Bit(0x0400);  //0501
  Lcd_WriteIndex(0x58); Lcd_WriteData_16Bit(0x1108);  //0E00
  Lcd_WriteIndex(0x59); Lcd_WriteData_16Bit(0x050C);  //000E
  delay_ms(50);                                     // Delay 50ms
  Lcd_WriteIndex(0x07); Lcd_WriteData_16Bit(0x1017);
  Lcd_WriteIndex(0x22);	
}



/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(uint16_t Xpos, uint16_t Ypos)
{	
#if USE_HORIZONTAL//��������˺���  	    	
	LCD_WriteReg(0x21,Xpos);
	LCD_WriteReg(0x20,Ypos);
#else//����	
	LCD_WriteReg(0x20,Xpos);
	LCD_WriteReg(0x21,Ypos);
#endif
	Lcd_WriteIndex(0x22);		
} 
/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
//������ʾ����
void Lcd_SetRegion(uint8_t xStar, uint8_t yStar,uint8_t xEnd,uint8_t yEnd)
{
#if USE_HORIZONTAL//��������˺���	
	LCD_WriteReg(0x38,xEnd);
	LCD_WriteReg(0x39,xStar);
	LCD_WriteReg(0x36,yEnd);
	LCD_WriteReg(0x37,yStar);
	LCD_WriteReg(0x21,xStar);
	LCD_WriteReg(0x20,yStar);
#else//����	
	LCD_WriteReg(0x36,xEnd);
	LCD_WriteReg(0x37,xStar);
	LCD_WriteReg(0x38,yEnd);
	LCD_WriteReg(0x39,yStar);
	LCD_WriteReg(0x20,xStar);
	LCD_WriteReg(0x21,yStar);
#endif
	Lcd_WriteIndex(0x22);	
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�����xy�������ɫ����
����ֵ����
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);

}    

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
   unsigned int i;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	 LCD_CS_CLR;
   LCD_RS_SET;	
   for(i=0;i<38720;i++)
   {	

		SPIv_WriteData(Color>>8);
		SPIv_WriteData(Color);
   }   
		LCD_CS_SET;
}

