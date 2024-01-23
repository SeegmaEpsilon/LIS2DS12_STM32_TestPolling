#ifndef LIS2DS12_H_
#define LIS2DS12_H_
//---------------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
//---------------------------------------------------------
#define CS_GPIO_PORT GPIOA
#define CS_PIN GPIO_PIN_4
#define CS_LOW HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_SET)
//---------------------------------------------------------
#define DUMMY_BYTE ((uint8_t)0x00)
//---------------------------------------------------------
#define READWRITE_CMD ((uint8_t)0x80)
#define MULTIPLEBYTE_CMD ((uint8_t)0x40)
//---------------------------------------------------------
#define DEVICE_ID ((uint8_t)0x43)
//---------------------------------------------------------
void lis2ds12_initialization();
void lis2ds12_get_acceleration();
//---------------------------------------------------------
#endif /* INC_LIS2DS12_H_ */
