#include "W25Q64.h"
#include "stdio.h"
void spi_loopback_test(void)
{
    uint8_t tx = 0x55;
    uint8_t rx = 0;

    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, 100);

    printf("Loopback RX = 0x%02X\r\n", rx);
}
static uint8_t W25Q_TransferByte(uint8_t tx)
{
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, W25Q_TIMEOUT);
    return rx;
}

/* -------- 读取状态寄存器 -------- */
uint8_t W25Q_ReadStatus1(void)
{
    uint8_t status;

    W25Q_CS_LOW();
    W25Q_TransferByte(W25Q_CMD_READ_STATUS1);
    status = W25Q_TransferByte(0xFF);
    W25Q_CS_HIGH();

    return status;
}

/* -------- 等待忙标志 WIP=0 -------- */
void W25Q_WaitBusy(void)
{
    while (W25Q_ReadStatus1() & 0x01)
    {
        osDelay(1);
    }
}

/* -------- 写使能 -------- */
void W25Q_WriteEnable(void)
{
    W25Q_CS_LOW();
    W25Q_TransferByte(W25Q_CMD_WRITE_ENABLE);
    W25Q_CS_HIGH();
}

/* -------- 读取 JEDEC ID（W25Q128 → 0xEF4018） -------- */
uint32_t W25Q_ReadJEDEC_ID(void)
{
    uint8_t id[3];

    W25Q_CS_HIGH();
    osDelay(1);

    W25Q_CS_LOW();
    osDelay(1);

    uint8_t cmd = 0x9F;
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);

    uint8_t dummy = 0xFF;

    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[0], 1, 100);
    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[1], 1, 100);
    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[2], 1, 100);

    W25Q_CS_HIGH();

    return ((uint32_t)id[0] << 16) |
           ((uint32_t)id[1] << 8) |
            (uint32_t)id[2];
}



/* -------- 4KB 扇区擦除 -------- */
void W25Q_SectorErase(uint32_t addr)
{
    W25Q_WriteEnable();

    W25Q_CS_LOW();
    W25Q_TransferByte(W25Q_CMD_SECTOR_ERASE);
    W25Q_TransferByte((addr >> 16) & 0xFF);
    W25Q_TransferByte((addr >> 8) & 0xFF);
    W25Q_TransferByte(addr & 0xFF);
    W25Q_CS_HIGH();

    W25Q_WaitBusy();
}

/* -------- 页写（最大 256B，不能跨页） -------- */
void W25Q_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len)
{
    if (len > W25Q_PAGE_SIZE)
        len = W25Q_PAGE_SIZE;

    W25Q_WriteEnable();

    W25Q_CS_LOW();
    W25Q_TransferByte(W25Q_CMD_PAGE_PROGRAM);
    W25Q_TransferByte((addr >> 16) & 0xFF);
    W25Q_TransferByte((addr >> 8) & 0xFF);
    W25Q_TransferByte(addr & 0xFF);

    HAL_SPI_Transmit(&hspi2, buf, len, W25Q_TIMEOUT);
    W25Q_CS_HIGH();

    W25Q_WaitBusy();
}

/* -------- 连续写（自动跨页） -------- */
void W25Q_WriteBuffer(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t page_offset = addr % W25Q_PAGE_SIZE;
    uint32_t remain = W25Q_PAGE_SIZE - page_offset;

    if (len <= remain)
    {
        W25Q_PageProgram(addr, buf, len);
        return;
    }

    W25Q_PageProgram(addr, buf, remain);
    addr += remain;
    buf  += remain;
    len  -= remain;

    while (len >= W25Q_PAGE_SIZE)
    {
        W25Q_PageProgram(addr, buf, W25Q_PAGE_SIZE);
        addr += W25Q_PAGE_SIZE;
        buf  += W25Q_PAGE_SIZE;
        len  -= W25Q_PAGE_SIZE;
    }

    if (len)
        W25Q_PageProgram(addr, buf, len);
}

