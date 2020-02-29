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
#include "gps.h"


extern nmea_msg gpsx;
extern u8 gnss_num,gnss_mode;
extern u16 gnss_rate;
extern u8 phone[17];
extern u32 id;
extern u16 flash;

extern const u8*fixmode_tbl[4];
extern const u8*fixmode_tbl_eng[4];
extern const u8*gnss_tbl[7];

extern u8 POS[100];
extern u8 POS1[100];
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
	
	

	//GPS_Init();				//初始化GPS设置
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

//将已经截获的GPS定位信进行整合
void Gps_Msg_Show(void)
{
	char *p=(char *)TEMP_BUF4;
    //char *p1=(char *)TEMP_BUF4;
 	float tp;
	if(gpsx.fixmode<=3)														//定位状态
	{
		//u2_printf("当前位置信息如下:\r\n");
		sprintf(p,"zhuangtai:%s \r\n",fixmode_tbl[gpsx.fixmode]);
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
		u3_printf(p);
		sprintf(p,"weiingshu:%02d \r\n",gpsx.posslnum);	 		//用于定位的卫星数 
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u3_printf(p);
		sprintf(p,"kejianweixingshu:%02d \r\n",gpsx.svnum%100);	 		//可见卫星数		 				
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u3_printf(p);
        tp=gpsx.longitude;	   
		sprintf(p,"jingdu:%.5f %1c \r\n",tp/=100000,gpsx.ewhemi);	//得到经度字符   
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u2_printf(p);
        tp=gpsx.latitude;	   
		sprintf(p,"weidu:%.5f %1c \r\n",tp/=100000,gpsx.nshemi);	//得到纬度字符串	 	 
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u2_printf(p);
        tp=gpsx.altitude;	   
		sprintf(p,"gaodu:%.1fm \r\n",tp/=10);	    			//得到高度字符串	 			   
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u2_printf(p);
        tp=gpsx.speed;	   
		sprintf(p,"sudu:%.3fkm/h \r\n",tp/=1000);		    		//得到速度字符串
        sim900a_send_cmd((u8 *)p,(u8 *)"",100);
        u2_printf(p);        
	}
	else u2_printf("定位模块存在问题,定位状态错误\r\n");
}




//对接收到的信息GPS定位信息进行解析
void usart1_analysis(void)
{
	u16 i,rxlen;
	//u2_printf("kaishi\n");
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{
		//u2_printf("kaishi\n");
		rxlen=USART1_RX_STA&0X7FFF;	//得到数据长度
		for(i=0;i<=rxlen;i++)
		{
		TEMP_BUF1[i]=USART1_RX_BUF[i];
		}

		GPS_Analysis(&gpsx,TEMP_BUF1);		//分析字符串
		//Gps_Msg_Show();
		GETPOS(POS,POS1);
		u2_printf(POS);
		USART1_RX_STA=0x0000;
 	}
}

   //??????GPS?????????????
//$POS,????,??,??,??,??,??
void GETPOS(char POS[],char POS1[])
{
	float tp;
//	Gps_Msg_Show();
	if(gpsx.fixmode<=3)
	{
		
		sprintf(POS,"%s",fixmode_tbl_eng[gpsx.fixmode]);//????
//		sprintf((char*)POS,"%s,%04d/%02d/%02d",POS,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//UTC??   
//		sprintf((char*)POS,"%s,%02d:%02d:%02d",POS,gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//UTC??	
		tp=(float)gpsx.longitude/100000.0;
		sprintf(POS,"%s,Long:%.2f%1c",POS,tp,gpsx.ewhemi);//??
		tp=(float)gpsx.latitude/100000.0;
		sprintf(POS,"%s,Lati:%.2f%1c",POS,tp,gpsx.nshemi);//??
		tp=(float)gpsx.altitude/10.0;
		sprintf(POS,"%s,Alti:%.2fm",POS,tp);//??
		tp=(float)gpsx.speed/1000.0;
		sprintf(POS,"%s,Speed:%.2fkm/h",POS,tp);//??
	}
	if(gpsx.fixmode<=3)
	{
		int g = 1;
		char end =0x1A;
//GET /student/update/?group=%d&longitude=%f&latitude=%f&altitude=%f&speed=%f%c HTTP/1.1",g,longitude,latitude,altitude,speed
		sprintf(POS1,"GET /student/update/?group=%d",g);//????
//		sprintf((char*)POS,"%s,%04d/%02d/%02d",POS,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//UTC??   
//		sprintf((char*)POS,"%s,%02d:%02d:%02d",POS,gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//UTC??	
		tp=(float)gpsx.longitude/100000.0;
		sprintf(POS1,"%s&longitude=%.2f",POS1,tp);//??,gpsx.ewhemi
		tp=(float)gpsx.latitude/100000.0;
		sprintf(POS1,"%s&latitude=%.2f",POS1,tp);//??,gpsx.nshemi
		tp=(float)gpsx.altitude/10.0;
		sprintf(POS1,"%s&height=%.2f",POS1,tp);//??
		tp=(float)gpsx.speed/1000.0;
		sprintf(POS1,"%s&speed=%.2f",POS1,tp);//??
		sprintf(POS1,"%s %c HTTP/1.1",POS1,end);//??
	}
//	if (gnss_rate)
//	{
		//u2_printf((char *)POS);
//	}
}
