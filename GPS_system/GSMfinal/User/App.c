#include <string.h>
#include <stdio.h>
#include "App.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart1.h"
#include "usart2.h"	 
#include "usart3.h"
#include "uart4.h"
#include "sim900a.h"

extern u8 phone[17];
extern u32 id;
extern u16 flash;

extern u8 TEMP_BUF1[USART1_MAX_RECV_LEN];		//串口1暂存数组
extern u8 TEMP_BUF2[USART2_MAX_RECV_LEN];		//串口2暂存数组
extern u8 TEMP_BUF3[USART3_MAX_RECV_LEN];		//串口3暂存数组
extern u8 TEMP_BUF4[UART4_MAX_RECV_LEN];			//串口4暂存数组

void Init_All(void)
{
	u8* p=TEMP_BUF4;
	SystemInit();
	delay_init();	    		//延时函数初始化
//	KEY_Init();
	delay_ms(100);

	
	LED_Init();						//初始化与LED连接的硬件接口
	
 	USART1_Init(9600);		//初始化串口1
	USART2_Init(9600);		//初始化串口2
	USART3_Init(9600);		//初始化串口3
	
	

	sim900a_init();		//初始化sim900a
	USART2_RX_STA=0;
}

//从buf里面得到第cx个逗号所在的位置
//返回值:0~0XFE,代表逗号所在位置的偏移.
//0XFF,代表不存在第cx个逗号							  
u8 Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//m^n函数
//返回值:m^n次方.
u32 Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//str转换为数字,以','或者'*'或者空格或者换行符结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*')||(*p==' ')||(*p==0x0D)||(*p==0))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	
