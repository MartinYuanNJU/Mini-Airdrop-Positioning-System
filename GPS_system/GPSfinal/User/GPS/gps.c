#include <stdio.h>
#include <string.h>
#include "App.h"
#include "gps.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"

//********************************************************************************
//UBLOX NEO-M8N���ü������ļ�
//��������ҵ��ѧ����ѧԺ
//V1.0  20150601
//������UBLOX NEO-M8N���ú�����
//1,Ublox_Cfg_Cfg_Save����
//2,Ublox_Cfg_Msg����
//3,Ublox_Cfg_Prt����
//4,Ublox_Cfg_Rate����
//5,Ublox_Cfg_GNSS����
////////////////////////////////////////////////////////////////////////////////// 	   

/*�˶δ�����Ϊ������������app.c����,����ʹ�õ�ʱ��ȡ��ע�ͼ���
//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
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
		ires+=sim900a_pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=sim900a_pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*sim900a_pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	
 */
 
 //����GSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;   
	p1=(u8*)strstr((const char *)buf,"$GPGSA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GNGSA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GLGSA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GBGSA");
	posx=Comma_Pos(p1,2);								//�õ���λ����
	if(posx!=0XFF)gpsx->fixmode=Str2num(p1+posx,&dx);	
	if(gpsx->fixmode>3) gpsx->fixmode=0;
	for(i=0;i<12;i++)										//�õ���λ���Ǳ��
	{
		posx=Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=Comma_Pos(p1,15);								//�õ�PDOPλ�þ�������
	if(posx!=0XFF)gpsx->pdop=Str2num(p1+posx,&dx);  
	posx=Comma_Pos(p1,16);								//�õ�HDOPλ�þ�������
	if(posx!=0XFF)gpsx->hdop=Str2num(p1+posx,&dx);  
	posx=Comma_Pos(p1,17);								//�õ�VDOPλ�þ�������
	if(posx!=0XFF)gpsx->vdop=Str2num(p1+posx,&dx);  
}
//����GSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"$GPGSV");
	if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GNGSV");
	if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GLGSV");
	if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GBGSV");
	len=p1[7]-'0';								//�õ�GPGSV������
	posx=Comma_Pos(p1,3); 					//�õ��ɼ���������
	if(posx!=0XFF)gpsx->svnum=Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"$GPGSV");
		if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GNGSV");
		if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GLGSV");
		if (p1==NULL) p1=(u8*)strstr((const char *)p,"$GBGSV");		
		for(j=0;j<4;j++)
		{	  
			posx=Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=Str2num(p1+posx,&dx);	//�õ����Ǳ��
			else break; 
			posx=Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=Str2num(p1+posx,&dx);//�õ��������� 
			else break;
			posx=Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
			else break; 
			posx=Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=Str2num(p1+posx,&dx);	//�õ����������
			else break;
			slx++;	   
		}   
 		p=p1+1;//�л�����һ��GPGSV��Ϣ
	}   
}
//����GGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPGGA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GNGGA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GLGGA");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GBGGA");
	posx=Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF)gpsx->gpssta=Str2num(p1+posx,&dx);	
	posx=Comma_Pos(p1,7);								//�õ����ڶ�λ��������
	if(posx!=0XFF)gpsx->posslnum=Str2num(p1+posx,&dx); 
	posx=Comma_Pos(p1,9);								//�õ����θ߶�
	if(posx!=0XFF && gpsx->fixmode>=2)gpsx->altitude=Str2num(p1+posx,&dx);  
}

//����RMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_RMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs;  
	p1=(u8*)strstr((const char *)buf,"GPRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"GNRMC");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"GLRMC");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"GBRMC");
	posx=Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=Str2num(p1+posx,&dx)/Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour=(temp/10000);
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=Comma_Pos(p1,3);								//�õ�γ��
	if(posx!=0XFF && gpsx->fixmode>=2)
	{
		temp=Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/Pow(10,dx+2);	//�õ���
		rs=temp%Pow(10,dx+2);				//�õ�'		 
		gpsx->latitude=gpsx->latitude*Pow(10,5)+(rs*Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=Comma_Pos(p1,4);								//��γ���Ǳ�γ 
	if(posx!=0XFF && gpsx->fixmode>=2)gpsx->nshemi=*(p1+posx);					 
 	posx=Comma_Pos(p1,5);								//�õ�����
	if(posx!=0XFF && gpsx->fixmode>=2)
	{												  
		temp=Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/Pow(10,dx+2);	//�õ���
		rs=temp%Pow(10,dx+2);				//�õ�'		 
		gpsx->longitude=gpsx->longitude*Pow(10,5)+(rs*Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=Comma_Pos(p1,6);								//������������
	if(posx!=0XFF && gpsx->fixmode>=2)gpsx->ewhemi=*(p1+posx);		 
	posx=Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=Str2num(p1+posx,&dx);		 				//�õ�UTC����
		gpsx->utc.date=temp/10000;
		if(gpsx->utc.hour>=16) gpsx->utc.date++;
		gpsx->utc.hour=(gpsx->utc.hour+8)%24;			//����ʱ������
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
}
//����VTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_VTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPVTG");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GNVTG");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GLVTG");
	if (p1==NULL) p1=(u8*)strstr((const char *)buf,"$GBVTG");
	posx=Comma_Pos(p1,7);								//�õ���������
	if(posx!=0XFF)
	{
		gpsx->speed=Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=Pow(10,3-dx);	 	 		//ȷ������1000��
	}
}  
//��ȡNMEA-0183��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	NMEA_GSA_Analysis(gpsx,buf);	//GPGSA����,�ȵõ���λ����
	NMEA_GSV_Analysis(gpsx,buf);	//GPGSV����
	NMEA_GGA_Analysis(gpsx,buf);	//GPGGA���� 	
	NMEA_RMC_Analysis(gpsx,buf);	//GPRMC����
	NMEA_VTG_Analysis(gpsx,buf);	//GPVTG����
}

//GPSУ��ͼ���
//buf:���ݻ������׵�ַ
//len:���ݳ���
//cka,ckb:����У����.
void Ublox_CheckSum(u8 *buf,u16 len,u8* cka,u8*ckb)
{
	u16 i;
	*cka=0;*ckb=0;
	for(i=0;i<len;i++)
	{
		*cka=*cka+buf[i];
		*ckb=*ckb+*cka;
	}
}
/////////////////////////////////////////UBLOX ���ô���/////////////////////////////////////
//���CFG����ִ�����
//����ֵ:0,ACK�ɹ�
//       1,���ճ�ʱ����
//       2,û���ҵ�ͬ���ַ�
//       3,���յ�NACKӦ��
u8 Ublox_Cfg_Ack_Check(void)
{			 
	u16 len=0,i;
	u8 rval=0;
	while((USART1_RX_STA&0X8000)==0 && len<100)//�ȴ����յ�Ӧ��   
	{
		len++;
		delay_ms(5);
	}		 
	if(len<250)   	//��ʱ����.
	{
		len=USART1_RX_STA&0X7FFF;	//�˴ν��յ������ݳ��� 
		for(i=0;i<len;i++)if(USART1_RX_BUF[i]==0XB5)break;//����ͬ���ַ� 0XB5
		if(i==len)rval=2;						//û���ҵ�ͬ���ַ�
		else if(USART1_RX_BUF[i+3]==0X00)rval=3;//���յ�NACKӦ��
		else rval=0;	   						//���յ�ACKӦ��
	}else rval=1;								//���ճ�ʱ����
    USART1_RX_STA=0;							//�������
	return rval;  
}
//���ñ���
//����ǰ���ñ������ⲿEEPROM����
//����ֵ:0,ִ�гɹ�;1,ִ��ʧ��.
u8 Ublox_Cfg_Cfg_Save(void)
{
	u8 i;
	_ublox_cfg_cfg *cfg_cfg=(_ublox_cfg_cfg *)USART1_TX_BUF;
	cfg_cfg->header=0X62B5;		//cfg header
	cfg_cfg->id=0X0906;			//cfg cfg id
	cfg_cfg->dlength=13;		//����������Ϊ13���ֽ�.		 
	cfg_cfg->clearmask=0;		//�������Ϊ0
	cfg_cfg->savemask=0XFFFF; 	//��������Ϊ0XFFFF
	cfg_cfg->loadmask=0; 		//��������Ϊ0 
	cfg_cfg->devicemask=4; 		//������EEPROM����		 
	Ublox_CheckSum((u8*)(&cfg_cfg->id),sizeof(_ublox_cfg_cfg)-4,&cfg_cfg->cka,&cfg_cfg->ckb);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_cfg));	//ͨ��dma���ͳ�ȥ
	for(i=0;i<6;i++)if(Ublox_Cfg_Ack_Check()==0)break;		//EEPROMд����Ҫ�ȽϾ�ʱ��,���������ж϶��
	return i==6?1:0;
}
//����NMEA�����Ϣ��ʽ
//msgid:Ҫ������NMEA��Ϣ��Ŀ,���������Ĳ�����
//      00,GPGGA;01,GPGLL;02,GPGSA;
//		03,GPGSV;04,GPRMC;05,GPVTG;
//		06,GPGRS;07,GPGST;08,GPZDA;
//		09,GPGBS;0A,GPDTM;0D,GPGNS;
//uart1set:0,����ر�;1,�������.	  
//����ֵ:0,ִ�гɹ�;����,ִ��ʧ��.
u8 Ublox_Cfg_Msg(u8 msgid,u8 uart1set)
{
	_ublox_cfg_msg *cfg_msg=(_ublox_cfg_msg *)USART1_TX_BUF;
	cfg_msg->header=0X62B5;		//cfg header
	cfg_msg->id=0X0106;			//cfg msg id
	cfg_msg->dlength=8;			//����������Ϊ8���ֽ�.	
	cfg_msg->msgclass=0XF0;  	//NMEA��Ϣ
	cfg_msg->msgid=msgid; 		//Ҫ������NMEA��Ϣ��Ŀ
	cfg_msg->iicset=1; 			//Ĭ�Ͽ���
	cfg_msg->uart1set=uart1set; //��������
	cfg_msg->uart2set=1; 	 	//Ĭ�Ͽ���
	cfg_msg->usbset=1; 			//Ĭ�Ͽ���
	cfg_msg->spiset=1; 			//Ĭ�Ͽ���
	cfg_msg->ncset=1; 			//Ĭ�Ͽ���	  
	Ublox_CheckSum((u8*)(&cfg_msg->id),sizeof(_ublox_cfg_msg)-4,&cfg_msg->cka,&cfg_msg->ckb);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_msg));	//ͨ��dma���ͳ�ȥ
	return Ublox_Cfg_Ack_Check();
}
//����NMEA�����Ϣ��ʽ
//baudrate:������,4800/9600/19200/38400/57600/115200/230400	  
//����ֵ:0,ִ�гɹ�;����,ִ��ʧ��(���ﲻ�᷵��0��)
u8 Ublox_Cfg_Prt(u32 baudrate,u16 outprotomask)
{
	_ublox_cfg_prt *cfg_prt=(_ublox_cfg_prt *)USART1_TX_BUF;
	cfg_prt->header=0X62B5;		//cfg header
	cfg_prt->id=0X0006;			//cfg prt id
	cfg_prt->dlength=20;		//����������Ϊ20���ֽ�.	
	cfg_prt->portid=1;			//��������1
	cfg_prt->reserved1=0;	 	//�����ֽ�,����Ϊ0
	cfg_prt->txready=0;	 		//TX Ready����Ϊ0
	cfg_prt->mode=0X08D0; 		//8λ,1��ֹͣλ,��У��λ
	cfg_prt->baudrate=baudrate; //����������
	cfg_prt->inprotomask=0X0007;//0+1+2
	cfg_prt->outprotomask=outprotomask;//0+1+2
 	cfg_prt->flags=0; 		//�����ֽ�,����Ϊ0
 	cfg_prt->reserved2=0; 		//�����ֽ�,����Ϊ0 
	Ublox_CheckSum((u8*)(&cfg_prt->id),sizeof(_ublox_cfg_prt)-4,&cfg_prt->cka,&cfg_prt->ckb);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_prt));	//ͨ��dma���ͳ�ȥ
	delay_ms(200);				//�ȴ�������� 
	USART1_Init(baudrate);	//���³�ʼ������1   
	return Ublox_Cfg_Ack_Check();//���ﲻ�ᷴ��0,��ΪUBLOX��������Ӧ���ڴ������³�ʼ����ʱ���Ѿ���������.
} 

