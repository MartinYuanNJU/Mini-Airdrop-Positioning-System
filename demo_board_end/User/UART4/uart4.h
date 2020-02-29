#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"     

#define UART4_MAX_RECV_LEN		1024					//�����ջ����ֽ���
#define UART4_MAX_SEND_LEN		1024					//����ͻ����ֽ���
#define UART4_RX_EN 			1					//0,������;1,����.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���USART4_MAX_RECV_LEN�ֽ�
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//���ͻ���,���USART4_MAX_SEND_LEN�ֽ�
extern u16 UART4_RX_STA;   						//��������״̬

void UART4_Init(u32 bound);				//����4��ʼ�� 
void TIM5_Set(u8 sta);
void TIM5_Init(u16 arr,u16 psc);
char * itoa(int value,char * string,int radix);
void USART_printf(USART_TypeDef * USARTx,uint8_t *Data,...);
void u4_printf(char *Data);
#endif
