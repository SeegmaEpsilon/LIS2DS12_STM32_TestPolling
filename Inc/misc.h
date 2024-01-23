/*
 * misc.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: agapitov
 */

#ifndef MISC_H_
#define MISC_H_

#define RS_485_ON             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET)
#define RS_485_OFF            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET)

void print_UART(const char *format, ...);

#endif /* MISC_H_ */
