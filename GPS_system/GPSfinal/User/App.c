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
extern u8 TEMP_BUF1[USART1_MAX_RECV_LEN];		//����1�ݴ�����
extern u8 TEMP_BUF2[USART2_MAX_RECV_LEN];		//����2�ݴ�����
extern u8 TEMP_BUF3[USART3_MAX_RECV_LEN];		//����3�ݴ�����
extern u8 TEMP_BUF4[UART4_MAX_RECV_LEN];			//����4�ݴ�����

void Init_All(void)
{
	u8* p=TEMP_BUF4;
	SystemInit();
	delay_init();	    		//��ʱ������ʼ��
//	KEY_Init();
	delay_ms(100);

	
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�
	
 	USART1_Init(9600);		//��ʼ������1
	USART2_Init(9600);		//��ʼ������2
	USART3_Init(9600);		//��ʼ������3

	USART2_RX_STA=0;
}

//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//0XFF,�������ڵ�cx������							  
u8 Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//m^n����
//����ֵ:m^n�η�.
u32 Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//strת��Ϊ����,��','����'*'���߿ո���߻��з�����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*')||(*p==' ')||(*p==0x0D)||(*p==0))break;//����������
		if(*p=='.'){mask|=0X01;p++;}//����С������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	

//�Խ��յ�����ϢGPS��λ��Ϣ���н���
void usart1_analysis(void)
{
	u16 i,rxlen;
	//u2_printf("kaishi\n");
	if(USART1_RX_STA&0X8000)		//���յ�һ��������
	{
		//u2_printf("kaishi\n");
		rxlen=USART1_RX_STA&0X7FFF;	//�õ����ݳ���
		for(i=0;i<=rxlen;i++)
		{
		TEMP_BUF1[i]=USART1_RX_BUF[i];
		}
		u2_printf(POS);
		GPS_Analysis(&gpsx,TEMP_BUF1);		//�����ַ���
		//Gps_Msg_Show();
		GETPOS(POS);
		USART1_RX_STA=0x0000;
 	}
}

   //??????GPS?????????????
//$POS,????,??,??,??,??,??
void GETPOS(char POS[])
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
//	if (gnss_rate)
//	{
		//u2_printf((char *)POS);
//	}
}
