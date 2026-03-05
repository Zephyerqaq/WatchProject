#ifndef US_DELAY_H
#define US_DELAY_H

#include "Hardware.h"
//void delay_us(uint32_t us);
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);
/*
使用TIM5进行usDelay     psc = 100-1 主频 = 100M   
*/
#endif


