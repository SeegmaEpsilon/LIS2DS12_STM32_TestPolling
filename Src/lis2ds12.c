#include "includes.h"

extern SPI_HandleTypeDef hspi1;
stmdev_ctx_t dev_ctx;

#define FIFO_WATER_MARK 0

int32_t accelerometer_read(void *context, uint8_t sensorRegister, uint8_t *sensorData, uint16_t length)
{
  sensorRegister |= 0x80;
  CS_LOW;
  HAL_SPI_Transmit(&hspi1, &sensorRegister, 1, 1000);
  HAL_SPI_Receive(&hspi1, sensorData, length, 1000);
  CS_HIGH;
  return 0;
}

int32_t accelerometer_write(void *context, uint8_t sensorRegister, const uint8_t *sensorData, uint16_t length) // Функция записи в акселлерометр
{
  CS_LOW;
  HAL_SPI_Transmit(&hspi1, &sensorRegister, 1, 1000);
  HAL_SPI_Transmit(&hspi1, (uint8_t*)sensorData, length, 1000);
  CS_HIGH;
  return 0;
}

void lis2ds12_get_acceleration(void)      // Функция обработки данных с акселерометра и отправка по RS485
{
  int16_t raw[3] = {0};

  lis2ds12_acceleration_raw_get(&dev_ctx, raw);

  float xval = lis2ds12_from_fs4g_to_mg(raw[0]);
  float yval = lis2ds12_from_fs4g_to_mg(raw[1]);
  float zval = lis2ds12_from_fs4g_to_mg(raw[2]);

  print_UART("[LIS2DS12: DATA]: X: %.2f (mg)\r\n", xval);
  print_UART("[LIS2DS12: DATA]: Y: %.2f (mg)\r\n", yval);
  print_UART("[LIS2DS12: DATA]: Z: %.2f (mg)\r\n", zval);
  print_UART("[LIS2DS12: DATA]: ------------\r\n", zval);
}

void lis2ds12_initialization(void)				 // Функция инициализации акселлерометра
{
  dev_ctx.read_reg = accelerometer_read;
  dev_ctx.write_reg = accelerometer_write;

  uint8_t readedID = 0;
  uint8_t error_counters = 0;

  while(readedID != DEVICE_ID && error_counters != 10)
  {
    lis2ds12_device_id_get(&dev_ctx, &readedID);

    print_UART("[LIS2DS12: ID] Поиск акселерометра... \r\n");
    error_counters++;
  }

  if(readedID == DEVICE_ID) print_UART("[LIS2DS12: ID] Акселерометр обнаружен: LIS2DS12\r\n");
  else print_UART("[LIS2DS12: ID] Акселерометр LIS2DS12 не найден, получен ID: 0x%02X\r\n", readedID);

  NVIC_DisableIRQ(EXTI1_IRQn);

  lis2ds12_reg_t reg;

  reg.byte = 0;
  reg.ctrl1.bdu = 1;
  reg.ctrl1.fs = LIS2DS12_4g;
  reg.ctrl1.odr = (uint8_t)(LIS2DS12_XL_ODR_6k4Hz_HF & 0x0F);
  reg.ctrl1.hf_odr = (uint8_t)((LIS2DS12_XL_ODR_6k4Hz_HF & 0x10) >> 4);
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_CTRL1, (uint8_t*)&reg.byte, 1);

  reg.byte = 0;
  reg.ctrl2.i2c_disable = 1;
  reg.ctrl2.if_add_inc = 1;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_CTRL2, (uint8_t*)&reg.byte, 1);

  reg.byte = 0;
  reg.ctrl3.lir = 0;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_CTRL3, (uint8_t*)&reg.byte, 1);

  reg.byte = 0;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_CTRL5, (uint8_t*)&reg.byte, 1);

  reg.byte = 0;
  reg.fifo_ctrl.fmode = LIS2DS12_BYPASS_MODE;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_FIFO_CTRL, (uint8_t*)&reg.byte, 1);

#if FIFO_WATER_MARK > 0
  reg.byte = 0;
  reg.fifo_ctrl.fmode = LIS2DS12_STREAM_MODE;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_FIFO_CTRL, (uint8_t*)&reg.byte, 1);
#endif

  reg.byte = FIFO_WATER_MARK;
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_FIFO_THS, (uint8_t*)&reg.byte, 1);

  reg.byte = 0;
#if FIFO_WATER_MARK > 0
  reg.ctrl4.int1_fth = 1;
#else
  reg.ctrl4.int1_drdy = 1;
#endif
  lis2ds12_write_reg(&dev_ctx, LIS2DS12_CTRL4, (uint8_t*)&reg.byte, 1);

  // enable irq from INT1
  NVIC_EnableIRQ(EXTI1_IRQn);
}
