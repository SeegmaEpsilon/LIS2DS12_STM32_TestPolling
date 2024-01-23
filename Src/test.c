/*
 * test.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: agapitov
 */
#include "includes.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

#define PWM_MAX htim2.Init.Period
#define PWM_MIN 0

void test_led(void)      // Функция проверки светодиодов
{
  print_UART("[DO] Проверка дискретного выхода...\n\r");

  TIM2->CCR3 = PWM_MAX;
  HAL_Delay(2000);
  TIM2->CCR3 = PWM_MIN;
  HAL_Delay(2000);
}

void test_pwm(void)       // Функция проверки Ш�?М
{
  print_UART("[PWM] Проверка токовой петли...\n\r");

  uint8_t PWM_delay = 6;
  for(int i = 0; i <= PWM_MAX; i++)
  {
    TIM2->CCR2 = i;
    TIM2->CCR3 = i;
    HAL_Delay(PWM_delay);
  }

  for(int i = PWM_MAX; i >= 0; i--)
  {
    TIM2->CCR2 = i;
    TIM2->CCR3 = i;
    HAL_Delay(PWM_delay);
  }
  HAL_Delay(10);
}

void test_rs485(void)     // Функция проверки RS485
{
  char *string_test = "123";

  uint8_t str[10];

  print_UART("[RS-485/UART] Проверка RS-485/UART...\r\n");
  print_UART("[RS-485/UART] Отправьте число 123 в течение 10 секунд...\r\n");

  HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, str, strlen(string_test), 10000);

  if (status == HAL_OK)
  {
    if(strstr((char*)str, string_test) != NULL)
    {
      print_UART("[RS-485/UART] Приемопередача в порядке\r\n");
    }
    else
    {
      print_UART("[RS-485/UART] Принятое число отличается от 123\r\n");
    }
  }
  else
  {
    print_UART("[RS-485/UART] Время вышло\r\n");
  }
}
