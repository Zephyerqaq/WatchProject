#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"

#define LED_On 		GPIO_PIN_SET
#define LED_Off 	GPIO_PIN_RESET

void LED_turn_on(void);
void LED_turn_off(void);
void LED_toggle(void);


#endif


