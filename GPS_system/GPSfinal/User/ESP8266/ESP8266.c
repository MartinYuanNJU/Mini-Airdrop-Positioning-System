#include "usart1.h"
#include "usart2.h"	 
#include "usart3.h"
#include "uart4.h"
#include "delay.h"
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"
#include "ESP8266.h"

u8 ESP_TEMP_BUF[100];
u8 slave_mode=1;
//是否启用ESP8266的AT指令模式
//option:	0不启动	1启动
void ESP_ENABLE(u8 option)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	if(option) GPIO_SetBits(GPIOC, GPIO_Pin_12);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_12);
	

}
u8* esp_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	UART4_RX_STA=0;
	return (u8*)strx;
}

//向esp8266发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 esp_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	UART4_RX_STA=0;
	printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(UART4_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp_check_cmd(ack))break;//得到有效数据 
				UART4_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 



void ESP_Init(void)
{
	u8 flag=0;
	UART4_RX_STA=0;
	while(esp_send_cmd("AT","OK",200))
	{
			flag++;
			if (flag==2)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_12);
				delay_ms(200);
				GPIO_SetBits(GPIOC, GPIO_Pin_12);
				delay_ms(500);
			}
			if(flag>=5) break;
	}
	if (flag<5)
	{
		slave_mode=0;
		esp_send_cmd("AT+CWMODE=2","OK",200);
		esp_send_cmd("AT+CWSAP=\"HANDER\",\"88888888\",11,4","OK",200);
		esp_send_cmd("AT+CIPMUX=1","OK",200);
		esp_send_cmd("AT+CIPSERVER=1,4747","OK",200);
		esp_send_cmd("AT+CIPSTO=0","OK",200);
		UART4_RX_STA=0;
		while(1)
		{
			if(UART4_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp_check_cmd("CONNECT"))break;//得到有效数据 
				UART4_RX_STA=0;
			} 
		}
	}
}

void ESP_SEND(char* DATA)
{
	int LEN;
	if(!slave_mode)
	{
		u8 *p=ESP_TEMP_BUF;
		LEN=strlen(DATA);
		sprintf((char*)p,"AT+CIPSEND=0,%d\0",LEN);
		esp_send_cmd(p,"OK",200);
		sprintf((char*)p,"%s\0",DATA);
		esp_send_cmd(p,"SEND OK",200);
	}
}
