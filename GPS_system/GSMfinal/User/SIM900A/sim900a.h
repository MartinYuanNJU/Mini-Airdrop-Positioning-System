#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#include "usart3.h"

#define max_message_num 50 //����������

#define SIM_OK 0
#define SIM_COMMUNTION_ERR 0xff
#define SIM_CPIN_ERR 0xfe
#define SIM_CREG_FAIL 0xfd
#define SIM_MAKE_CALL_ERR 0Xfc
#define SIM_ATA_ERR 0xfb

#define SIM_CMGF_ERR 0xfa
#define SIM_CSCS_ERR 0xf9
#define SIM_CSCA_ERR 0xf8
#define SIM_CSMP_ERR 0Xf7
#define SIM_CMGS_ERR 0xf6
#define SIM_CMGS_SEND_FAIL	0xf5

extern u8 POS[100];

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
#endif