//����UBLOX NEO-M8N�ĸ�������	    
//measrate:����ʱ��������λΪms�����ٲ���С��200ms��5Hz��
//reftime:�ο�ʱ�䣬0=UTC Time��1=GPS Time��һ������Ϊ1��
//����ֵ:0,���ͳɹ�;����,����ʧ��.
u8 Ublox_Cfg_Rate(u16 measrate,u8 reftime)
{
	_ublox_cfg_rate *cfg_rate=(_ublox_cfg_rate *)USART1_TX_BUF;
 	if(measrate<200)return 1;	//С��200ms��ֱ���˳�
 	cfg_rate->header=0X62B5;	//cfg header
	cfg_rate->id=0X0806;	 	//cfg rate id
	cfg_rate->dlength=6;	 	//����������Ϊ6���ֽ�.
	cfg_rate->measrate=measrate;//������,us
	cfg_rate->navrate=1;		//�������ʣ����ڣ����̶�Ϊ1
	cfg_rate->timeref=reftime; 	//�ο�ʱ��ΪGPSʱ��
	Ublox_CheckSum((u8*)(&cfg_rate->id),sizeof(_ublox_cfg_rate)-4,&cfg_rate->cka,&cfg_rate->ckb);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��14�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_rate));//ͨ��dma���ͳ�ȥ
    u2_printf("���ͳɹ���");
	return Ublox_Cfg_Ack_Check();
}

