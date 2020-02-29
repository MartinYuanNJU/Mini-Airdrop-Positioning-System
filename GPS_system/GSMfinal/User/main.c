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
#include "sim900a.h"



//全局变量
u8 phone[17];
u32 id;
u16 flash = 0;

char TEMP[100];
u8 TEMP_BUF1[USART1_MAX_RECV_LEN];		//串口1暂存数组
u8 TEMP_BUF2[USART2_MAX_RECV_LEN];		//串口2暂存数组
u8 TEMP_BUF3[USART3_MAX_RECV_LEN];		//串口3暂存数组
u8 TEMP_BUF4[UART4_MAX_RECV_LEN];			//串口4暂存数组



int main(void)
{
    u8 uc, ucDs18b20Id [ 8 ];
    float temp;
    int y = 0;
		int j = 0;
    int i, index = 0;
		//进行一系列的初始化 
		LCD_Init ();         //LCD 初始化
		Init_All();
	
		LED0 = !LED0;
		delay_ms(1000);
		LED0 = !LED0;
		//作显示前的准备
		
		ILI9341_Clear ( 0, 0, 240, 320, macBACKGROUND);
		ILI9341_DispString_EN (50, 10, "Welcome to HIT", macBACKGROUND, macCYAN );
		while( DS18B20_Init() )	
						
		//测量当前温度并进行显示	
		DS18B20_ReadId ( ucDs18b20Id  );
		while(!y)
    {
       y = sim900a_enmessage_mode_out();
    }
		index=sim900a_sms_send("18846832453","Summer school_GSM module test");
		while(1)
    {
				y=0;
				temp = DS18B20_GetTemp_SkipRom ();
				sprintf(TEMP, "Current temperature is:%.2f", temp);
				ILI9341_DispString_EN ( 0, 40, TEMP, macBACKGROUND, macCYAN );
				ILI9341_DispString_EN ( 0, 80, "Summer school_GSM module test", macBACKGROUND, macCYAN );
    }

}
