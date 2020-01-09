/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_usart.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "QDTFT_demo.h"
#include "as608.h"
#include "fsl_ctimer.h"
#include "max30102.h"
#include "myiic.h"

#define USART2_MAX_RECV_LEN		400					//最大接收缓存字节数
#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */
#define MAX_BRIGHTNESS 255
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_PORT BOARD_LED_BLUE_GPIO_PORT
#define BOARD_LED_PIN BOARD_LED_BLUE_GPIO_PIN

void UART_IO_INIT(void);
void ShowErrMessage(uint8_t ensure);
void ctimer_match0_callback(uint32_t flags);
void press_FR(void);
void judge(uint16_t ch);
void delay_init();
void Press_2();
void GUI_INIT();
void GUI(int NUM);
void handle_event();
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
void ctimer_match1_callback();
//void maxim_max30102_read_first();
//void get_rate_spo();
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;
int num=10000;
int flag=0;
int Ready_Flusf=1;
uint32_t fac_ms;
uint8_t uch_dummy;
int GUI_NUM=0;
uint32_t fac_us;
int Hour=24, Min=59, Sec=50;
int Last_NUM=0;
int32_t n_ir_buffer_length; //data length
uint32_t aun_ir_buffer[150]; //infrared LED sensor data
uint32_t aun_red_buffer[150];  //red LED sensor data
int32_t n_spo2;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
volatile uint16_t txIndex; /* Index of the data to send out. */
volatile uint16_t rxIndex; /* Index of the memory to save new arrived data. */
uint8_t demoRingBuffer[16];
ctimer_callback_t ctimer_callback_table[] = {
     NULL, ctimer_match0_callback, NULL, NULL, NULL, NULL, NULL, NULL};
ctimer_callback_t ctimer3_callback_table[] = {
     NULL, ctimer_match1_callback, NULL, NULL, NULL, NULL, NULL, NULL};
uint8_t g_tipString[] =
    "Usart functional API interrupt example\r\nBoard receives characters then sends them out\r\nNow please input:\r\n";
/*******************************************************************************
 * Code
 ******************************************************************************/

