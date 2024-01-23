/*
 * misc.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: agapitov
 */

#include "includes.h"

extern UART_HandleTypeDef huart1;

/* send str with value over UART */
void print_UART(const char *format, ...)
{
  uint8_t temp[200] = { 0 };

  va_list args;
  va_start(args, format);
  vsprintf((char *)temp, format, args);
  va_end(args);

  RS_485_ON;
  while (HAL_UART_Transmit(&huart1, temp, strlen((char *)temp), 100) == HAL_BUSY);
  RS_485_OFF;
}
