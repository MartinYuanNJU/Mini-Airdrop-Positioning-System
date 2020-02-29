#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#include "usart3.h"

#define max_message_num 50 //最大短信条数

extern u8 POS[100];
extern u8 POS1[100];

void sim_at_response(u8 mode);	


u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

void sim900a_init(void);			//sim900a模块连接检查初始化
u8 sim900a_enmessage_mode_out(void);	//让GSM板收到的英文或数字短信不转码

u8* sim900a_sms_read(u8 index);	//读短信
u8 sim900a_sms_send(u8* phone_number,u8 *str);	//发短信
u8 SIM_HANDLE_MESSAGE_CMD(void);

void sim900a_sms_analysis(u8 NewMseeage_Num);		//分析短信指令并且执行相应操作
void sim900a_uart_analysis(u8*buf);		//分析串口消息是否是电话铃音和收到短息的消息

//int gprs(double latitude,double longitude,double altitude,double speed);
int gprs(void);
#endif





