/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Hardware.h"
//#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for usartTask */
osThreadId_t usartTaskHandle;
const osThreadAttr_t usartTask_attributes = {
  .name = "usartTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for DHT11Task */
osThreadId_t DHT11TaskHandle;
const osThreadAttr_t DHT11Task_attributes = {
  .name = "DHT11Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for W25Q64task */
osThreadId_t W25Q64taskHandle;
const osThreadAttr_t W25Q64task_attributes = {
  .name = "W25Q64task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for lcd */
osThreadId_t lcdHandle;
const osThreadAttr_t lcd_attributes = {
  .name = "lcd",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myBinarySem01 */
osSemaphoreId_t myBinarySem01Handle;
const osSemaphoreAttr_t myBinarySem01_attributes = {
  .name = "myBinarySem01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
SemaphoreHandle_t printf_handle;//创建printf互斥锁句柄
SemaphoreHandle_t W25Q64_handle;//创建W25Q64互斥锁句柄

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void UsartTaskf(void *argument);
void DHT11Taskf(void *argument);
void w25q64taskf(void *argument);
void LCDTask05(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of myBinarySem01 */
  myBinarySem01Handle = osSemaphoreNew(1, 1, &myBinarySem01_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	printf_handle = xSemaphoreCreateMutex();//创建互斥锁
	W25Q64_handle= xSemaphoreCreateMutex();//创建W25Q64互斥锁
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of usartTask */
  usartTaskHandle = osThreadNew(UsartTaskf, NULL, &usartTask_attributes);

  /* creation of DHT11Task */
  DHT11TaskHandle = osThreadNew(DHT11Taskf, NULL, &DHT11Task_attributes);

  /* creation of W25Q64task */
  W25Q64taskHandle = osThreadNew(w25q64taskf, NULL, &W25Q64task_attributes);

  /* creation of lcd */
  lcdHandle = osThreadNew(LCDTask05, NULL, &lcd_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		if(Key_scan()==Key_pressed)
		{
			printf("Key_pressed");
			LED_toggle();
		}
		vTaskDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_UsartTaskf */
/**
* @brief Function implementing the usartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UsartTaskf */
void UsartTaskf(void *argument)
{
  /* USER CODE BEGIN UsartTaskf */
		const char t_data[]={"InitTask\n\r"};
    char tbuf[128];
    char rbuf[128];

    memset(tbuf,0,sizeof(tbuf));
    memset(rbuf,0,sizeof(rbuf));
		
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)t_data, strlen(t_data));
  /* Infinite loop */
  for(;;)
  {
		//xSemaphoreTake(printf_handle,portMAX_DELAY);
		if(HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t*)rbuf, sizeof(rbuf)-1)==HAL_OK)	
		{
			memcpy(tbuf,rbuf,sizeof(rbuf));
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tbuf, strlen(tbuf));
			if (strchr(rbuf, '1') != NULL)
            {
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // 反转 LED
                printf("LED toggled by Bluetooth\r\n");
            }
		}
		//xSemaphoreGive(printf_handle);
    osDelay(1);
  }
  /* USER CODE END UsartTaskf */
}

/* USER CODE BEGIN Header_DHT11Taskf */
/**
* @brief Function implementing the DHT11Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DHT11Taskf */
void DHT11Taskf(void *argument)
{
  /* USER CODE BEGIN DHT11Taskf */
//	DWT_Init();
//	uint8_t temp =0;
// 	uint8_t humi=0;
//	uint8_t DHT11_Buf[5] = {0};
//	
  /* Infinite loop */
  for(;;)
  {
		//任务优先级高       在进行通信的时候不能频繁打断     
//		vTaskSuspendAll();
//		DHT11_output();
//		HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_RESET);
//		DWT_DelayMS(20);
//		HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
//		delay_us(30);
//		if(DHT11_check() == 0)
//		{
//				for(uint8_t i = 0; i < 5; i++) 			//读取40位数据
//					{
//							DHT11_Buf[i] = DHT11_Read_byte();	//读出数据
//					}
//			
//					if((DHT11_Buf[0] + DHT11_Buf[1] + DHT11_Buf[2] + DHT11_Buf[3]) == DHT11_Buf[4])	//数据校验
//					{
//							humi = DHT11_Buf[0];				//将湿度值放入指针humi
//							temp = DHT11_Buf[2];				//将温度值放入指针temp
//							printf("Temp=%d, Humi=%d\r\n", temp, humi);
//					}
//					else 
//					{
//						printf("Checksum error: %d %d %d %d %d\r\n",
//						 DHT11_Buf[0], DHT11_Buf[1], DHT11_Buf[2], DHT11_Buf[3], DHT11_Buf[4]);
//					}
//			xTaskResumeAll();
			osDelay(1);
	}
//			vTaskDelay(1000);//10s测量一次		

  /* USER CODE END DHT11Taskf */
}

/* USER CODE BEGIN Header_w25q64taskf */
/**
* @brief Function implementing the W25Q64task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_w25q64taskf */
void w25q64taskf(void *argument)
{
  /* USER CODE BEGIN w25q64taskf */
	//uint32_t ID;
//		uint32_t id;
//    uint8_t tx[] = "Hello W25Q128 from SPI2!";
//    uint8_t rx[64] = {0};

//    osDelay(10);   // 上电稳定

//    id = W25Q_ReadJEDEC_ID();
//    printf("W25Q JEDEC ID = 0x%06X\r\n", id);

//    if (id != 0xEF4018)
//    {
//        printf("W25Q128 NOT FOUND!\r\n");
//        vTaskSuspend(NULL);
//    }

//    printf("Erasing sector 0...\r\n");
//    W25Q_SectorErase(0);

//    printf("Writing data...\r\n");
//    W25Q_WriteBuffer(0, tx, sizeof(tx));

//    printf("Reading back...\r\n");
//    W25Q_ReadBuffer(0, rx, sizeof(tx));

//    printf("Read data: %s\r\n", rx);

//    if (memcmp(tx, rx, sizeof(tx)) == 0)
//        printf("W25Q128 TEST PASS!\r\n");
//    else
//        printf("W25Q128 TEST FAIL!\r\n");

//    W25Q_PowerDown();

    for(;;)
		{
        osDelay(1000);
		}
  /* USER CODE END w25q64taskf */
}

/* USER CODE BEGIN Header_LCDTask05 */
/**
* @brief Function implementing the lcd thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCDTask05 */
void LCDTask05(void *argument)
{
  /* USER CODE BEGIN LCDTask05 */
  /* Infinite loop */
    // 初始化 LCD
		vTaskSuspendAll();
    CST816_GPIO_Init();
    CST816_RESET();
    LCD_Init();
    LCD_Open_Light();
		xTaskResumeAll();
    // 填充白色背景
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
		LCD_Set_Light(40);
    // 显示字符串
    LCD_ShowString(34, LCD_H/2+48, (uint8_t*)"FreeRTOS LCD", WHITE, BLACK, 24, 0);
    // 无限循环刷新
    for(;;)
    {
        LCD_ShowString(10, 20, (uint8_t*)"Hello FreeRTOS", BLACK, WHITE, 16, 0);
        osDelay(1000); // 每秒刷新一次
    }
  /* USER CODE END LCDTask05 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

