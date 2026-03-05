#include "Key.h"
uint8_t Key_state,Key_Lstate;

uint8_t Key_scan(void)
{
	static uint32_t utick;
	uint8_t ctick = HAL_GetTick();
	
	
	if(ctick-utick<=DEBOUNCE_TIME)return Key_shaking;
	
	
	
	Key_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	if(Key_state==0&&Key_Lstate==1)
	{
			utick  = ctick;
			return Key_pressed;
			
	}
	Key_Lstate = Key_state;
	return 0;
}