/* -------- 连续读 -------- */
void W25Q_ReadBuffer(uint32_t addr, uint8_t *buf, uint32_t len)
{
    W25Q_CS_LOW();

    W25Q_TransferByte(W25Q_CMD_READ_DATA);
    W25Q_TransferByte((addr >> 16) & 0xFF);
    W25Q_TransferByte((addr >> 8) & 0xFF);
    W25Q_TransferByte(addr & 0xFF);

    while (len--)
    {
        *buf++ = W25Q_TransferByte(0xFF);
    }

    W25Q_CS_HIGH();
}

/* -------- 掉电 -------- */
void W25Q_PowerDown(void)
{
    W25Q_CS_LOW();
    W25Q_TransferByte(W25Q_CMD_POWER_DOWN);
    W25Q_CS_HIGH();
}

//#include "W25Q64.h"
//#include "Freertos.h"
//#include "semphr.h"

///* 获取缓冲区的长度 */

///* 发送缓冲区初始化 */
////uint8_t Tx_Buffer[] = "this is a string a test data\r\nI am Xue Dingwei.\r\n";


////#define DEBUG
//__IO uint32_t DeviceID = 0;
//__IO uint32_t FlashID = 0;
//__IO TestStatus TransferStatus1 = FAILED;


//static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

//static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


///**
//  * @brief  使用SPI发送一个字节的数据
//  * @param  byte：要发送的数据
//  * @retval 返回接收到的数据
//  */

////uint8_t SPI_FLASH_SendByte(uint8_t byte)
////{

////  SPITimeout = SPIT_FLAG_TIMEOUT;

////  /* 等待发送缓冲区为空，TXE事件 */
////  while (__HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_TXE ) == RESET)
////   {
////    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
////   }

////  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
////  WRITE_REG(hspi1.Instance->DR, byte);

////  SPITimeout = SPIT_FLAG_TIMEOUT;

////  /* 等待接收缓冲区非空，RXNE事件 */
////  while (__HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_RXNE ) == RESET)
////   {
////    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
////   }

////  /* 读取数据寄存器，获取接收缓冲区数据 */
////  return READ_REG(hspi1.Instance->DR);	
////}

//HAL_StatusTypeDef SPI_FLASH_SendByte(uint8_t byteData) 
//{
//	return HAL_SPI_Transmit(&hspi2, &byteData, 1, MAX_TIMEOUT);
//}



////SPI接口接收一个字节， 返回接收的一个字节数据
//uint8_t SPI_FLASH_ReceiveByte(void)
//{
//    uint8_t tx = 0xFF;
//    uint8_t rx = 0;
//    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, MAX_TIMEOUT);
//    return rx;
//}



///**
//  * @brief  向FLASH发送 写使能 命令
//  * @param  none
//  * @retval none
//  */
//void SPI_FLASH_WriteEnable(void)
//{
//  /* 通讯开始：CS低 */
//  SPI_FLASH_CS_LOW();


//  /* 发送写使能命令*/
//  SPI_FLASH_SendByte(W25X_WriteEnable);
//	


//  /*通讯结束：CS高 */
//  SPI_FLASH_CS_HIGH();
//}

// /**
//  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
//  * @param  none
//  * @retval none
//  */
//void SPI_FLASH_WaitForWriteEnd(void)
//{
//  uint8_t FLASH_Status = 0;

//  /* 选择 FLASH: CS 低 */
//  SPI_FLASH_CS_LOW();

//  /* 发送 读状态寄存器 命令 */
//  SPI_FLASH_SendByte(W25X_ReadStatusReg);

//  SPITimeout = SPIT_FLAG_TIMEOUT;
//  /* 若FLASH忙碌，则等待 */
//  do
//  {
//    /* 读取FLASH芯片的状态寄存器 */
//		uint8_t dummy = 0xFF;
//		HAL_SPI_TransmitReceive(&hspi2, &dummy, &FLASH_Status, 1, MAX_TIMEOUT);
//    //FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 

