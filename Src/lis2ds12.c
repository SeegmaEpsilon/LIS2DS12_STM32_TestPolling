#include "lis2ds12.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
uint8_t buf2[8]={0};
char str1[100]={0};

static void Error_Device(uint8_t error)			 // Обработчик ошибок: устройство не является LIS2DS12
{
	uint8_t strEr[100];

	sprintf((char*)strEr, "[ERROR] Device is not LIS2DS12, founded ID: 0x%02X\r\n", error);
	RS_485_ON;
	while(HAL_UART_Transmit(&huart1, (uint8_t*)strEr, strlen((char*)strEr), 0x1000) == HAL_BUSY);

	HAL_Delay(100);
	RS_485_OFF;
}

static void Error_SPI (void)			 // Обработчик ошибок: SPI не подключен
{
	uint8_t strEr[]="[ERROR] SPI is not linked\r\n";
	RS_485_ON;
	HAL_UART_Transmit(&huart1, (uint8_t*)strEr, strlen((char*)strEr), 0x1000);
	HAL_Delay(100);
	RS_485_OFF;
}

uint8_t SPIx_WriteRead(uint8_t Byte) 	 // Функция работы с SPI
{
	uint8_t receivedbyte = 0;
	if(HAL_SPI_TransmitReceive(&hspi1,(uint8_t*) &Byte,(uint8_t*) &receivedbyte,1,0x1000) != HAL_OK)
	{
		Error_SPI();
	}
	return receivedbyte;
}

void Accel_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) // Функция чтения с акселлерометра
{
	if(NumByteToRead>0x01)
	{
		ReadAddr |= (uint8_t) (READWRITE_CMD | MULTIPLEBYTE_CMD);
	}
	else
	{
		ReadAddr |= (uint8_t) READWRITE_CMD;
	}
	CS_ON;
	SPIx_WriteRead(ReadAddr);
	while(NumByteToRead>0x00)
	{
		 *pBuffer=SPIx_WriteRead(DUMMY_BYTE);
		 NumByteToRead--;
		 pBuffer++;
	}
	CS_OFF;
}

void Accel_IO_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite) // Функция записи в акселлерометр
{
	CS_OFF;
	if(NumByteToWrite>0x01)
	{
		WriteAddr |= (uint8_t) MULTIPLEBYTE_CMD;
	}
	CS_ON;
	SPIx_WriteRead(WriteAddr);
	while(NumByteToWrite>=0x01)
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

void Accel_GetXYZ(int16_t* pData)	 // Функция получения данных с акселлерометра
{
	int8_t buffer[6];
	uint8_t ctrl, i = 0x00;
	float sensitivity = LIS2DS12_SENSITIVITY_0_06G;
	float valueinfloat;
	Accel_IO_Read(&ctrl, LIS2DS12_CTRL1, 1);
	Accel_IO_Read((uint8_t*)&buffer[0], LIS2DS12_OUT_X_L, 1);
	Accel_IO_Read((uint8_t*)&buffer[1], LIS2DS12_OUT_X_H, 1);
	Accel_IO_Read((uint8_t*)&buffer[2], LIS2DS12_OUT_Y_L, 1);
	Accel_IO_Read((uint8_t*)&buffer[3], LIS2DS12_OUT_Y_H, 1);
	Accel_IO_Read((uint8_t*)&buffer[4], LIS2DS12_OUT_Z_L, 1);
	Accel_IO_Read((uint8_t*)&buffer[5], LIS2DS12_OUT_Z_H, 1);
	for(i=0;i<3;i++)
	{
		valueinfloat = ((buffer[2*i+1]  << 8) + buffer[2*i])*sensitivity;
		pData[i]=(int16_t)valueinfloat;
	}
}

void Accel_ReadAcc(void) 			// Функция обработки данных с акселлерометра и отправка по RS485
{
	int16_t buffer[3]={0};
	int16_t xval, yval, zval = 0x0000;
	Accel_GetXYZ(buffer);
	xval = buffer[0];
	yval = buffer[1];
	zval = buffer[2];
	sprintf(str1, "[DATA] X:%06d Y:%06d Z:%06d\r\n", xval, yval, zval);
	RS_485_ON;
	HAL_UART_Transmit(&huart1, (uint8_t*)str1, strlen(str1), 0x1000);
	RS_485_OFF;
	HAL_Delay(100);
}

void Accel_Ini(void)				 // Функция инициализации акселлерометра
{
	uint8_t ctrl1 = 0x00;
	uint8_t ctrl2 = 0x00;
	uint8_t ctrl4 = 0x00;
	HAL_Delay(1000);
	uint8_t readedID = Accel_ReadID();
	if(readedID==0x43)
		{
	  char* device_ok = "[SUCCESS] Device has been founded: LIS2DS12\r\n";
		RS_485_ON;
		HAL_UART_Transmit(&huart1, (uint8_t*)device_ok, strlen((char*)device_ok), 0x1000);
		HAL_Delay(100);
		RS_485_OFF;;
		}
	else Error_Device(readedID);
	ctrl1 = (uint8_t) (0x60);
	ctrl2 = (uint8_t) (0x20);
	ctrl4 = (uint8_t) (0x03);
	Accel_IO_Write(&ctrl2, LIS2DS12_CTRL2, 1);
	Accel_IO_Write(&ctrl1, LIS2DS12_CTRL1, 1);
	Accel_IO_Write(&ctrl4, LIS2DS12_CTRL4, 1);
}
