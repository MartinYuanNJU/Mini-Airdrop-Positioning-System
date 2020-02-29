#include "SV651.h"
#include "usart1.h"
#include "usart2.h"	 
#include "usart3.h"
#include "uart4.h"
#include "delay.h"
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	

//初始化SV651
//初始化设置位为PA1
//返回保存的id值
u32 SV651_Init(void)
{
	char read_state[]={0xAA,0xFA,0x01,0x0D,0x0A,0x00};
	u32 id0;
	//初始化设置位为PA1
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	USART2_RX_STA=0;
	delay_ms(100);	//延时100ms等待进入设置模式
	u2_printf(read_state);
	delay_ms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);//等待其进入工作模式
	if(USART2_RX_STA&0X8000)
	{ 
		if(USART2_RX_BUF[1]==0x01)
		{
			id0=USART2_RX_BUF[8] | USART2_RX_BUF[9]<<8 | USART2_RX_BUF[10]<<16 | USART2_RX_BUF[11]<<24;
//			u4_printf("SV651id读取成功\r\n");
		}
//		else u4_printf("SV651id读取失败\r\n");
	} 
	USART2_RX_STA=0;
	return id0;
}

void SV651_SET(u32 netid)
{
	_sv *sv=(_sv *)USART2_TX_BUF;
	sv->header=0xFAAA;		//header,默认为0xFAAA
	sv->option=0x03;		//option, 只选用0x03
	sv->channel=0x13;		//信道,设为0x22
	sv->band=0x01;			//频段,设为0x01
	sv->data_rate=0x03;	//空中通信速率,设为0x03
	sv->power=0x07;			//发射功率,设为0x07
	sv->baud_rate=0x03;	//串口波特率,设为0x03
	sv->data_bit=0x02;	//串口数据位,设为0x02
	sv->stop=0x01;			//串口停止位,设为0x01
	sv->parity=0x01;		//串口校验位,设为0x01
	sv->net_id=netid;		//NET ID
	sv->node_id=0x0000;	//NODE ID,设为0x01
	sv->new_line=0x0A0D;//结束位，0x0A0D
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);	//延时50ms等待进入设置模式
	
	while(DMA1_Channel7->CNDTR!=0);	//等待通道14传输完成   
	UART_DMA_Enable(DMA1_Channel7,sizeof(_sv));//通过dma发送出去
	
	delay_ms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);//等待其进入工作模式
//	if(USART2_RX_STA&0X8000)
//	{ 
//		if(strstr((const char*)USART2_RX_BUF,"OK"))
//		{
//			u4_printf("SV651设置成功\r\n");
//		}
//		else u4_printf("SV651设置失败\r\n");
//	} 
	USART2_RX_STA=0;
}
