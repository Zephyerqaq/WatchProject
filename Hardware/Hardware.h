#ifndef Hardware_H
#define Hardware_H

#include		"stm32f4xx.h"

#include "FreeRTOS.h"
#include "sys.h"
/*	hardware.h*/
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "spi.h"
#include "iic_hal.h"

#define SYS_CLK 		100

/*   task.h*/

#include "semphr.h"




/*	device.h*/
#include		"LED.h"
#include		"key.h"
#include 	"DHT11.h"
#include "us_delay.h"
#include		"usart_for_debug.h"
#include "W25Q64.h"
#include "lcd.h"
#include "lcd_init.h"
#include "CST816.h"



extern SemaphoreHandle_t  printf_handle;//printf»¥³âËø
extern SemaphoreHandle_t	W25Q64_handle;//W25Q64»¥³âËø


#endif

