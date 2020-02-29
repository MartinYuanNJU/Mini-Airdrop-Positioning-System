#ifndef __SV651_H
#define __SV651_H
#include "sys.h"
#include "stm32f10x.h"
//********************************************************************************
//无线数传模块设置文件
//哈尔滨工业大学电信学院
//V1.0  20150704
//包含的SV651配置函数有
//1,SV651_Init函数,初始化SV651
//2,SV651_NETID函数,设置netid
__packed typedef struct  
{										    
 	u16 header;		//header,默认为0xFAAA
	u8 option;		//option, 只选用03
	u8 channel;		//信道,设为0x22
	u8 band;			//频段,设为0x01
	u8 data_rate;	//空中通信速率,设为0x03
	u8 power;			//发射功率,设为0x07
	u8 baud_rate;	//串口波特率,设为0x03
	u8 data_bit;	//串口数据位,设为0x02
	u8 stop;			//串口停止位,设为0x01
	u8 parity;		//串口校验位,设为0x01
	u32 net_id;		//NET ID
	u16 node_id;	//NODE ID,设为0x0000
	u16 new_line;	//结束位，0x0A0D
}_sv; 

//////////////////////////////////////////////////////////////////////////////////
u32  SV651_Init(void);
void SV651_SET(u32 netid);

#endif 