void FLEXCOMM0_IRQHandler(void)
{
		uint8_t res;
    /* If new data arrived. */
    if ((kUSART_RxFifoNotEmptyFlag | kUSART_RxError) & USART_GetStatusFlags(USART0))
    {
        res = USART_ReadByte(USART0);
			if((USART0_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
			{ 
				if(USART0_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
				{
					CTIMER2->TC=0;
					if(USART0_RX_STA==0) 				//使能CTIMER2的中断 
					{
						CTIMER_StartTimer(CTIMER2);
					}
					USART0_RX_BUF[USART0_RX_STA++]=res;	//记录接收到的值	 
				}
				else 
				{
					USART0_RX_STA|=1<<15;				//强制标记接收完成
				} 
		}
	}  				 											     
}


/*!
 * @brief Main function
 */
int main(void)
{
	 char port_state = 0;
	 char str[20] = {0};
	 uint8_t t;
	 uint8_t len;	
	 uint16_t times=0;  
	 usart_config_t uart_config;
	 ctimer_config_t ctimer2_config;
	 ctimer_config_t ctimer3_config;
	 ctimer_match_config_t matchConfig0;
	 ctimer_match_config_t matchConfig1;
	 
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        1,
    };
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
		CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);
		
		 /* Use 12 MHz clock for some of the Ctimers */
		CLOCK_AttachClk(kFRO_HF_to_CTIMER2);
		CLOCK_AttachClk(kFRO_HF_to_CTIMER3);
		
		//GPIO_CLK init
		CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
		
    /* Board pin init */
    BOARD_InitPins();
		BOARD_BootClockFROHF96M();
		BOARD_InitDebugConsole();
		delay_init();
		
    /* Init output GPIO. */
    GPIO_PortInit(GPIO, BOARD_LED_PORT);
		GPIO_PortInit(GPIO, 0U);
    GPIO_PinInit(GPIO, BOARD_LED_PORT, 4u, &led_config);
		GPIO_PinInit(GPIO, BOARD_LED_PORT, 6u, &led_config);
		GPIO_PinInit(GPIO, BOARD_LED_PORT, 7u, &led_config);
		LCD_GPIO_Init();
		UART_IO_INIT();
		led_config.pinDirection=kGPIO_DigitalInput;
		GPIO_PinInit(GPIO, 1U, 20u, &led_config);
		GPIO_PinInit(GPIO, 1U, 25u, &led_config);
		GPIO_PinInit(GPIO, 0U, 1u, &led_config);

		//USART0
		USART_GetDefaultConfig(&uart_config);
    uart_config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    uart_config.enableTx     = true;
    uart_config.enableRx     = true;
		USART_Init(USART0, &uart_config, CLOCK_GetFreq(kCLOCK_Flexcomm0));
		USART_EnableInterrupts(USART0, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
    EnableIRQ(FLEXCOMM0_IRQn);
		
		//USART2
		USART_GetDefaultConfig(&uart_config);
    uart_config.baudRate_Bps = 115200U;
    uart_config.enableTx     = true;
    uart_config.enableRx     = true;
		USART_Init(USART2, &uart_config, CLOCK_GetFreq(kCLOCK_Flexcomm2));
		USART_EnableInterrupts(USART2, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
    EnableIRQ(FLEXCOMM2_IRQn);
		
		//ctimer2
		CTIMER_GetDefaultConfig(&ctimer2_config);
		CTIMER_Init(CTIMER2, &ctimer2_config);
		matchConfig0.enableCounterReset = true;
    matchConfig0.enableCounterStop  = false;
    matchConfig0.matchValue         = CLOCK_GetFreq(kCLOCK_CTmier2)/100;
    matchConfig0.outControl         = kCTIMER_Output_Toggle;
    matchConfig0.outPinInitState    = false;
    matchConfig0.enableInterrupt    = true;
		CTIMER_RegisterCallBack(CTIMER2, &ctimer_callback_table[0], kCTIMER_MultipleCallback);
		CTIMER_SetupMatch(CTIMER2, kCTIMER_Match_1, &matchConfig0);
		
		//ctimer3
		CTIMER_GetDefaultConfig(&ctimer3_config);
		CTIMER_Init(CTIMER3, &ctimer3_config);
		matchConfig1.enableCounterReset = true;
    matchConfig1.enableCounterStop  = false;
    matchConfig1.matchValue         = CLOCK_GetFreq(kCLOCK_CTmier3);
    matchConfig1.outControl         = kCTIMER_Output_Toggle;
    matchConfig1.outPinInitState    = false;
    matchConfig1.enableInterrupt    = true;
		CTIMER_RegisterCallBack(CTIMER3, &ctimer3_callback_table[0], kCTIMER_MultipleCallback);
		CTIMER_SetupMatch(CTIMER3, kCTIMER_Match_1, &matchConfig1);
		CTIMER_StartTimer(CTIMER3);
		
		//LCD
		Lcd_Init();	
		Lcd_Clear(GRAY0);
		DisplayButtonUp(43,18,190,36);
		Gui_DrawFont_GBK16(45,20,RED,GRAY0,"2019 MCU 挑战赛!!!");
		DisplayButtonUp(58,45,156,63);		
		Gui_DrawFont_GBK16(60,47,GREEN,GRAY0,"地表最强战队");	
		DisplayButtonUp(58,70,156,88);
		Gui_DrawFont_GBK16(60,72,YELLOW,GRAY0,"智能手表项目");	
		DisplayButtonUp(28,95,206,113);
		Gui_DrawFont_GBK16(30,97,BLACK,GRAY0,"请按下手指以解锁......");	
		
		//handle pressure
		while(PS_HandShake(&AS608Addr));
		Gui_DrawFont_GBK16(10,125,BLUE,GRAY0,"Handle Init Succcessful!!");
		delay_ms(500U);  
		Gui_DrawFont_GBK16(10,125,BLUE,GRAY0,"                         ");
		//PAJ7620U2传感器初始化
		while(!paj7620u2_init())
		{		
			;
		}
		Gesture_init();
		Gui_DrawFont_GBK16(10,125,BLUE,GRAY0,"Measure Init Succcessful!!");
		delay_ms(500U); 
		Gui_DrawFont_GBK16(10,125,BLUE,GRAY0,"                          ");
		while(Ready_Flusf)
		{
			if(GPIO_PinRead(GPIO, 1u, 20u))
			{
				delay_ms(10u);
				if(GPIO_PinRead(GPIO, 1u, 20u))
				{
					Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,"Checking...");
					press_FR();
				}
			}
		}
		delay_ms(10U); 
		Lcd_Clear(GRAY0);
		GUI_INIT();
    while (1)
    {
			paj7620u2_sensor_test();
			GUI(GUI_NUM);
			handle_event();
		}
}

void handle_event()
{
	static int Event_num=0, test_num=0;
	char test_str[10] = {0};
	
	Event_num++;
	
	if(Event_num>=50)
	{
		Event_num=0;
		if(GUI_NUM==0)
		{
			Gui_DrawFont_Num32(30,70,RED,GRAY0,Hour/10);
			Gui_DrawFont_Num32(55,70,RED,GRAY0,Hour%10);
			Gui_DrawFont_GBK16(82,80,RED,GRAY0,"--");
			Gui_DrawFont_Num32(100,70,RED,GRAY0,Min/10);
			Gui_DrawFont_Num32(125,70,RED,GRAY0,Min%10);
			Gui_DrawFont_GBK16(152,80,RED,GRAY0,"--");
			Gui_DrawFont_Num32(170,70,RED,GRAY0,Sec/10);
			Gui_DrawFont_Num32(195,70,RED,GRAY0,Sec%10);
		}
		else if(GUI_NUM==1)
		{
			Gui_DrawFont_GBK16(50,80,RED,GRAY0,"心率:95bPs/s");
		}
		else if(GUI_NUM==2)
		{
			Gui_DrawFont_GBK16(50,80,RED,GRAY0,"当前步数:1000步");
		}
		else if(GUI_NUM==3)
		{
			if(GPIO_PinRead(GPIO, 0u, 1u)==0)
			{
				delay_ms(10u);
				if(GPIO_PinRead(GPIO, 0u, 1u)==0)
				{
					test_num++;
					sprintf(test_str,"Hase Send Number:%d", test_num);
					Send_Chinese_Message();
					Gui_DrawFont_GBK16(50,50,BLUE,GRAY0,test_str);
				}
				while(!GPIO_PinRead(GPIO, 0u, 1u));
			}
		}
		else if(GUI_NUM==4)
		{
			QDTFT_Test_Demo();
		}
		else
		{
			if(GPIO_PinRead(GPIO, 1u, 20u))
			{
				delay_ms(10u);
				if(GPIO_PinRead(GPIO, 1u, 20u))
				{
					Press_2();
				}
			}
		}
	}
}

void GUI_INIT()
{
	Gui_DrawFont_GBK16(80,20,BLUE,GRAY0,"时间显示");
}

void GUI(int NUM)
{ 
	if(NUM!=Last_NUM)  //页面切换
	{
		Lcd_Clear(GRAY0);
		switch(NUM)
		{
			case 0:
				Gui_DrawFont_GBK16(80,20,BLUE,GRAY0,"时间显示");
				
				break;
			case 1:
				Gui_DrawFont_GBK16(80,20,BLUE,GRAY0,"心率测量");
				break;
			case 2:
				Gui_DrawFont_GBK16(80,20,BLUE,GRAY0,"运动计步");
				break;
			case 3:
				Gui_DrawFont_GBK16(65,20,BLUE,GRAY0,"紧急短信发送");
				break;
			case 4:
				Gui_DrawFont_GBK16(80,20,BLUE,GRAY0,"更多功能");
				break;
			case 5:
				Gui_DrawFont_GBK16(65,20,BLUE,GRAY0,"指纹检测功能");
				break;
		}
	}
	Last_NUM=NUM;
}

void ctimer_match0_callback(uint32_t flags)
{
   USART0_RX_STA|=1<<15;
	 CTIMER_StopTimer(CTIMER2);
	 CTIMER_ClearStatusFlags(CTIMER2, kCTIMER_Match1Flag);
}

int fputc(int ch, FILE *f)
{      
	USART_WriteByte(USART0, (uint8_t)ch);
	while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(USART0)));
	return ch;
}


