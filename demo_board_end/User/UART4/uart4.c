#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   	   

//nmea_msg gpsx; 			//GPS信息
//u8 gnss_num, gnss_mode;
//u16 gnss_rate = 0;


//串口发送缓存区 	
__align(8) u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//发送缓冲,最大UART4_MAX_SEND_LEN字节
#ifdef UART4_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大UART4_MAX_RECV_LEN个字节.



//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 UART4_RX_STA=0;   	 
void UART4_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
 
	res =USART_ReceiveData(UART4);		
		if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//还可以接收数据
		{
			TIM_SetCounter(TIM5,0);//计数器清空        				 
			if(UART4_RX_STA==0)TIM5_Set(1);	 	//使能定时器1的中断 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			UART4_RX_STA|=1<<15;					//强制标记接收完成
			UART4_RX_BUF[(UART4_RX_STA&0X7FFF)+1]=0;//添加结束符
		} 
	}  											 
}   
//初始化IO 串口4
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void UART4_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

 	USART_DeInit(UART4);  //复位串口4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10
   
    //UART4_RX	  PC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11
	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART4, &USART_InitStructure); //初始化串口	4
  
	//波特率设置
 //	UART4->BRR=(pclk1*1000000)/(bound);// 波特率设置	 
	//UART4->CR1|=0X200C;  	//1位停止,无校验位.
	USART_Cmd(UART4, ENABLE);                    //使能串口 
	
#ifdef UART4_RX_EN		  	//如果使能了接收
	//使能接收中断
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM5_Init(1999,7200);		//10ms中断
	UART4_RX_STA=0;		//清零
	TIM5_Set(0);			//关闭定时器3
#endif	 	

}

//定时器5中断服务程序		    
void TIM5_IRQHandler(void)
{ 	
	int debug = 1;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		UART4_RX_STA|=1<<15;	//标记接收完成
		UART4_RX_BUF[(UART4_RX_STA&0X7FFF)]=0;//添加结束符
		//debug = gprs((double)gpsx.latitude/100000,(double)gpsx.longitude/100000);
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志    
		TIM5_Set(0);			//关闭TIM3  
	}	    
}
//设置TIM5的开关
//sta:0，关闭;1,开启;
void TIM5_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM5,0);//计数器清空
		TIM_Cmd(TIM5, ENABLE);  //使能TIMx	
	}else TIM_Cmd(TIM5, DISABLE);//关闭定时器5	   
}
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM5_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能//TIM5时钟使能    
	
	//定时器TIM5初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM5中断,允许更新中断

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
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
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);//等待串口发送完成，避免连续发送的时候漏发
    while(*Data!=0)//判断是字符串是否结束
    {
    USART_ClearFlag(USARTx,USART_FLAG_TC);
        if(*Data==0x5c)//判断是否是"\"
        {
            switch(*++Data)
            {
                case 'r':             //回车符其ascll为13也就是0x0d
                        USART_SendData(USARTx,0x0d );
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                case 'n':         //换行符其ascll为10也就是0x0a
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
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
	UART4->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
