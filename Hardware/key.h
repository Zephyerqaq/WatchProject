#ifndef Key_H
#define Key_H

#include "stm32f4xx.h"


#define Key_pressed 1
#define	Key_shaking 0
#define DEBOUNCE_TIME 10

uint8_t Key_scan(void);


#endif