//    {
//      if((SPITimeout--) == 0) 
//      {
//        SPI_TIMEOUT_UserCallback(4);
//        return;
//      }
//    } 
//  }
//  while ((FLASH_Status & WIP_Flag) == SET); /* 正在写入标志 */

//  /* 停止信号  FLASH: CS 高 */
//  SPI_FLASH_CS_HIGH();
//}


///**
//  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
//  * @param	pBuffer，要写入数据的指针
//  * @param WriteAddr，写入地址
//  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
//  * @retval 无
//  */
//void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
//{
//  /* 发送FLASH写使能命令 */
//  SPI_FLASH_WriteEnable();

//  /* 选择FLASH: CS低电平 */
//  SPI_FLASH_CS_LOW();
//  /* 写页写指令*/
//  SPI_FLASH_SendByte(W25X_PageProgram);
//  /*发送写地址的高位*/
//  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
//  /*发送写地址的中位*/
//  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
//  /*发送写地址的低位*/
//  SPI_FLASH_SendByte(WriteAddr & 0xFF);

//  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
//  {
//     NumByteToWrite = SPI_FLASH_PerWritePageSize;
//     FLASH_ERROR("SPI_FLASH_PageWrite too large!");
//  }

//  /* 写入数据*/
//  while (NumByteToWrite--)
//  {
//    /* 发送当前要写入的字节数据 */
//    SPI_FLASH_SendByte(*pBuffer);
//    /* 指向下一字节数据 */
//    pBuffer++;
//  }

//  /* 停止信号 FLASH: CS 高电平 */
//  SPI_FLASH_CS_HIGH();

//  /* 等待写入完毕*/
//  SPI_FLASH_WaitForWriteEnd();
//}

///**
//  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
//  * @param	pBuffer，要写入数据的指针
//  * @param  WriteAddr，写入地址
//  * @param  NumByteToWrite，写入数据长度
//  * @retval 无
//  */
//void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
//{
//  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
//	
//	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
//  Addr = WriteAddr % SPI_FLASH_PageSize;
//	
//	/*差count个数据值，刚好可以对齐到页地址*/
//  count = SPI_FLASH_PageSize - Addr;	
//	/*计算出要写多少整数页*/
//  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
//	/*mod运算求余，计算出剩余不满一页的字节数*/
//  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

//	 /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
//  if (Addr == 0) 
//  {
//		/* NumByteToWrite < SPI_FLASH_PageSize */
//    if (NumOfPage == 0) 
//    {
//      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
//    }
//    else /* NumByteToWrite > SPI_FLASH_PageSize */
//    {
//			/*先把整数页都写了*/
//      while (NumOfPage--)
//      {
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
//        WriteAddr +=  SPI_FLASH_PageSize;
//        pBuffer += SPI_FLASH_PageSize;
//      }
//			
//			/*若有多余的不满一页的数据，把它写完*/
//      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
//    }
//  }
//	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
//  else 
//  {
//		/* NumByteToWrite < SPI_FLASH_PageSize */
//    if (NumOfPage == 0) 
//    {
//			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
//      if (NumOfSingle > count) 
//      {
//        temp = NumOfSingle - count;
//				
//				/*先写满当前页*/
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
//        WriteAddr +=  count;
//        pBuffer += count;
//				
//				/*再写剩余的数据*/
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
//      }
//      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
//      {				
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
//      }
//    }
//    else /* NumByteToWrite > SPI_FLASH_PageSize */
//    {
//			/*地址不对齐多出的count分开处理，不加入这个运算*/
//      NumByteToWrite -= count;
//      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
//      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

//      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
//      WriteAddr +=  count;
//      pBuffer += count;
//			
//			/*把整数页都写了*/
//      while (NumOfPage--)
//      {
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
//        WriteAddr +=  SPI_FLASH_PageSize;
//        pBuffer += SPI_FLASH_PageSize;
//      }
//			/*若有多余的不满一页的数据，把它写完*/
//      if (NumOfSingle != 0)
//      {
//        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
//      }
//    }
//  }
//}

