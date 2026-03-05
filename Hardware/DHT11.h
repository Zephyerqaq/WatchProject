#ifndef DHT11_H
#define DHT11_H

#include "Hardware.h"

void DHT11_output(void);
void DHT11_input(void);
uint8_t DHT11_check(void);
uint8_t DHT11_Read_bit(void);
uint8_t DHT11_Read_byte(void);

#endif


