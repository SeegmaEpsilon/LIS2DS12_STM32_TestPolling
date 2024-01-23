/*
 * cpu.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: agapitov
 */
#include "includes.h"

extern TIM_HandleTypeDef htim2;
extern stmdev_ctx_t dev_ctx;

uint32_t INT1_counts = 0;
uint32_t INT2_counts = 0;
uint32_t INTx_counts = 0;

uint32_t acceleration_counter = 0;

#define VD15_TEST_VERSION "1.3"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_1)
  {
    INT1_counts++;
  }
  else if(GPIO_Pin == GPIO_PIN_2)
  {
    INT2_counts++;
  }
  else INTx_counts++;
}

void cpu_init()
{
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

  while(1) cpu_loop();
}

void cpu_loop()
{
  const uint32_t DELAY_BETWEEN_TEST = 2000;
  const uint8_t DATA_REQUEST_COUNT = 5;

  INT1_counts = 0;
  INT2_counts = 0;
  INTx_counts = 0;

  acceleration_counter = 0;

  print_UART("[INIT] ВД15 модульный тест, версия прошивки: %s\n\r", VD15_TEST_VERSION);

  test_led();       // Вызов функции проверки светодиодов
  HAL_Delay(DELAY_BETWEEN_TEST);

  test_pwm();       // Вызов функции проверки ШИМ
  HAL_Delay(DELAY_BETWEEN_TEST);

  test_rs485();     // Вызов функции проверки RS485
  HAL_Delay(DELAY_BETWEEN_TEST);

  lis2ds12_initialization();       // Вызов функция инициализации акселлерометра

  while (1)
  {
    /* Read output only if new value is available. */
    lis2ds12_reg_t reg;
    lis2ds12_status_reg_get(&dev_ctx, &reg.status);

    if(acceleration_counter == DATA_REQUEST_COUNT) break;

    if (reg.status.drdy)
    {
      /* Read acceleration data. */
      lis2ds12_get_acceleration();
      acceleration_counter++;
    }
  }

  print_UART("[LIS2DS12: INTERRUPTS] Количество сработанных прерываний: %d из %d\n\r", INT1_counts, DATA_REQUEST_COUNT);
  print_UART("\r\n\r\n");
  HAL_Delay(DELAY_BETWEEN_TEST);
}
