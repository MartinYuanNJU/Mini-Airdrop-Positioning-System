#ifndef __SV651_H
#define __SV651_H
#include "sys.h"
#include "stm32f10x.h"
//********************************************************************************
//��������ģ�������ļ�
//��������ҵ��ѧ����ѧԺ
//V1.0  20150704
//������SV651���ú�����
//1,SV651_Init����,��ʼ��SV651
//2,SV651_NETID����,����netid
__packed typedef struct  
{										    
 	u16 header;		//header,Ĭ��Ϊ0xFAAA
	u8 option;		//option, ֻѡ��03
	u8 channel;		//�ŵ�,��Ϊ0x22
	u8 band;			//Ƶ��,��Ϊ0x01
	u8 data_rate;	//����ͨ������,��Ϊ0x03
	u8 power;			//���书��,��Ϊ0x07
	u8 baud_rate;	//���ڲ�����,��Ϊ0x03
	u8 data_bit;	//��������λ,��Ϊ0x02
	u8 stop;			//����ֹͣλ,��Ϊ0x01
	u8 parity;		//����У��λ,��Ϊ0x01
	u32 net_id;		//NET ID
	u16 node_id;	//NODE ID,��Ϊ0x0000
	u16 new_line;	//����λ��0x0A0D
}_sv; 

//////////////////////////////////////////////////////////////////////////////////
u32  SV651_Init(void);
void SV651_SET(u32 netid);

#endif 
