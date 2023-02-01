#include "lis2ds12.h"
#include "stdio.h"
#include "lis2ds12_reg.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
uint8_t buf2[8] = { 0 };
char str1[100] = { 0 };
#define FIFO_WATER_MARK 200

static void Error_Device(uint8_t error)			 // Обработчик ошибок: устройство не является LIS2DS12
{
  uint8_t strEr[100];

  sprintf((char*)strEr, "[LIS2DS12: ID - ERROR] Device is NOT LIS2DS12, founded ID: 0x%02X\r\n", error);
  RS_485_ON;
  while(HAL_UART_Transmit(&huart1, (uint8_t*)strEr, strlen((char*)strEr), 0x1000) == HAL_BUSY);
  HAL_Delay(100);
  RS_485_OFF;
}

static void Error_SPI(void)			 // Обработчик ошибок: SPI не подключен
{
  uint8_t strEr[] = "[SPI: ERROR] SPI is not linked\r\n";
  RS_485_ON;
  HAL_UART_Transmit(&huart1, (uint8_t*)strEr, strlen((char*)strEr), 0x1000);
  HAL_Delay(100);
  RS_485_OFF;
}

uint8_t SPIx_WriteRead(uint8_t Byte) 	 // Функция работы с SPI
{
  uint8_t receivedbyte = 0;
  if(HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&Byte, (uint8_t*)&receivedbyte, 1, 0x1000) != HAL_OK)
  {
    Error_SPI();
  }
  return receivedbyte;
}

void Accel_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) // Функция чтения с акселлерометра
{
  if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    ReadAddr |= (uint8_t) READWRITE_CMD;
  }
  CS_ON;
  SPIx_WriteRead(ReadAddr);
  while(NumByteToRead > 0x00)
  {
    *pBuffer = SPIx_WriteRead(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  CS_OFF;
}

void Accel_IO_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite) // Функция записи в акселлерометр
{
  CS_OFF;
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t) MULTIPLEBYTE_CMD;
  }
  CS_ON;
  SPIx_WriteRead(WriteAddr);
  while(NumByteToWrite >= 0x01)
  {
    SPIx_WriteRead(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  CS_OFF;
}

uint8_t Accel_ReadID(void)			 // Функция чтения идентификатора акселлерометра
{
  uint8_t ctrl = 0;
  Accel_IO_Read(&ctrl, LIS2DS12_WHO_AM_I_ADDR, 1);
  return ctrl;
}

void Accel_GetXYZ(int16_t *pData)	 // Функция получения данных с акселлерометра
{
  int8_t buffer[6];
  Accel_IO_Read((uint8_t*)&buffer[0], LIS2DS12_OUT_X_L, 1);
  Accel_IO_Read((uint8_t*)&buffer[1], LIS2DS12_OUT_X_H, 1);
  Accel_IO_Read((uint8_t*)&buffer[2], LIS2DS12_OUT_Y_L, 1);
  Accel_IO_Read((uint8_t*)&buffer[3], LIS2DS12_OUT_Y_H, 1);
  Accel_IO_Read((uint8_t*)&buffer[4], LIS2DS12_OUT_Z_L, 1);
  Accel_IO_Read((uint8_t*)&buffer[5], LIS2DS12_OUT_Z_H, 1);
  for(uint8_t i = 0; i < 3; i++)
  {
    float valueinfloat = ((buffer[2 * i + 1] << 8) + buffer[2 * i]);
    pData[i] = (int16_t)valueinfloat;
  }
}

void Accel_ReadAcc(void) 			// Функция обработки данных с акселлерометра и отправка по RS485
{
  int16_t buffer[3] = { 0 };
  float xval, yval, zval = 0;
  Accel_GetXYZ(buffer);
  xval = lis2ds12_from_fs4g_to_mg(buffer[0]);
  yval = lis2ds12_from_fs4g_to_mg(buffer[1]);
  zval = lis2ds12_from_fs4g_to_mg(buffer[2]);

  sprintf(str1, "[LIS2DS12: DATA]:\r\n X: %.2f (mg) \r\n Y: %.2f (mg) \r\n Z: %.2f (mg)\r\n", xval, yval, zval);
  RS_485_ON;
  HAL_UART_Transmit(&huart1, (uint8_t*)str1, strlen(str1), 0x1000);
  RS_485_OFF;
  HAL_Delay(100);
}

void Accel_Ini(void)				 // Функция инициализации акселлерометра
{
  uint8_t readedID = 0;
  uint8_t error_counters = 0;
  HAL_Delay(1000);

  while(readedID != 0x43 && error_counters != 10)
  {
    readedID = Accel_ReadID();
    char *device_trying = "[LIS2DS12: ID - LOADING] Looking for accelerometer... \r\n";
    RS_485_ON;
    HAL_UART_Transmit(&huart1, (uint8_t*)device_trying, strlen((char*)device_trying), 0x1000);
    HAL_Delay(100);
    RS_485_OFF;
    error_counters++;
  }

  if(readedID == 0x43)
  {
    char *device_ok = "[LIS2DS12: ID - OK] Device has been founded: LIS2DS12\r\n";
    RS_485_ON;
    HAL_UART_Transmit(&huart1, (uint8_t*)device_ok, strlen((char*)device_ok), 0x1000);
    HAL_Delay(100);
    RS_485_OFF;
  }
  else Error_Device(readedID);

  NVIC_DisableIRQ(EXTI1_IRQn);

  lis2ds12_reg_t reg;

  reg.byte = 0;
  reg.ctrl1.bdu = 1;
  reg.ctrl1.fs = LIS2DS12_4g;
  reg.ctrl1.odr = (uint8_t)(LIS2DS12_XL_ODR_6k4Hz_HF & 0x0F);
  reg.ctrl1.hf_odr = (uint8_t)((LIS2DS12_XL_ODR_6k4Hz_HF & 0x10) >> 4);
  Accel_IO_Write(&reg.byte, LIS2DS12_CTRL1, 1);

  reg.byte = 0;
  reg.ctrl2.i2c_disable = 1;
  reg.ctrl2.if_add_inc = 1;
  Accel_IO_Write(&reg.byte, LIS2DS12_CTRL2, 1);

  reg.byte = 0;
  reg.ctrl3.lir = 0;
  Accel_IO_Write(&reg.byte, LIS2DS12_CTRL3, 1);

  reg.byte = 0;
  Accel_IO_Write(&reg.byte, LIS2DS12_CTRL5, 1);

  reg.byte = 0;
  reg.fifo_ctrl.fmode = LIS2DS12_BYPASS_MODE;
  Accel_IO_Write(&reg.byte, LIS2DS12_FIFO_CTRL, 1);

#if FIFO_WATER_MARK > 0
  reg.byte = 0;
  reg.fifo_ctrl.fmode = LIS2DS12_STREAM_MODE;
  Accel_IO_Write(&reg.byte, LIS2DS12_FIFO_CTRL, 1);
#endif

  reg.byte = FIFO_WATER_MARK;
  Accel_IO_Write(&reg.byte, LIS2DS12_FIFO_THS, 1);

  reg.byte = 0;
#if FIFO_WATER_MARK > 0
  reg.ctrl4.int1_fth = 1;
#else
    reg.ctrl4.int1_drdy = 1;
  #endif
  Accel_IO_Write(&reg.byte, LIS2DS12_CTRL4, 1);

  // enable irq from INT1
  NVIC_EnableIRQ(EXTI1_IRQn);

}