//����UBLOX NEO-M8N��λ����
//GNSSID������ϵͳID	 00  GPS;01  SBAS;2  Galileo;
//					03  BeiDou;04 IMES;05  QZSS;6  GLONASS;
//enable: 0x01��ʹ���������ϵͳ��0x00��ر��������ϵͳ

u8 Ublox_Cfg_GNSS(u8 GNSSID,u8 enable)
{
	_ublox_cfg_gnss *cfg_gnss=(_ublox_cfg_gnss *)USART1_TX_BUF;
 	cfg_gnss->header=0X62B5;	//cfg header
	cfg_gnss->id=0X3E06;	 	//cfg rate id
	cfg_gnss->dlength=12;	 	//����������Ϊ12���ֽ�.
	cfg_gnss->msgver=0;		//��Ϣ�汾��ȡ0��Ϊ��ǰ�汾
	cfg_gnss->numTrkChHw=0;				//ֻ����Ϣ�������ŵ���
	cfg_gnss->numTrkChUse=32;				//ʹ���ŵ���Ŀ��С�ڿ����ŵ�������Ϊ0���Զ�����
	cfg_gnss->numConfigBlocks=1;		//��Ҫ���õ�����ϵͳ�������˴�ȡ1����ÿ������1������ϵͳ
	cfg_gnss->gnssId=GNSSID;	//����ϵͳID		00,GPS;01,SBAS;2,Galileo;03,BeiDou;04,IMES;05,QZSS;6,GLONASS;
	cfg_gnss->resTrkCh=8;			//��С׷���ŵ���Ŀ
	cfg_gnss->maxTrkCh=16;		//���׷���ŵ���Ŀ
	cfg_gnss->reserved1=0;		//�����ֽ�,����Ϊ0
	cfg_gnss->flags=enable|(1<<24);		//��־�źţ�����0x01�����Ӧ��ϵͳ������0x00��ر���Ӧϵͳ
	Ublox_CheckSum((u8*)(&cfg_gnss->id),sizeof(_ublox_cfg_gnss)-4,&cfg_gnss->cka,&cfg_gnss->ckb);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��14�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_gnss));//ͨ��dma���ͳ�ȥ
	delay_ms(10);
	return Ublox_Cfg_GNSS1();
}

u8 Ublox_Cfg_GNSS1(void)
{
	_ublox_cfg_gnss1 *cfg_gnss1=(_ublox_cfg_gnss1 *)USART1_TX_BUF;
	cfg_gnss1->header1=0X62B5;	//cfg header
	cfg_gnss1->id1=0X3E06;	 	//cfg rate id
	cfg_gnss1->dlength1=0;	 	//����������Ϊ0���ֽ�.
	Ublox_CheckSum((u8*)(&cfg_gnss1->id1),sizeof(_ublox_cfg_gnss1)-4,&cfg_gnss1->cka1,&cfg_gnss1->ckb1);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��14�������   
	UART_DMA_Enable(DMA1_Channel4,sizeof(_ublox_cfg_gnss1));//ͨ��dma���ͳ�ȥ
	return Ublox_Cfg_Ack_Check();
}

//��ʼ��UBLOX NEO-M8N
//������ʹ��UBLOX NEO-M8Nģ��Ĭ�ϲ�����9600
//���ս�������Ϣ���浽�ⲿEEPROM����
void GPS_Init(void)
{
	u8 flag=0;
	Ublox_Cfg_Prt(9600,0x0003);	//������GPSģ�����Э�����ubloxЭ��
	delay_ms(100);
	while(Ublox_Cfg_Rate(10000,1)!=0)
	{
		flag++;
		delay_ms(10);
		if(flag>=10)
		{
			u2_printf("GPS FAIL\r\n");
			break;
		}
	}
	if(flag<10) u2_printf("GPS SUCCEED\r\n");

	flag=0;
	Ublox_Cfg_GNSS(0,0);
	delay_ms(10);
	Ublox_Cfg_GNSS(3,0);
	delay_ms(10);
	while(Ublox_Cfg_GNSS(00,0x011)!=0)
	{
		flag++;
		delay_ms(10);
		if(flag>=10)
		{
             u2_printf("GPS FAIL2\r\n");
			break;
		}
	}
	Ublox_Cfg_GNSS(3,1);
	Ublox_Cfg_Prt(9600,0x0002);	//�������GPSģ�����Э�鲻��ubloxЭ��
	Ublox_Cfg_Cfg_Save();	//������������
}
