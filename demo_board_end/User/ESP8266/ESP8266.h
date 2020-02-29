#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stm32f10x.h"
//********************************************************************************
//ESP-01 WIFIģ�������ļ�
//��������ҵ��ѧ����ѧԺ
//V1.0  20150912
//������ESP8266�ú�����
//1,ESP_Init����,��ʼ��ESP8266
//2,ESP_SEND����,ͨ��ESPģ�鷢������

//////////////////////////////////////////////////////////////////////////////////
void ESP_ENABLE(u8 option);
u8* esp_check_cmd(u8 *str);
u8 esp_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void ESP_Init(void);
void ESP_SEND(char* DATA);

#endif 
