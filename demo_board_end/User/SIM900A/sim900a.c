#include <stdio.h>
#include <string.h>
#include "App.h"
#include "led.h"
#include "sim900a.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
//#include "bsp_lcd.h"

u8 GSM_SEND_BUF[USART3_MAX_SEND_LEN];	//短信发送缓冲区
u8 GSM_BUF [50];		//数据处理防溢出缓冲区
//char GSM_ANALYSIS_BUF[USART3_MAX_RECV_LEN];
u8 GSM_ANALYSIS_BUF[USART3_MAX_RECV_LEN];
u8 checknum[11][17];
u8 num_count = 0; //当前已保存号码总数
extern u8 POS[100];
extern u8 POS1[100];
extern u16 flash;
extern u32 id;
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void sim_at_response(u8 mode)
{
    u16 i,rxlen;
		u8 *p = GSM_SEND_BUF;
    if(USART3_RX_STA & 0X8000)		//接收到一次数据了
    {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
        sprintf((char*)p, (const char*)"%s/n", USART3_RX_BUF);	//发送到串口
        u2_printf((char*)p);
        if(mode)USART3_RX_STA = 0;
			
		rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度

		sim900a_uart_analysis(USART3_RX_BUF);
		USART3_RX_STA=0x0000;
    }
		
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//SIM900A 各项测试共用代码

//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
    char *strx = 0;
    if(USART3_RX_STA & 0X8000)		//接收到一次数据了
    {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
        strx = strstr((const char*)USART3_RX_BUF, (const char*)str);
        u2_printf((char*)USART3_RX_BUF);
    }
    USART3_RX_STA = 0;
    return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim900a_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART3_RX_STA = 0;
    if((u32)cmd <= 0XFF)
    {
        while(DMA1_Channel2->CNDTR != 0);	//等待通道2传输完成
        USART3->DR = (u32)cmd;
    } else u3_printf("%s\r", cmd); //发送命令
    if(ack && waittime)		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            delay_ms(10);
            if(USART3_RX_STA & 0X8000) //接收到期待的应答结果
            {
                if(sim900a_check_cmd(ack))break;//得到有效数据
                USART3_RX_STA = 0;
            }
        }
        if(waittime == 0)res = 1;
    }
    return res;
}



//连接并初始化sim900a
//执行顺序为：连接，设置不回显模式，工作状态检查，
//关闭来显，

void sim900a_init(void)
{
    u8 flag = 0;
    while(sim900a_send_cmd("AT", "OK", 200)) //检测是否应答AT指令
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL1\r\n");
            break;
        }
    }
    flag = 0;
    while(sim900a_send_cmd("AT+CPIN?", "READY", 200)) //不回显
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL2\r\n");
            break;
        }
    }
    flag = 0;
    while(sim900a_send_cmd("AT+CREG?", "0,1", 200)) //不回显
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL3\r\n");
            break;
        }
    }
    u2_printf("success\r\n");
}

//发送*str指向的字符串
//发送成功发回1，失败返回0
u8 sim900a_enmessage_mode_out(void)	//让GSM板收到的英文或数字短信不转码
{
    int flag = 0;
    while(sim900a_send_cmd("AT+CMGF=1", "OK", 200)) //不回显
    {
        flag++;
				delay_ms(100);
				if (flag >= 10)
				{
						u2_printf("out FAIL\r\n");
						return 0;
						break;
				}
    }
		flag = 0;
    while(sim900a_send_cmd("AT+CSCS=\"GSM\"", "OK", 200)) //不回显
    {
        flag++;
				delay_ms(100);
				if (flag >= 10)
				{
						u2_printf("out FAIL\r\n");
						return 0;
						break;
				}
    }
		flag = 0;
		while(sim900a_send_cmd("AT+CNMI=2,2", "OK", 200)) //不回显
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("out FAIL\r\n");
            return 0;
            break;
        }
		}
    return 1;
		delay_ms(100);
}


