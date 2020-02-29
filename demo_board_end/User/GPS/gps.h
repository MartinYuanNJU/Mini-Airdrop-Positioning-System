#ifndef __GPS_H
#define __GPS_H	 
#include "sys.h"  
//********************************************************************************
//UBLOX NEO-M8N配置及解析文件
//哈尔滨工业大学电信学院
//V1.0  20150601
//包含的UBLOX NEO-M8N配置函数有
//1,Ublox_Cfg_Cfg_Save函数
//2,Ublox_Cfg_Msg函数
//3,Ublox_Cfg_Prt函数
//4,Ublox_Cfg_Rate函数
//5,Ublox_Cfg_GNSS函数
////////////////////////////////////////////////////////////////////////////////// 	   
 									   						    
//GPS NMEA-0183协议重要参数结构体定义 
//卫星信息
__packed typedef struct  
{										    
 	u8 num;		//卫星编号
	u8 eledeg;	//卫星仰角
	u16 azideg;	//卫星方位角
	u8 sn;		//信噪比		   
}nmea_slmsg;  
//UTC时间信息
__packed typedef struct  
{										    
 	u16 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
}nmea_utc_time;   	   
//NMEA 0183 协议解析后数据存放结构体
__packed typedef struct  
{										    
 	u8 svnum;					//可见卫星数
	nmea_slmsg slmsg[12];		//最多12颗卫星
	nmea_utc_time utc;			//UTC时间
	u32 latitude;				//纬度 分扩大100000倍,实际要除以100000
	u8 nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	u32 longitude;			    //经度 分扩大100000倍,实际要除以100000
	u8 ewhemi;					//东经/西经,E:东经;W:西经
	u8 gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
 	u8 posslnum;				//用于定位的卫星数,0~12.
 	u8 possl[12];				//用于定位的卫星编号
	u8 fixmode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
	u16 pdop;					//位置精度因子 0~500,对应实际值0~50.0
	u16 hdop;					//水平精度因子 0~500,对应实际值0~50.0
	u16 vdop;					//垂直精度因子 0~500,对应实际值0~50.0 

	int altitude;			 	//海拔高度,放大了10倍,实际除以10.单位:0.1m	 
	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
//UBLOX NEO-M8N 配置(清除,保存,加载等)结构体
__packed typedef struct  
{										    
 	u16 header;					//cfg header,固定为0X62B5(小端模式)
	u16 id;						//CFG CFG ID:0X0906 (小端模式)
	u16 dlength;				//数据长度 12/13
	u32 clearmask;				//子区域清除掩码(1有效)
	u32 savemask;				//子区域保存掩码
	u32 loadmask;				//子区域加载掩码
	u8  devicemask; 		  	//目标器件选择掩码	b0:BK RAM;b1:FLASH;b2,EEPROM;b4,SPI FLASH
	u8  cka;		 			//校验CK_A 							 	 
	u8  ckb;			 		//校验CK_B							 	 
}_ublox_cfg_cfg; 

//UBLOX NEO-M8N消息设置结构体
__packed typedef struct  
{										    
 	u16 header;					//cfg header,固定为0X62B5(小端模式)
	u16 id;						//CFG MSG ID:0X0106 (小端模式)
	u16 dlength;				//数据长度 8
	u8  msgclass;				//消息类型(F0 代表NMEA消息格式)
	u8  msgid;					//消息 ID 
								//00,GPGGA;01,GPGLL;02,GPGSA;
								//03,GPGSV;04,GPRMC;05,GPVTG;
								//06,GPGRS;07,GPGST;08,GPZDA;
								//09,GPGBS;0A,GPDTM;0D,GPGNS;
	u8  iicset;					//IIC消输出设置    0,关闭;1,使能.
	u8  uart1set;				//UART1输出设置	   0,关闭;1,使能.
	u8  uart2set;				//UART2输出设置	   0,关闭;1,使能.
	u8  usbset;					//USB输出设置	   0,关闭;1,使能.
	u8  spiset;					//SPI输出设置	   0,关闭;1,使能.
	u8  ncset;					//未知输出设置	   默认为1即可.
 	u8  cka;			 		//校验CK_A 							 	 
	u8  ckb;			    	//校验CK_B							 	 
}_ublox_cfg_msg; 

//UBLOX NEO-M8N 输入输出端口设置结构体
__packed typedef struct  
{										    
 	u16 header;					//cfg header,固定为0X62B5(小端模式)
	u16 id;						//CFG PRT ID:0X0006 (小端模式)
	u16 dlength;				//数据长度 20
	u8  portid;					//端口号,0=IIC;1=UART1;2=UART2;3=USB;4=SPI;
	u8  reserved1;				//保留,设置为0
	u16 txready;				//TX Ready引脚设置,默认为0
	u32 mode;					//串口工作模式设置,奇偶校验,停止位,字节长度等的设置.
 	u32 baudrate;				//波特率设置
 	u16 inprotomask;		 	//输入协议激活屏蔽位  默认设置为0X07 0X00即可.
 	u16 outprotomask;		 	//输出协议激活屏蔽位  默认设置为0X03 0X00即可.
 	u16 flags; 				//保留,设置为0
 	u16 reserved2; 				//保留,设置为0 
 	u8  cka;			 		//校验CK_A 							 	 
	u8  ckb;			    	//校验CK_B							 	 
}_ublox_cfg_prt; 

//UBLOX NEO-M8N 刷新速率配置结构体
__packed typedef struct  
{										    
 	u16 header;					//cfg header,固定为0X62B5(小端模式)
	u16 id;						//CFG RATE ID:0X0806 (小端模式)
	u16 dlength;				//数据长度
	u16 measrate;				//测量时间间隔，单位为ms，最少不能小于200ms（5Hz）
	u16 navrate;				//导航速率（周期），固定为1
	u16 timeref;				//参考时间：0=UTC Time；1=GPS Time；
 	u8  cka;					//校验CK_A 							 	 
	u8  ckb;					//校验CK_B							 	 
}_ublox_cfg_rate; 

//UBLOX NEO-M8N 卫星系统设置结构体
__packed typedef struct  
{										    
 	u16 header;					//cfg header,固定为0X62B5(小端模式)
	u16 id;						//CFG GNSS ID:0X3E06 (小端模式)
	u16 dlength;				//数据长度 4+8=12
	u8  msgver;					//信息版本，取0则为当前版本
	u8  numTrkChHw;				//只读信息，可用信道数
	u8	numTrkChUse;				//使用信道数目，小于可用信道数，只读信息？
	u8	numConfigBlocks;		//将要设置的卫星系统数量，此处取1，即每次设置1个卫星系统
 	u8	gnssId;				//卫星系统ID		00,GPS;01,SBAS;2,Galileo;03,BeiDou;04,IMES;05,QZSS;6,GLONASS;
 	u8	resTrkCh;		 	//最小追踪信道数目
 	u8	maxTrkCh;		 	//最大追踪信道数目
 	u8	reserved1; 				//保留,设置为0
 	u32	flags; 				//标志信号，等于0x01则打开相应的系统，等于0x00则关闭相应系统 
 	u8  cka;			 		//校验CK_A 							 	 
	u8  ckb;			    	//校验CK_B
}_ublox_cfg_gnss;

//UBLOX NEO-M8N 卫星系统设置结构体
__packed typedef struct  
{										    
	u16 header1;					//cfg header,固定为0X62B5(小端模式)
	u16 id1;						//CFG GNSS ID:0X3E06 (小端模式)
	u16 dlength1;				//数据长度 0
	u8  cka1;			 		//校验CK_A 							 	 
	u8  ckb1;			    	//校验CK_B
}_ublox_cfg_gnss1;

void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf);
u8 Ublox_Cfg_Cfg_Save(void);
u8 Ublox_Cfg_Msg(u8 msgid,u8 uart1set);
u8 Ublox_Cfg_Prt(u32 baudrate,u16 outprotomask);
u8 Ublox_Cfg_Rate(u16 measrate,u8 reftime);
u8 Ublox_Cfg_GNSS(u8 GNSSID,u8 enable);
u8 Ublox_Cfg_GNSS1(void);

void GPS_Init(void);
#endif  

 



