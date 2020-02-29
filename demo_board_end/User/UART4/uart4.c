#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   	   

//nmea_msg gpsx; 			//GPS��Ϣ
//u8 gnss_num, gnss_mode;
//u16 gnss_rate = 0;


//���ڷ��ͻ����� 	
__align(8) u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�
#ifdef UART4_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//���ջ���,���UART4_MAX_RECV_LEN���ֽ�.



//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 UART4_RX_STA=0;   	 
void UART4_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 
 
	res =USART_ReceiveData(UART4);		
		if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM_SetCounter(TIM5,0);//���������        				 
			if(UART4_RX_STA==0)TIM5_Set(1);	 	//ʹ�ܶ�ʱ��1���ж� 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			UART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
			UART4_RX_BUF[(UART4_RX_STA&0X7FFF)+1]=0;//��ӽ�����
		} 
	}  											 
}   
//��ʼ��IO ����4
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void UART4_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

 	USART_DeInit(UART4);  //��λ����4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC10
   
    //UART4_RX	  PC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC11
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(UART4, &USART_InitStructure); //��ʼ������	4
  
	//����������
 //	UART4->BRR=(pclk1*1000000)/(bound);// ����������	 
	//UART4->CR1|=0X200C;  	//1λֹͣ,��У��λ.
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 
	
#ifdef UART4_RX_EN		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM5_Init(1999,7200);		//10ms�ж�
	UART4_RX_STA=0;		//����
	TIM5_Set(0);			//�رն�ʱ��3
#endif	 	

}

//��ʱ��5�жϷ������		    
void TIM5_IRQHandler(void)
{ 	
	int debug = 1;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		UART4_RX_STA|=1<<15;	//��ǽ������
		UART4_RX_BUF[(UART4_RX_STA&0X7FFF)]=0;//��ӽ�����
		//debug = gprs((double)gpsx.latitude/100000,(double)gpsx.longitude/100000);
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־    
		TIM5_Set(0);			//�ر�TIM3  
	}	    
}
//����TIM5�Ŀ���
//sta:0���ر�;1,����;
void TIM5_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM5,0);//���������
		TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM5, DISABLE);//�رն�ʱ��5	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM5_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��//TIM5ʱ��ʹ��    
	
	//��ʱ��TIM5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM5�ж�,��������ж�

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
#endif		 


char * itoa(int value,char * string,int radix)
{
    int d,i;
    char flag=0;
    char *p=string;
    if(radix!=10)
    {
        *p=0;
        return(string);
    }
    if(!value)
    {
        *p++=0x30;
        *p=0;
        return(string);
     }
    if(value<0)
    {
        *p++='-';
        value*=-1;
    }
    for(i=10000;i>0;i/=10)
	{
		d=value/i;
		if(d||flag)
		{
			*p++=(char)(d+0x30);
			value-=(d*i);
			flag=1;
		}
	}
*p=0;
return (string);
}

void USART_printf(USART_TypeDef * USARTx,uint8_t *Data,...)
{
    const char *s;
    char buf[16];
    int d;
    //char c;
    va_list ap;
    va_start(ap,Data);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);//�ȴ����ڷ�����ɣ������������͵�ʱ��©��
    while(*Data!=0)//�ж����ַ����Ƿ����
    {
    USART_ClearFlag(USARTx,USART_FLAG_TC);
        if(*Data==0x5c)//�ж��Ƿ���"\"
        {
            switch(*++Data)
            {
                case 'r':             //�س�����ascllΪ13Ҳ����0x0d
                        USART_SendData(USARTx,0x0d );
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                case 'n':         //���з���ascllΪ10Ҳ����0x0a
                        USART_SendData(USARTx, 0X0a);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                default :
                        USART_SendData(USARTx, *Data);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;                              
            }
        }
        else
        {
            if(*Data==0x25)
            {
                switch (*++Data)
                {
                    case 's':
                        s=va_arg(ap,const char* );
                        for(;*s;s++)
                        {
                            USART_SendData(USARTx,*s );
                            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        }
                        Data++;
                        break;
                    case 'd':
                        d=va_arg(ap,int);
                        itoa(d,buf,10);
                        for(s=buf;*s;s++)
                        {
                            USART_SendData(USARTx,*s);
                            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        }
                        Data++;
                        break;
                    case 'c':
                        d=va_arg(ap,int);
                        USART_SendData(USARTx,d);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                   default:
                        Data++;
                        break;                                                  
                }
            }
            else
            {
                USART_SendData(USARTx, *Data++);
                while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
            }
        }
        //delay10ms();
    }
    va_end(ap);
    
}

void u4_printf(char *Data)
{
	USART_printf(UART4,(u8*)Data);
}

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//ѭ������,ֱ���������   
	UART4->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
