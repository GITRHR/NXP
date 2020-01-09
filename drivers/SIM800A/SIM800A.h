#ifndef SIM800A__H
#define SIM800A__H


#include "fsl_gpio.h"
#include "fsl_usart.h"
#include "string.h"

#define Buf2_Max 200
#define UART2_SendLR() UART2_SendString("\r\n")


/*************	本地函数声明	**************/
void CLR_Buf2(void);
uint8_t Find(char *a);
void Second_AT_Command(char *b,char *a,uint8_t wait_time);
void Wait_CREG(void);
void Set_Pdu_Mode(void);//设置短信模式PDU
void Send_Pdu_Sms(void);//发送一条中文短信
int Send_Chinese_Message();












#endif