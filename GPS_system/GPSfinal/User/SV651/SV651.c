#include "SV651.h"
#include "usart1.h"
#include "usart2.h"	 
#include "usart3.h"
#include "uart4.h"
#include "delay.h"
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	

//��ʼ��SV651
//��ʼ������λΪPA1
//���ر����idֵ
u32 SV651_Init(void)
{
	char read_state[]={0xAA,0xFA,0x01,0x0D,0x0A,0x00};
	u32 id0;
	//��ʼ������λΪPA1
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	USART2_RX_STA=0;
	delay_ms(100);	//��ʱ100ms�ȴ���������ģʽ
	u2_printf(read_state);
	delay_ms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);//�ȴ�����빤��ģʽ
	if(USART2_RX_STA&0X8000)
	{ 
		if(USART2_RX_BUF[1]==0x01)
		{
			id0=USART2_RX_BUF[8] | USART2_RX_BUF[9]<<8 | USART2_RX_BUF[10]<<16 | USART2_RX_BUF[11]<<24;
//			u4_printf("SV651id��ȡ�ɹ�\r\n");
		}
//		else u4_printf("SV651id��ȡʧ��\r\n");
	} 
	USART2_RX_STA=0;
	return id0;
}

void SV651_SET(u32 netid)
{
	_sv *sv=(_sv *)USART2_TX_BUF;
	sv->header=0xFAAA;		//header,Ĭ��Ϊ0xFAAA
	sv->option=0x03;		//option, ֻѡ��0x03
	sv->channel=0x13;		//�ŵ�,��Ϊ0x22
	sv->band=0x01;			//Ƶ��,��Ϊ0x01
	sv->data_rate=0x03;	//����ͨ������,��Ϊ0x03
	sv->power=0x07;			//���书��,��Ϊ0x07
	sv->baud_rate=0x03;	//���ڲ�����,��Ϊ0x03
	sv->data_bit=0x02;	//��������λ,��Ϊ0x02
	sv->stop=0x01;			//����ֹͣλ,��Ϊ0x01
	sv->parity=0x01;		//����У��λ,��Ϊ0x01
	sv->net_id=netid;		//NET ID
	sv->node_id=0x0000;	//NODE ID,��Ϊ0x01
	sv->new_line=0x0A0D;//����λ��0x0A0D
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);	//��ʱ50ms�ȴ���������ģʽ
	
	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��14�������   
	UART_DMA_Enable(DMA1_Channel7,sizeof(_sv));//ͨ��dma���ͳ�ȥ
	
	delay_ms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	delay_ms(100);//�ȴ�����빤��ģʽ
//	if(USART2_RX_STA&0X8000)
//	{ 
//		if(strstr((const char*)USART2_RX_BUF,"OK"))
//		{
//			u4_printf("SV651���óɹ�\r\n");
//		}
//		else u4_printf("SV651����ʧ��\r\n");
//	} 
	USART2_RX_STA=0;
}
