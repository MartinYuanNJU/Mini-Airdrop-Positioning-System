#ifndef __TEMP_H
#define	__TEMP_H



#include "stm32f10x.h"



/************************** DS18B20 连接引脚定义********************************/
#define      macDS18B20_DQ_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      macDS18B20_DQ_GPIO_CLK                       RCC_APB2Periph_GPIOD

#define      macDS18B20_DQ_GPIO_PORT                      GPIOD
#define      macDS18B20_DQ_GPIO_PIN                       GPIO_Pin_6



/************************** DS18B20 函数宏定义********************************/
#define      macDS18B20_DQ_0	                            GPIO_ResetBits ( macDS18B20_DQ_GPIO_PORT, macDS18B20_DQ_GPIO_PIN ) 
#define      macDS18B20_DQ_1	                            GPIO_SetBits ( macDS18B20_DQ_GPIO_PORT, macDS18B20_DQ_GPIO_PIN ) 

#define      macDS18B20_DQ_IN()	                          GPIO_ReadInputDataBit ( macDS18B20_DQ_GPIO_PORT, macDS18B20_DQ_GPIO_PIN ) 



/************************** DS18B20 函数声明 ********************************/
uint8_t                  DS18B20_Init                      ( void );
void                     DS18B20_ReadId                    ( uint8_t * ds18b20_id );
float                    DS18B20_GetTemp_SkipRom           ( void );
float                    DS18B20_GetTemp_MatchRom          ( uint8_t * ds18b20_id );








#endif /* __DS18B20_H */






