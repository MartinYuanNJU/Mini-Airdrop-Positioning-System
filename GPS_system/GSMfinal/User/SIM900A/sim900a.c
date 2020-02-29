#include <stdio.h>
#include <string.h>
#include "App.h"
#include "led.h"
#include "sim900a.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "bsp_lcd.h"

u8 GSM_SEND_BUF[USART3_MAX_SEND_LEN];	//���ŷ��ͻ�����
u8 GSM_BUF [50];		//���ݴ��������������
//char GSM_ANALYSIS_BUF[USART3_MAX_RECV_LEN];
u8 GSM_ANALYSIS_BUF[USART3_MAX_RECV_LEN];
u8 checknum[11][17];
u8 num_count = 0; //��ǰ�ѱ����������
extern u8 POS[100];
extern u16 flash;
extern u32 id;
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void sim_at_response(u8 mode)
{
    u16 i,rxlen;
		u8 *p = GSM_SEND_BUF;
    if(USART3_RX_STA & 0X8000)		//���յ�һ��������
    {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0; //���ӽ�����
        sprintf((char*)p, (const char*)"%s/n", USART3_RX_BUF);	//���͵�����
        u2_printf((char*)p);
        if(mode)USART3_RX_STA = 0;
			
		rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���

		sim900a_uart_analysis(USART3_RX_BUF);
		USART3_RX_STA=0x0000;
    }
		
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//SIM900A ������Թ��ô���

//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
    char *strx = 0;
    if(USART3_RX_STA & 0X8000)		//���յ�һ��������
    {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0; //���ӽ�����
        strx = strstr((const char*)USART3_RX_BUF, (const char*)str);
        u2_printf((char*)USART3_RX_BUF);
    }
    USART3_RX_STA = 0;
    return (u8*)strx;
}
//��sim900a��������
//cmd:���͵������ַ���(����Ҫ���ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim900a_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART3_RX_STA = 0;
    if((u32)cmd <= 0XFF)
    {
        while(DMA1_Channel2->CNDTR != 0);	//�ȴ�ͨ��2�������
        USART3->DR = (u32)cmd;
    } else u3_printf("%s\r", cmd); //��������
    if(ack && waittime)		//��Ҫ�ȴ�Ӧ��
    {
        while(--waittime)	//�ȴ�����ʱ
        {
            delay_ms(10);
            if(USART3_RX_STA & 0X8000) //���յ��ڴ���Ӧ����
            {
                if(sim900a_check_cmd(ack))break;//�õ���Ч����
                USART3_RX_STA = 0;
            }
        }
        if(waittime == 0)res = 1;
    }
    return res;
}



//���Ӳ���ʼ��sim900a
//ִ��˳��Ϊ�����ӣ����ò�����ģʽ������״̬��飬
//�ر����ԣ�

void sim900a_init(void)
{
    u8 flag = 0;
    while(sim900a_send_cmd("AT", "OK", 200)) //����Ƿ�Ӧ��ATָ��
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL1\r\n");
            break;
        }
    }
    flag = 0;
    while(sim900a_send_cmd("AT+CPIN?", "READY", 200)) //������
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL2\r\n");
            break;
        }
    }
    flag = 0;
    while(sim900a_send_cmd("AT+CREG?", "0,1", 200)) //������
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("SIM FAIL3\r\n");
            break;
        }
    }
    u2_printf("success\r\n");
}

//����*strָ����ַ���
//���ͳɹ�����1��ʧ�ܷ���0
u8 sim900a_enmessage_mode_out(void)	//��GSM���յ���Ӣ�Ļ����ֶ��Ų�ת��
{
    int flag = 0;
    while(sim900a_send_cmd("AT+CMGF=1", "OK", 200)) //������
    {
        flag++;
				delay_ms(100);
				if (flag >= 10)
				{
						u2_printf("out FAIL\r\n");
						return 0;
						break;
				}
    }
		flag = 0;
    while(sim900a_send_cmd("AT+CSCS=\"GSM\"", "OK", 200)) //������
    {
        flag++;
				delay_ms(100);
				if (flag >= 10)
				{
						u2_printf("out FAIL\r\n");
						return 0;
						break;
				}
    }
		flag = 0;
		while(sim900a_send_cmd("AT+CNMI=2,2", "OK", 200)) //������
    {
        flag++;
        delay_ms(100);
        if (flag >= 10)
        {
            u2_printf("out FAIL\r\n");
            return 0;
            break;
        }
		}
    return 1;
		delay_ms(100);
}


