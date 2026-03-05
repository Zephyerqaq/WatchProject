
#ifndef __W25Q128_H
#define __W25Q128_H

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <stdint.h>

/* ==== 你已经在 CubeMX 配好的 SPI2 ==== */
extern SPI_HandleTypeDef hspi2;

/* ==== 片选引脚（你截图里的 PB12） ==== */
#define W25Q_CS_GPIO_Port   GPIOB
#define W25Q_CS_Pin         GPIO_PIN_12

#define W25Q_CS_LOW()   HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_RESET)
#define W25Q_CS_HIGH()  HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_SET)

/* ==== 超时 ==== */
#define W25Q_TIMEOUT   200   // ms

/* ==== W25Q128 指令 ==== */
#define W25Q_CMD_WRITE_ENABLE   0x06
#define W25Q_CMD_READ_STATUS1   0x05
#define W25Q_CMD_READ_DATA      0x03
#define W25Q_CMD_PAGE_PROGRAM   0x02
#define W25Q_CMD_SECTOR_ERASE   0x20   // 4KB
#define W25Q_CMD_CHIP_ERASE     0xC7
#define W25Q_CMD_JEDEC_ID       0x9F
#define W25Q_CMD_POWER_DOWN     0xB9

/* ==== 参数 ==== */
#define W25Q_PAGE_SIZE     256
#define W25Q_SECTOR_SIZE   4096

/* ==== API ==== */
uint32_t W25Q_ReadJEDEC_ID(void);
uint8_t  W25Q_ReadStatus1(void);
void     W25Q_WriteEnable(void);
void     W25Q_WaitBusy(void);
void     W25Q_SectorErase(uint32_t addr);
void     W25Q_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len);
void     W25Q_WriteBuffer(uint32_t addr, uint8_t *buf, uint32_t len);
void     W25Q_ReadBuffer(uint32_t addr, uint8_t *buf, uint32_t len);
void     W25Q_PowerDown(void);
void spi_loopback_test(void);

#endif



//#ifndef __SPI_FLASH_H
//#define __SPI_FLASH_H

//#include "Hardware.h"
//#include "stm32f4xx.h"
///* Private typedef -----------------------------------------------------------*/
////#define  sFLASH_ID                       0xEF3015     //W25X16
////#define  sFLASH_ID                       0xEF4015	    //W25Q16
////#define  sFLASH_ID                        0XEF4017     //W25Q64
//#define  sFLASH_ID                       0XEF4018    //W25Q128


////#define SPI_FLASH_PageSize            4096
//#define SPI_FLASH_PageSize              256
//#define SPI_FLASH_PerWritePageSize      256

///* Private define ------------------------------------------------------------*/
///*命令定义-开头*******************************/
//#define W25X_WriteEnable		      0x06 
//#define W25X_WriteDisable		      0x04 
//#define W25X_ReadStatusReg		    0x05 
//#define W25X_WriteStatusReg		    0x01 
//#define W25X_ReadData			        0x03 
//#define W25X_FastReadData		      0x0B 
//#define W25X_FastReadDual		      0x3B 
//#define W25X_PageProgram		      0x02 
//#define W25X_BlockErase			      0xD8 
//#define W25X_SectorErase		      0x20 
//#define W25X_ChipErase			      0xC7 
//#define W25X_PowerDown			      0xB9 
//#define W25X_ReleasePowerDown	    0xAB 
//#define W25X_DeviceID			        0xAB 
//#define W25X_ManufactDeviceID   	0x90 
//#define W25X_JedecDeviceID		    0x9F

//#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
//#define Dummy_Byte                0xFF
///*命令定义-结尾*******************************/


//#define FLASH_CS_PIN                     GPIO_PIN_12              
//#define FLASH_CS_GPIO_PORT               GPIOB                    


//#define SPI_FLASH_CS_LOW()      HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT,FLASH_CS_PIN,GPIO_PIN_RESET)
//#define SPI_FLASH_CS_HIGH()     HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT,FLASH_CS_PIN,GPIO_PIN_SET)
///*SPI接口定义-结尾****************************/

///*等待超时时间*/
//#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
//#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

///*信息输出*/
//#define FLASH_DEBUG_ON         1


//#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
//#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
//#define FLASH_DEBUG(fmt,arg...)          do{\
//                                          if(FLASH_DEBUG_ON)\
//                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
//                                          }while(0)

//#define 	TxBufferSize1   (countof(TxBuffer1) - 1)
//#define 	RxBufferSize1   (countof(TxBuffer1) - 1)
//#define 	countof(a)      (sizeof(a) / sizeof(*(a)))
//	
//#define  	BufferSize 		(countof(Tx_Buffer)-1)

//#define  FLASH_WriteAddress     0x00000
//#define  FLASH_ReadAddress      FLASH_WriteAddress
//#define  FLASH_SectorToErase    FLASH_WriteAddress

//#define MAX_TIMEOUT   200		//SPI轮询操作时的最大等待时间,ms

//typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

//extern __IO uint32_t DeviceID;
//extern __IO uint32_t FlashID;
//extern __IO TestStatus TransferStatus1;


////HAL_StatusTypeDef SPI_TransmitOneByte(uint8_t byteData);
////uint8_t SPI_ReceiveOneByte();					
//uint32_t SPI_FLASH_ReadID(void);
//uint32_t SPI_FLASH_ReadDeviceID(void);
////uint8_t SPI_FLASH_SendByte(uint8_t byte);
//void SPI_Flash_PowerDown(void);																					
//void StartTask_SPI(void const * argument);
//void SPI_FLASH_WaitForWriteEnd(void);
//void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
//void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
//void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
//void SPI_FLASH_SectorErase(uint32_t SectorAddr);
//void SPI_Flash_PowerDown(void);
//static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);
//TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
////TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

//#endif /* __SPI_FLASH_H */

//#ifndef W25Q64_H
//#define W25Q64_H

//#include "Hardware.h"

//#include "gpio.h"
//// ==== SPI 与引脚定义 ====
//// 在 main.c 或其他初始化文件中定义实际使用的 SPI 句柄和 CS 引脚
//#define W25Q64_SPI         hspi2
//#define W25Q64_CS_GPIO     GPIOB
//#define W25Q64_CS_PIN      GPIO_PIN_12
// 
//// ==== 指令码定义 ====
//#define W25Q64_CMD_WRITE_ENABLE     0x06
//#define W25Q64_CMD_WRITE_DISABLE    0x04
//#define W25Q64_CMD_READ_STATUS1     0x05
//#define W25Q64_CMD_READ_DATA        0x03
//#define W25Q64_CMD_PAGE_PROGRAM     0x02
//#define W25Q64_CMD_SECTOR_ERASE     0x20
//#define W25Q64_CMD_CHIP_ERASE       0xC7
//#define W25Q64_CMD_READ_ID          0x9F
// 
//// ==== 容量参数 ====
//#define W25Q64_SECTOR_SIZE          4096
//#define W25Q64_PAGE_SIZE            256
//#define W25Q64_TOTAL_SIZE           (8 * 1024 * 1024) // 8MB

//uint32_t W25Q64_ReadID(void);
//void     W25Q64_WriteEnable(void);
//uint8_t  W25Q64_ReadStatus1(void);
//void     W25Q64_WaitBusy(void);
//void     W25Q64_ReadData(uint32_t addr, uint8_t* buf, uint32_t len);
//void     W25Q64_PageProgram(uint32_t addr, const uint8_t* buf, uint32_t len);
//void     W25Q64_SectorErase(uint32_t addr);
//void     W25Q64_ChipErase(void);

//#endif