// /**
//  * @brief  读取FLASH数据
//  * @param 	pBuffer，存储读出数据的指针
//  * @param   ReadAddr，读取地址
//  * @param   NumByteToRead，读取数据长度
//  * @retval 无
//  */
//void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
//{
//  /* 选择FLASH: CS低电平 */
//  SPI_FLASH_CS_LOW();

//  /* 发送 读 指令 */
//  SPI_FLASH_SendByte(W25X_ReadData);

//  /* 发送 读 地址高位 */
//  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
//  /* 发送 读 地址中位 */
//  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
//  /* 发送 读 地址低位 */
//  SPI_FLASH_SendByte(ReadAddr & 0xFF);
//  
//	/* 读取数据 */
//  while (NumByteToRead--)
//  {
//    /* 读取一个字节*/
//    *pBuffer = SPI_FLASH_ReceiveByte();
//    /* 指向下一个字节缓冲区 */
//    pBuffer++;
//  }

//  /* 停止信号 FLASH: CS 高电平 */
//  SPI_FLASH_CS_HIGH();
//}

///**
//  * @brief  读取FLASH ID
//  * @param 	无
//  * @retval FLASH ID
//  */
//uint32_t SPI_FLASH_ReadID(void)
//{
//    uint8_t id[3];

//    SPI_FLASH_CS_LOW();

//    uint8_t cmd = W25X_JedecDeviceID;
//    HAL_SPI_Transmit(&hspi2, &cmd, 1, MAX_TIMEOUT);

//    uint8_t dummy = 0xFF;
//    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[0], 1, MAX_TIMEOUT);
//    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[1], 1, MAX_TIMEOUT);
//    HAL_SPI_TransmitReceive(&hspi2, &dummy, &id[2], 1, MAX_TIMEOUT);

//    SPI_FLASH_CS_HIGH();

//    return ((uint32_t)id[0] << 16) |
//           ((uint32_t)id[1] << 8) |
//            (uint32_t)id[2];
//}


// /**
//  * @brief  读取FLASH Device ID
//  * @param 	无
//  * @retval FLASH Device ID
//  */
//uint32_t SPI_FLASH_ReadDeviceID(void)
//{
//  uint32_t Temp = 0;

//  /* Select the FLASH: Chip Select low */
//  SPI_FLASH_CS_LOW();
//#ifndef DEBUG
//  /* Send "RDID " instruction */
//  SPI_FLASH_SendByte(W25X_DeviceID);
//  SPI_FLASH_SendByte(Dummy_Byte);
//  SPI_FLASH_SendByte(Dummy_Byte);
//  SPI_FLASH_SendByte(Dummy_Byte);
//  
//  /* Read a byte from the FLASH */
//  Temp = SPI_FLASH_ReceiveByte();
//#else
//	SPI_FLASH_SendByte(W25X_DeviceID);
//	SPI_FLASH_SendByte(Dummy_Byte);
//  SPI_FLASH_SendByte(Dummy_Byte);
//	SPI_FLASH_SendByte(Dummy_Byte);
//	/* Read a byte from the FLASH */
//  Temp = SPI_FLASH_ReceiveByte();
//#endif
//  /* Deselect the FLASH: Chip Select high */
//  SPI_FLASH_CS_HIGH();

//  return Temp;
//}



//void SPI_FLASH_SectorErase(uint32_t SectorAddr)
//{
//  /* 发送FLASH写使能命令 */
//  SPI_FLASH_WriteEnable();
//  SPI_FLASH_WaitForWriteEnd();
//  /* 擦除扇区 */
//  /* 选择FLASH: CS低电平 */
//  SPI_FLASH_CS_LOW();
//  /* 发送扇区擦除指令*/
//  SPI_FLASH_SendByte(W25X_SectorErase);
//  /*发送擦除扇区地址的高位*/
//  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
//  /* 发送擦除扇区地址的中位 */
//  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
//  /* 发送擦除扇区地址的低位 */
//  SPI_FLASH_SendByte(SectorAddr & 0xFF);
//  /* 停止信号 FLASH: CS 高电平 */
//  SPI_FLASH_CS_HIGH();
//  /* 等待擦除完毕*/
//  SPI_FLASH_WaitForWriteEnd();
//}