u8 SIM_HANDLE_MESSAGE_CMD(void)
{
		//u2_printf((char*)USART3_RX_BUF);
	    u16 i,rxlen;
			//int y = 0;
			u8 *p = GSM_SEND_BUF;

			if(USART3_RX_STA & 0X8000)		//接收到一次数据了
			{
					USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
					sprintf((char*)p, (const char*)"%s/n", USART3_RX_BUF);	//发送到串口
					u2_printf((char*)p);
			
			rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度

			sim900a_uart_analysis(USART3_RX_BUF);
			USART3_RX_STA=0x0000;
				
					USART3_RX_STA = 0;
			}
		if(strstr((char*)USART3_RX_BUF, "18846832453") != NULL)//13091895372
    {
    if(strstr((char*)USART3_RX_BUF, "LED1") != NULL)
    {
        return 1;
    }
    else if(strstr((char*)USART3_RX_BUF, "LED2") != NULL)
    {
        return 2;
    }
    else if(strstr((char*)USART3_RX_BUF, "RELAY1") != NULL)
    {
        return 3;
    }
		else if(strstr((char*)USART3_RX_BUF, "RELAY2") != NULL)
    {
        return 4;
    }
		else if(strstr((char*)USART3_RX_BUF, "RELAY3") != NULL)
    {
        return 5;
    }
		else if(strstr((char*)USART3_RX_BUF, "RELAY4") != NULL)
    {
        return 6;
    }
    else if(strstr((char*)USART3_RX_BUF, "pos") != NULL)
    {
        return 7;
    }
    else if(strstr((char*)USART3_RX_BUF, "GPRS") != NULL)
    {
        return 8;
    }
	}
    return 0;
}
//读短信
//若接收成功则将短信内容存到GSM_ANALYSIS_BUF里面并且返回地址，以便后续处理
//若接收失败则将返回空
//若是短信数量达到45则在读取短信成功之后删除所有短信
//AT+CMGD=1,4
u8* sim900a_sms_read(u8 index)
{
	u16 i;
	u8 *p=GSM_BUF;
	sprintf((char*)p,"AT+CMGR=%d\0",index);
//	ESP_SEND((char*)p);
//	ESP_SEND("\r\n");
	if (sim900a_send_cmd(p,"+CMGR:",200)==0)
	{
		for (i=0;USART3_RX_BUF[i]!=0;i++)
		{
			GSM_ANALYSIS_BUF[i]=USART3_RX_BUF[i];
		}
		GSM_ANALYSIS_BUF[i]=0;
		//if(index>=45) sim900a_send_cmd("AT+CMGDA=\"DEL ALL\"","OK",300);
		if(index>=45) sim900a_send_cmd("AT+CMGD=1,1","OK",300);
		USART3_RX_STA=0;
		return GSM_ANALYSIS_BUF;
	}
	return NULL;
}
//发送*str指向的字符串
//发送成功发回1，失败返回0
u8 sim900a_sms_send(u8* phone_number, u8 *str)
{
    u8 *p = GSM_BUF;
    sprintf((char*)p, "AT+CMGS=\"%s\"", phone_number);
    //u2_printf(p);

    if(sim900a_send_cmd("AT+CMGF=1", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSCS=\"GSM\"", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSCA?", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSMP=17,167,0,25", "OK", 100))	return 0;
    //sim900a_send_cmd(p,"",200);
    if(sim900a_send_cmd(p, ">", 200) == 0)					//发送短信命令+电话号码
    {
        sim900a_send_cmd(str, "", 200);		 						//发送短信内容到GSM模块
//发送结束符,等待发送完成(最长等待10秒钟,因为短信长了的话,等待时间会长一些)
        if(sim900a_send_cmd((u8*)0X1A, "+CMGS:", 3000) == 0)
        {
            return 1;
        }
    }
    return 0;
}

//usart3收到的消息进行处理
void sim900a_uart_analysis(u8*buf)
{
	u8 NewMseeage_Num=0;
	u8 dx;
	u8 *p;
	//判断是否是收到了新的短信或者电话来电
	if(strstr((const char *)buf,"+CMTI"))//判断收到的是否为短信消息
	{		//解析得出消息条目数
		p=(u8*)strstr((const char *)buf,",");
		NewMseeage_Num=Str2num(p+1,&dx);	
	}
	if(strstr((const char *)buf,"RING"))//判断收到的是否为电话消息
	{		//执行挂电话处理
		sim900a_send_cmd("ATH","OK",200);
		USART3_RX_STA=0;
	}
	if(NewMseeage_Num)
	{
//		ESP_SEND("收到短信：%d\r\n",NewMseeage_Num);
		//ILI9341_DispString_EN ( 0, 150, NewMseeage_Num, macBACKGROUND, macGREEN  );
		buf=sim900a_sms_read(NewMseeage_Num);		//收到短信则读取短信并分析
		NewMseeage_Num=0;
	}
}

//返回值:0,发送成功(得到了期待的应答结果)
//       其他正数，发送失败
//int gprs(double latitude,double longitude,double altitude,double speed)
int gprs(void)
{
	char end =0x1A;
	int g = 1;
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",500))
		return 1;
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",500))
		return 2;
	if(sim900a_send_cmd("AT+CGATT=1","OK",500))
		return 3;
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))
		return 4;
	if(sim900a_send_cmd("AT+CIPSTART=\"TCP\",\"129.204.221.68\",\"8000\"","CONNECT OK",500))
		return 5;
	if(sim900a_send_cmd("AT+CIPSEND",">",500))
		return 6;
	//u8 *p1;
	//char pp = "GET/student/update/?group=%d&longitude=%f&latitude=%f&altitude=%f&speed=%f%c HTTP/1.1",g,longitude,latitude,altitude,speed,end;
	//p1 =(u8*)"GET/student/update/?group=%d&longitude=%f&latitude=%f&altitude=%f&speed=%f%c HTTP/1.1",g,longitude,latitude,altitude,speed,end;
	//sim900a_send_cmd("GET/student/update/?group=0&longitude=0&latitude=0&altitude=0speed=0%c HTTP/1.1", "", 200);
	if(sim900a_send_cmd(POS1,"SEND OK",500))
		return 6;
	//u3_printf(POS1);
	//printf("%f,%f%c",longitude,latitude,end);
	if(sim900a_send_cmd("AT+CIPCLOSE=1","OK",500))
		return 7;
	if(sim900a_send_cmd("AT+CIPSHUT","OK",500))
		return 8;
	return 0;
}
