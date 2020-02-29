#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"
#include "sys.h"

//#define LED0 PCout(1)	// PC7
//#define LED1 PCout(2)	// PC6

#define LED0 PBout(5)	// PB3
#define LED1 PBout(0)	// PB4
#define LED2 PBout(1)	// PB4

#define RELAY1 PDout(0)	// PD0
#define RELAY2 PDout(1)	// PD1
#define RELAY3 PDout(2)	// PD2
#define RELAY4 PDout(3)	// PD3

void LED_Init(void);
void RELAY_Init(void);
#endif 