//void SPI_Flash_PowerDown(void)   
//{ 
//  /* 选择 FLASH: CS 低 */
//  SPI_FLASH_CS_LOW();

//  /* 发送 掉电 命令 */
//  SPI_FLASH_SendByte(W25X_PowerDown);

//  /* 停止信号  FLASH: CS 高 */
//  SPI_FLASH_CS_HIGH();
//}   

///**
//  * @brief  等待超时回调函数
//  * @param  None.
//  * @retval None.
//  */
//static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
//{
//  /* 等待超时后的处理,输出错误信息 */
//  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
//  return 0;
//}

///*
// * 函数名：Buffercmp
// * 描述  ：比较两个缓冲区中的数据是否相等
// * 输入  ：-pBuffer1     src缓冲区指针
// *         -pBuffer2     dst缓冲区指针
// *         -BufferLength 缓冲区长度
// * 输出  ：无
// * 返回  ：-PASSED pBuffer1 等于   pBuffer2
// *         -FAILED pBuffer1 不同于 pBuffer2
// */
//TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
//{
//  while(BufferLength--)
//  {
//    if(*pBuffer1 != *pBuffer2)
//    {
//      return FAILED;
//    }

//    pBuffer1++;
//    pBuffer2++;
//  }
//  return PASSED;
//}


//////对读取 写入进行保护 使用互斥信号量

////extern SPI_HandleTypeDef W25Q64_SPI;
//////设置片选
////static void W25Q64_CS_Low(void)
////{
////	HAL_GPIO_WritePin(W25Q64_CS_GPIO,W25Q64_CS_PIN,GPIO_PIN_RESET);
////}


////static void W25Q64_CS_High(void)
////{
////	HAL_GPIO_WritePin(W25Q64_CS_GPIO,W25Q64_CS_PIN,GPIO_PIN_SET);
////}

//////写使能

////void W25Q64_WriteEnable(void)
////{
////	uint8_t cmd = W25Q64_CMD_WRITE_ENABLE;
////	W25Q64_CS_Low();
////	HAL_SPI_Transmit(&W25Q64_SPI, &cmd, 1, HAL_MAX_DELAY);
////	W25Q64_CS_High();
////}

//////读取状态寄存器
////uint8_t W25Q64_ReadStatus1(void)
////{
////	xSemaphoreTake(W25Q64_handle,portMAX_DELAY);
////	uint8_t cmd = W25Q64_CMD_READ_STATUS1;
////	uint8_t status;
////	W25Q64_CS_Low();
////	HAL_SPI_Transmit(&W25Q64_SPI, &cmd, 1, HAL_MAX_DELAY);
////	HAL_SPI_Receive(&W25Q64_SPI, &status, 1, HAL_MAX_DELAY);
////	W25Q64_CS_High();
////	xSemaphoreGive(W25Q64_handle);
////	return status;
////}

//////等待空闲，WIP=0
////void W25Q64_WaitBusy(void)
////{
////    uint8_t cmd = W25Q64_CMD_READ_STATUS1;
////    uint8_t status;

////    do {
////        W25Q64_CS_Low();
////        HAL_SPI_Transmit(&W25Q64_SPI, &cmd, 1, HAL_MAX_DELAY);
////        HAL_SPI_Receive(&W25Q64_SPI, &status, 1, HAL_MAX_DELAY);
////        W25Q64_CS_High();
////    } while (status & 0x01);  // WIP 位为 1 表示忙
////}