u8 SIM_HANDLE_MESSAGE_CMD(void)
{
		//u2_printf((char*)USART3_RX_BUF);
	    u16 i,rxlen;
			int y = 0;
			u8 *p = GSM_SEND_BUF;

			if(USART3_RX_STA & 0X8000)
			{
					USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;
					sprintf((char*)p, (const char*)"%s/n", USART3_RX_BUF);
					u2_printf((char*)p);
			
			rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���

			sim900a_uart_analysis(USART3_RX_BUF);
			USART3_RX_STA=0x0000;
				
					USART3_RX_STA = 0;
			}
    if(strstr((char*)USART3_RX_BUF, "LED1") != NULL)
    {
        return 1;
    }
    else if(strstr((char*)USART3_RX_BUF, "LED2") != NULL)
    {
        return 2;
    }
    else if(strstr((char*)USART3_RX_BUF, "pos") != NULL)
    {
        return 3;
    }
    return 0;
}
//������
//�����ճɹ��򽫶������ݴ浽GSM_ANALYSIS_BUF���沢�ҷ��ص�ַ���Ա��������
//������ʧ���򽫷��ؿ�
//���Ƕ��������ﵽ45���ڶ�ȡ���ųɹ�֮��ɾ�����ж���
//AT+CMGD=1,4
u8* sim900a_sms_read(u8 index)
{
	u16 i;
	u8 *p=GSM_BUF;
	sprintf((char*)p,"AT+CMGR=%d\0",index);
//	ESP_SEND((char*)p);
//	ESP_SEND("\r\n");
	if (sim900a_send_cmd(p,"+CMGR:",200)==0)
	{
		for (i=0;USART3_RX_BUF[i]!=0;i++)
		{
			GSM_ANALYSIS_BUF[i]=USART3_RX_BUF[i];
		}
		GSM_ANALYSIS_BUF[i]=0;
		//if(index>=45) sim900a_send_cmd("AT+CMGDA=\"DEL ALL\"","OK",300);
		if(index>=45) sim900a_send_cmd("AT+CMGD=1,1","OK",300);
		USART3_RX_STA=0;
		return GSM_ANALYSIS_BUF;
	}
	return NULL;
}
//����*strָ����ַ���
//���ͳɹ�����1��ʧ�ܷ���0
u8 sim900a_sms_send(u8* phone_number, u8 *str)
{
    /*u8 *p = GSM_BUF;
    sprintf((char*)p, "AT+CMGS=\"%s\"", phone_number);
    //u2_printf(p);

    if(sim900a_send_cmd("AT+CMGF=1", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSCS=\"GSM\"", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSCA?", "OK", 100))	return 0;
    if(sim900a_send_cmd("AT+CSMP=17,167,0,25", "OK", 100))	return 0;
    //sim900a_send_cmd(p,"",200);
//    if(sim900a_send_cmd(p, ">", 200) == 0)					//���Ͷ�������+�绰����
//    {
//        sim900a_send_cmd(str, "", 200);		 						//���Ͷ������ݵ�GSMģ��
////���ͽ�����,�ȴ��������(��ȴ�10����,��Ϊ���ų��˵Ļ�,�ȴ�ʱ��᳤һЩ)
//        if(sim900a_send_cmd((u8*)0X1A, "+CMGS:", 3000) == 0)
//        {
//            return 1;
//        }
//    }
    if(sim900a_send_cmd(p, ">", 200))	return 0;				//���Ͷ�������+�绰����
    if(sim900a_send_cmd(str, "", 200))	return 0;		 	//���Ͷ������ݵ�GSMģ��
//���ͽ�����,�ȴ��������(��ȴ�10����,��Ϊ���ų��˵Ļ�,�ȴ�ʱ��᳤һЩ)
    if(sim900a_send_cmd((u8*)0X1A, "+CMGS:", 3000)) return 0;
    return 0;*/
		
		u8 cmd[100];
		sprintf((char*)cmd,"AT+CMGS=\"%s\"",phone_number);
		if(sim900a_send_cmd((u8 *)"AT+CMGF=1",(u8 *)"OK",100))	return SIM_CMGF_ERR;
		if(sim900a_send_cmd((u8 *)"AT+CSCS=\"GSM\"",(u8 *)"OK",100))	return SIM_CSCS_ERR;
		if(sim900a_send_cmd((u8 *)"AT+CSCA?",(u8 *)"OK",100))	return SIM_CSCA_ERR;
		if(sim900a_send_cmd((u8 *)"AT+CSMP=17,167,0,241",(u8 *)"OK",100))	return SIM_CSMP_ERR;
		if(sim900a_send_cmd(cmd,(u8 *)">",100))	return SIM_CMGS_ERR;
		u2_printf("%s",str);
		if(sim900a_send_cmd((u8 *)0x1A,(u8 *)"+CMGS:",1500))	return SIM_CMGS_SEND_FAIL;
		return SIM_OK;
}

//usart3�յ�����Ϣ���д���
void sim900a_uart_analysis(u8*buf)
{
	u8 NewMseeage_Num=0;
	u8 dx;
	u8 *p;
	//�ж��Ƿ����յ����µĶ��Ż��ߵ绰����
	if(strstr((const char *)buf,"+CMTI"))//�ж��յ����Ƿ�Ϊ������Ϣ
	{		//�����ó���Ϣ��Ŀ��
		p=(u8*)strstr((const char *)buf,",");
		NewMseeage_Num=Str2num(p+1,&dx);	
	}
	if(strstr((const char *)buf,"RING"))//�ж��յ����Ƿ�Ϊ�绰��Ϣ
	{		//ִ�йҵ绰����
		sim900a_send_cmd("ATH","OK",200);
		USART3_RX_STA=0;
	}
	if(NewMseeage_Num)
	{
//		ESP_SEND("�յ����ţ�%d\r\n",NewMseeage_Num);
		//ILI9341_DispString_EN ( 0, 150, NewMseeage_Num, macBACKGROUND, macGREEN  );
		buf=sim900a_sms_read(NewMseeage_Num);		//�յ��������ȡ���Ų�����
		NewMseeage_Num=0;
	}
}
