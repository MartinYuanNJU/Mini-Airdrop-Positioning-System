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
//#include "bsp_lcd.h"
#include "temp.h"
#include "sim900a.h"
#include "gps.h"
//#include "TIM2.h"



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
u8 POS1[100];
char POSS[100];											 
char TEMP[100];
u8 TEMP_BUF1[USART1_MAX_RECV_LEN];		//����1�ݴ�����
u8 TEMP_BUF2[USART2_MAX_RECV_LEN];		//����2�ݴ�����
u8 TEMP_BUF3[USART3_MAX_RECV_LEN];		//����3�ݴ�����
u8 TEMP_BUF4[UART4_MAX_RECV_LEN];			//����4�ݴ�����



int main(void)
{
    u8 uc, ucDs18b20Id [ 8 ];
    float temp;
		int debug = 1;
    int y = 0;
		int j = 0;
    int i, index = 0;
		//����һϵ�еĳ�ʼ�� 
		//LCD_Init ();         //LCD ��ʼ��
		Init_All();
		TIM5_Init(1999,7200);		//10ms�ж�
		
		
		//((1+arr )/72M)*(1+psc )=((1+1999)/72M)*(1+35999)=1s
		//TIM2_Config(1999,35999);
		//NVIC_Config();
	
		//����ʾǰ��׼��
		
		//ILI9341_Clear ( 0, 0, 240, 320, macBACKGROUND);
		//ILI9341_DispString_EN (50, 10, "Welcome to HIT", macBACKGROUND, macCYAN );
		//while( DS18B20_Init() )	
						
		//������ǰ�¶Ȳ�������ʾ	
		//DS18B20_ReadId ( ucDs18b20Id  );
		while(!y)
    {
       y = sim900a_enmessage_mode_out();
    }
		while(1)
    {
				
				y=0;

				//temp = DS18B20_GetTemp_SkipRom ();
				//sprintf(TEMP, "Current temperature is:%.2f", temp);
				//ILI9341_DispString_EN ( 0, 40, TEMP, macBACKGROUND, macCYAN );
				usart1_analysis();
			for (j =0;j<100;j++)
			{
				POSS[j] = (char)POS[j];
			}
				//ILI9341_DispString_EN ( 0, 80, POSS, macBACKGROUND, macGREEN  );
				//TIM3_Init(9999,7199);
					
				//TIM3_Set(0);
				
			debug = 1;
			//�ȴ����ܿ��ƶ���
        //while(!y)
        //{
        //    y = sim900a_enmessage_mode_out();
        //}
				y = SIM_HANDLE_MESSAGE_CMD();
        if(y)
        {
          //ILI9341_DispString_EN ( 0, 120, "ok", macBACKGROUND, macGREEN  );  
					if(y == 1)
            {
								LED1 = 0;
                LED2 = 1;
                u2_printf("1");
								//y = 0;
            }
            else if(y == 2)
            {
								LED1 = 1;
                LED2 = 0;
                u2_printf("2");
								//y = 0;
            }
            else if(y == 3)
            {
								RELAY1 = 1;
                u2_printf("3");
								//y = 0;
            }
						            else if(y == 4)
            {
								RELAY2 = 1;
                u2_printf("4");
								//y = 0;
            }
						            else if(y == 5)
            {
								RELAY3 = 1;
                u2_printf("5");
								//y = 0;
            }
						            else if(y == 6)
            {
								RELAY4 = 1;
                u2_printf("6");
								//y = 0;
            }
            else if(y == 7)
            {
                		//��������Ϣ���͵��ֻ���
							LED1 = 1;LED2 = 1;
							index=0;	
							while(!index)
								{
										index=sim900a_sms_send("18846832453",POS);//13091895372
								}
								//ILI9341_DispString_EN ( 0, 190, "OK", macBACKGROUND, macGREEN  );
            }
						else if(y == 8)
            {
              LED1 = 0;LED2 = 0;
							index=0;  
							while(!(debug == 0))//!index)
								{
										debug = gprs();
										//debug = gprs((double)gpsx.latitude/100000,(double)gpsx.longitude/100000,(double)gpsx.altitude/10,(double)gpsx.speed/1000);
										//index=sim900a_sms_send("18846832453",TEMP);
								}
								debug = 1;
								//ILI9341_DispString_EN ( 0, 190, "OK", macBACKGROUND, macGREEN  );
            }
						//y = sim900a_enmessage_mode_out();
						//u2_printf("%d",y);

        }else
				{
						//ILI9341_DispString_EN ( 100, 120, "fail_", macBACKGROUND, macGREEN  );
						
						//printf("%d\n",debug);
				}
				//delay_ms(1000);
    }

}