////uint32_t W25Q64_ReadID(void) 
////{
////		xSemaphoreTake(W25Q64_handle,portMAX_DELAY);
////    uint8_t cmd = W25Q64_CMD_READ_ID;
////    uint8_t id_buf[3];
////    W25Q64_CS_Low();
////    HAL_SPI_Transmit(&W25Q64_SPI, &cmd, 1, HAL_MAX_DELAY);
////    HAL_SPI_Receive(&W25Q64_SPI, id_buf, 3, HAL_MAX_DELAY);
////    W25Q64_CS_High();
////		xSemaphoreGive(W25Q64_handle);
////    return (id_buf[0] << 16) | (id_buf[1] << 8) | id_buf[2];
////}


////// === 读取数据 ===
////void W25Q64_ReadData(uint32_t addr, uint8_t* buf, uint32_t len)
////{
////		xSemaphoreTake(W25Q64_handle,portMAX_DELAY);
////    uint8_t cmd[4];
////    cmd[0] = W25Q64_CMD_READ_DATA;
////    cmd[1] = (addr >> 16) & 0xFF;
////    cmd[2] = (addr >> 8) & 0xFF;
////    cmd[3] = addr & 0xFF;
////    
////    W25Q64_CS_Low();
////    HAL_SPI_Transmit(&W25Q64_SPI, cmd, 4, HAL_MAX_DELAY);
////    HAL_SPI_Receive(&W25Q64_SPI, buf, len, HAL_MAX_DELAY);
////    W25Q64_CS_High();
////		xSemaphoreGive(W25Q64_handle);
////}
//// 
////// === 页编程（最大 256 字节，不能跨页） ===
////void W25Q64_PageProgram(uint32_t addr, const uint8_t* buf, uint32_t len) 
////{
////		xSemaphoreTake(W25Q64_handle,portMAX_DELAY);
////    if (len > W25Q64_PAGE_SIZE) len = W25Q64_PAGE_SIZE;
//// 
////    W25Q64_WriteEnable();
//// 
////    uint8_t cmd[4];
////    cmd[0] = W25Q64_CMD_PAGE_PROGRAM;
////    cmd[1] = (addr >> 16) & 0xFF;
////    cmd[2] = (addr >> 8) & 0xFF;
////    cmd[3] = addr & 0xFF;
//// 
////    W25Q64_CS_Low();
////    HAL_SPI_Transmit(&W25Q64_SPI, cmd, 4, HAL_MAX_DELAY);
////    HAL_SPI_Transmit(&W25Q64_SPI, (uint8_t*)buf, len, HAL_MAX_DELAY);
////    W25Q64_CS_High();
//// 
////    W25Q64_WaitBusy();
////		xSemaphoreGive(W25Q64_handle);
////}
//// 
////// === 扇区擦除（按4KB） ===
////void W25Q64_SectorErase(uint32_t addr) 
////{
////		xSemaphoreTake(W25Q64_handle,portMAX_DELAY);
////    W25Q64_WriteEnable();
//// 
////    uint8_t cmd[4];
////    cmd[0] = W25Q64_CMD_SECTOR_ERASE;
////    cmd[1] = (addr >> 16) & 0xFF;
////    cmd[2] = (addr >> 8) & 0xFF;
////    cmd[3] = addr & 0xFF;
//// 
////    W25Q64_CS_Low();
////    HAL_SPI_Transmit(&W25Q64_SPI, cmd, 4, HAL_MAX_DELAY);
////    W25Q64_CS_High();
//// 
////    W25Q64_WaitBusy();
////		xSemaphoreGive(W25Q64_handle);

////}
//// 
////// === 整片擦除 ===
////void W25Q64_ChipErase(void) 
////{
////		xSemaphoreTake(W25Q64_handle,portMAX_DELAY);	
////    W25Q64_WriteEnable();
//// 
////    uint8_t cmd = W25Q64_CMD_CHIP_ERASE;
////    W25Q64_CS_Low();
////    HAL_SPI_Transmit(&W25Q64_SPI, &cmd, 1, HAL_MAX_DELAY);
////    W25Q64_CS_High();
//// 
////    W25Q64_WaitBusy();
////		xSemaphoreGive(W25Q64_handle);
////}






