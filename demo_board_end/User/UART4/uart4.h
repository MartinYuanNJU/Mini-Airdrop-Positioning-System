#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"     

#define UART4_MAX_RECV_LEN		1024					//最大接收缓存字节数
#define UART4_MAX_SEND_LEN		1024					//最大发送缓存字节数
#define UART4_RX_EN 			1					//0,不接收;1,接收.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大USART4_MAX_RECV_LEN字节
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//发送缓冲,最大USART4_MAX_SEND_LEN字节
extern u16 UART4_RX_STA;   						//接收数据状态

void UART4_Init(u32 bound);				//串口4初始化 
void TIM5_Set(u8 sta);
void TIM5_Init(u16 arr,u16 psc);
char * itoa(int value,char * string,int radix);
void USART_printf(USART_TypeDef * USARTx,uint8_t *Data,...);
void u4_printf(char *Data);
#endif
