#include "delay.h"
#include "usart2.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   	   

//???TIM2??????
//???????????????
//((1+arr )/72M)*(1+psc )=((1+1999)/72M)*(1+35999)=1?
void TIM2_Config(u16 arr,u16 psc)
{
    //??????1s

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    //???????
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//TIM_ClearFlag(TIM2,TIM_FLAG_Update);//??????

    //????????0-0xffff,72M/36000=2000
    TIM_TimeBaseStructure.TIM_Period = arr;
    //????????0-oxffff,???36000??
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    ////???????????
    //????,???0,??????
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //????,???,????123
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    //????
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    //????
    //TIM_Cmd(TIM2,ENABLE);
}

//??????????NVIC

void NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//?????????
void TIM2_IRQHandler(void)
{
    //??TIM3????????
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
    {
        //???????
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    //????
    GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)(!GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13)));
    }

}
