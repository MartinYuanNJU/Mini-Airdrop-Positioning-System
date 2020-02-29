#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#include "usart3.h"

#define max_message_num 50 //����������

extern u8 POS[100];
extern u8 POS1[100];

void sim_at_response(u8 mode);	


u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

void sim900a_init(void);			//sim900aģ�����Ӽ���ʼ��
u8 sim900a_enmessage_mode_out(void);	//��GSM���յ���Ӣ�Ļ����ֶ��Ų�ת��

u8* sim900a_sms_read(u8 index);	//������
u8 sim900a_sms_send(u8* phone_number,u8 *str);	//������
u8 SIM_HANDLE_MESSAGE_CMD(void);

void sim900a_sms_analysis(u8 NewMseeage_Num);		//��������ָ���ִ����Ӧ����
void sim900a_uart_analysis(u8*buf);		//����������Ϣ�Ƿ��ǵ绰�������յ���Ϣ����Ϣ

//int gprs(double latitude,double longitude,double altitude,double speed);
int gprs(void);
#endif





