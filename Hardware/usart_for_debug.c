#include "usart_for_debug.h"
	/////////////////////////
//int fputc(int ch, FILE *f)
//{
//				// 将字符存储到缓冲区
//			xSemaphoreTake(printf_handle,portMAX_DELAY);

//			static uint8_t buffer[256];
//			static uint16_t index = 0;
//	 
//			buffer[index++] = (uint8_t)ch;
//	 
//			// 如果遇到换行符或缓冲区满，则启动DMA传输
//			if (ch == '\n' || index == sizeof(buffer)) {
//					HAL_UART_Transmit_DMA(&huart1, buffer, index);
//					index = 0; // 重置索引
//			}
//			xSemaphoreGive(printf_handle);	
//			return ch;
//		
//}


//void Usart_debug_init(void)
//{
//	const char t_data[]={"-----------\n"};
//	char t_buf[Transmit_buf_size];
//	char r_buf[Receive_buf_size];
//	memset(t_buf,0,sizeof(t_buf));
//	memset(r_buf,0,sizeof(r_buf));
//	
//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)t_data, strlen(t_data));
//}
	
	