void UART_IO_INIT(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port0_pin29_config = (/* Pin is configured as FC0_RXD_SDA_MOSI_DATA */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN29 (coords: 92) is configured as FC0_RXD_SDA_MOSI_DATA */
    IOCON_PinMuxSet(IOCON, 0U, 29U, port0_pin29_config);
		IOCON_PinMuxSet(IOCON, 1U, 24U, port0_pin29_config);

    const uint32_t port0_pin30_config = (/* Pin is configured as FC0_TXD_SCL_MISO_WS */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN30 (coords: 94) is configured as FC0_TXD_SCL_MISO_WS */
    IOCON_PinMuxSet(IOCON, 0U, 30U, port0_pin30_config);
	  IOCON_PinMuxSet(IOCON, 0U, 27U, port0_pin30_config);
}

void press_FR(void)
{
	SearchResult seach;
	char str_num[20] = {0};
	uint8_t ensure;
	uint16_t ID=0;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//搜索成功
			{				
				ID=seach.pageID;
				judge(ID);
				//sprintf(str_num,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//显示匹配指纹的ID和分数
				//Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,str_num);
			}
			else 
			{
				ShowErrMessage(ensure);		
			}				
	  }
		else
		{
			ShowErrMessage(ensure);
		}
		delay_ms(500U);//延时后清除显示
		Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,"                                        ");
    Gui_DrawFont_GBK16(0,150,BLUE,GRAY0,"                                        ");		
	}
}

