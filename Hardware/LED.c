#include "LED.h"
/*
	gpioc_13  Ϊled0 
*/

void LED_turn_on(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,LED_On);
	
}

void LED_turn_off(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,LED_Off);
	
}

void LED_toggle(void)
{
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
}


