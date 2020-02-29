#include <string.h>
#include <stdio.h>
#include "App.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "stm32f10x.h"
#include "bsp_lcd.h"
#include "temp.h"
#include "gps.h"
#include "sim900a.h"


//ȫ�ֱ���
nmea_msg gpsx; 			//GPS��Ϣ
u8 gnss_num, gnss_mode;
u16 gnss_rate = 0;
u8 phone[17];
u32 id;
u16 flash = 0;
const u8*fixmode_tbl[4] = {"��λʧ�� ", "��λʧ�� ", "2Dģʽ ", "3Dģʽ "};	//fix mode�ַ���
const u8*fixmode_tbl_eng[4] = {"failed", "failed", "2D mode", "3D mode"};	//fix mode�ַ���
const u8*gnss_tbl[7] = {"GPSϵͳ ", "SBASϵͳ ", \
                        "٤����ϵͳ ", "����ϵͳ ", "IMESϵͳ ", "QZSSϵͳ ", "������˹ϵͳ "
                       };	//����ϵͳ�ַ���

//u8 POS[100]={"$POS,3D mode,2015/05/29,16:19:09,126.62672 E,45.74571 N,185.7m,0.041km/h\r\n"};
//char POS[100];
u8 POS[100];		
char POSS[100];											 
char TEMP[100];
u8 TEMP_BUF1[USART1_MAX_RECV_LEN];		//����1�ݴ�����
u8 TEMP_BUF2[USART2_MAX_RECV_LEN];		//����2�ݴ�����
u8 TEMP_BUF3[USART3_MAX_RECV_LEN];		//����3�ݴ�����
u8 TEMP_BUF4[UART4_MAX_RECV_LEN];			//����4�ݴ�����



int main(void)
{
	/*
    u8 uc, ucDs18b20Id [ 8 ];
    float temp;
    int y = 0;
		int j = 0;
    int i, index = 0;
		//����һϵ�еĳ�ʼ�� 
		LCD_Init ();         //LCD ��ʼ��
		Init_All();
	
		LED1 = !LED1;
		delay_ms(100);
		LED1 = !LED1;
		//����ʾǰ��׼��
		
		ILI9341_Clear ( 0, 0, 240, 320, macBACKGROUND);
		ILI9341_DispString_EN (50, 10, "Welcome to HIT", macBACKGROUND, macCYAN );
		while( DS18B20_Init() )	
						
		//������ǰ�¶Ȳ�������ʾ	
		DS18B20_ReadId ( ucDs18b20Id  );

		while(1)
    {
				
				y=0;

				temp = DS18B20_GetTemp_SkipRom ();
				sprintf(TEMP, "Current temperature is:%.2f", temp);
				ILI9341_DispString_EN ( 0, 40, TEMP, macBACKGROUND, macCYAN );
				//����GPS��λ��������ʾ
				usart1_analysis();
			for (j =0;j<100;j++)
			{
				POSS[j] = (char)POS[j];
			}
				ILI9341_DispString_EN ( 0, 80, POSS, macBACKGROUND, macGREEN  );
			
    }*/
		
		int result=1;
		u8 res=0;
	
    u8 uc, ucDs18b20Id [ 8 ];
    float temp;
    int y = 0;
		int j = 0;
    int i, index = 0;
		
		//����һϵ�еĳ�ʼ�� 
		LCD_Init ();         //LCD ��ʼ��
		Init_All();
	
		LED1 = !LED1;
		delay_ms(500);
		LED1 = !LED1;

		//����ʾǰ��׼��
		
		ILI9341_Clear ( 0, 0, 240, 320, macBACKGROUND);
		ILI9341_DispString_EN (50, 10, "Welcome to HIT", macBACKGROUND, macCYAN );
		while( DS18B20_Init() )	
						
		//������ǰ�¶Ȳ�������ʾ	
		DS18B20_ReadId ( ucDs18b20Id );	
		
		delay_init();	    	 //��ʱ������ʼ��	  
		NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� 	LED_Init();
		USART2_Init(115200);	//��ʼ������2 
		USART3_Init(9600);	//LOG��Ϣ
		
		delay_ms(500);
		LED1 = !LED1;
		delay_ms(500);
		LED1 = !LED1;
		
		/*while(result)
		{
			result=GSM_Dect();
			delay_ms(2000);
		}	*/
		
		while(1)
    {
				y=0;
				temp = DS18B20_GetTemp_SkipRom ();
				sprintf(TEMP, "Current temperature is:%.2f", temp);
				ILI9341_DispString_EN ( 0, 40, TEMP, macBACKGROUND, macCYAN );

				usart1_analysis();
			
				for (j =0;j<100;j++)
				{
					POSS[j] = (char)POS[j];
				}
				ILI9341_DispString_EN ( 0, 80, POSS, macBACKGROUND, macGREEN  );
			
				res=SIM_HANDLE_MESSAGE_CMD();
			
				if(res==1)
				{
					LED1 = !LED1;
					delay_ms(1000);
					LED1 = !LED1;
				}
				else if(res==2)
				{
					LED1 = !LED1;
					delay_ms(1000);
					LED1 = !LED1;
					delay_ms(1000);
					LED1 = !LED1;
					delay_ms(1000);
					LED1 = !LED1;
				}
				else if(res==3)
				{
					result=1;
					while(result)
					{
						result=sim900a_sms_send("13522838752",POS);
					}
				}
    }
}
