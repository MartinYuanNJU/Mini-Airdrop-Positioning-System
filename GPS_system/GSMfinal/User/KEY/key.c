#include "key.h"
#include "delay.h" 
 	    
//������ʼ������ 
//PC12 ���ó�����
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//PC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC12
 
} 
/*
 * ��������Key_Scan
 * ����  ������Ƿ��а���
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��KEY_OFF(û���°���)��KEY_ON�����°�����
 */
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 ) 
	{	   
		/*��ʱ����*/
		delay_ms(10);		
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 )  
		{	 
			/*�ȴ������ͷ� */
//			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
			return 	1;	 
		}
		else
			return 0;
	}
	else
		return 0;
}
