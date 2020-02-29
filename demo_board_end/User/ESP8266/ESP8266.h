#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stm32f10x.h"
//********************************************************************************
//ESP-01 WIFI模块设置文件
//哈尔滨工业大学电信学院
//V1.0  20150912
//包含的ESP8266置函数有
//1,ESP_Init函数,初始化ESP8266
//2,ESP_SEND函数,通过ESP模块发送数据

//////////////////////////////////////////////////////////////////////////////////
void ESP_ENABLE(u8 option);
u8* esp_check_cmd(u8 *str);
u8 esp_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void ESP_Init(void);
void ESP_SEND(char* DATA);

#endif 
