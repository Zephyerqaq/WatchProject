#include "DHT11.h"
#include "gpio.h"
/*
DHT11 单行 需要不断改变GPIO的模式 , 先拉低至少18ms 再拉高20-40us
发送完成后传感器返回80us低电平，然后发送80us高电平
设置定时器实现us延时 ms延时则用vtaskdelay实现
*/
void DHT11_output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

}
void DHT11_input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

uint8_t DHT11_check(void)
{
	uint8_t retry=0;
	DHT11_input();
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==1&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	if(retry>100)return 1;
	else retry=0;
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==0&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	if(retry>100)return 1;
	return 0;
}

uint8_t DHT11_Read_bit(void)
{
	uint8_t retry=0;
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==1&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==0&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==1)
		return 1;
	else return 0;
}


uint8_t DHT11_Read_byte(void)
{
	uint8_t i,data;
	data = 0;
	for(i = 0 ; i<8;i++)
	{
		data |=  DHT11_Read_bit()<<(7-i);
	}
	return data;
}



