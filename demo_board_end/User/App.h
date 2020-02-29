#ifndef __App_H
#define __App_H

#include "stm32f10x.h"

void Init_All(void);

u8 Comma_Pos(u8 *buf,u8 cx);
u32 Pow(u8 m,u8 n);
int Str2num(u8 *buf,u8*dx);

void Gps_Msg_Show(void);	//显示GPS定位信息	 
void GETPOS(char POS[],char POS1[]);
void usart1_analysis(void);
void usart2_analysis(void);
void usart2_master_analysis(void);
void usart2_slave_analysis(void);

void usart3_analysis(void);

void uart4_analysis(void);
void uart4_set_analysis(void);
void uart4_mode_analysis(void);
#endif