void Press_2()
{
	SearchResult seach;
	char str_num[20] = {0};
	uint8_t ensure;
	uint16_t ID=0;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//搜索成功
			{				
				sprintf(str_num,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//显示匹配指纹的ID和分数
				Gui_DrawFont_GBK16(0,80,BLUE,GRAY0,str_num);
			}
			else 
			{
				ShowErrMessage(ensure);		
			}				
	  }
		else
		{
			ShowErrMessage(ensure);
		}
		delay_ms(500U);//延时后清除显示
		Gui_DrawFont_GBK16(0,80,BLUE,GRAY0,"                                        ");
    Gui_DrawFont_GBK16(0,150,BLUE,GRAY0,"                                        ");		
	}
}


void judge(uint16_t ch)
{
	switch(ch)
	{
		case 0:
			Gui_DrawFont_GBK16(0,125,BLUE,GRAY0,"刘皓然,欢迎进入系统!!!     ");
			delay_ms(100U); 
		  Lcd_Clear(GRAY0);
			Ready_Flusf=0;
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
	}
}

//显示确认码错误信息
void ShowErrMessage(uint8_t ensure)
{
	if(GUI_NUM!=5)
	{
		Gui_DrawFont_GBK16(10,150,BLUE,GRAY0,"Match Failed,Please Again!");
	}
	else
	{
		Gui_DrawFont_GBK16(10,80,BLUE,GRAY0,"Not exist this feature!");
	}
}

void delay_init()
{
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk;                         /* Function successful */
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	
	fac_us=SystemCoreClock/1000000;
	fac_ms=SystemCoreClock/1000;
}

void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
		
}


void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	g_systickCounter=nms;
	SysTick->LOAD=(uint32_t)fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		;
	}while(g_systickCounter!=0);		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 

void delay_us(uint32_t nus)
{		
	uint32_t temp;	   
	g_systickCounter=nus;
	SysTick->LOAD=fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		;
	}while(g_systickCounter!=0);		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}














