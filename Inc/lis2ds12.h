#ifndef LIS2DS12_H_
#define LIS2DS12_H_
//---------------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
//---------------------------------------------------------
#define ABS(x) (x < 0) ? (-x) : x
//---------------------------------------------------------
#define LD_PORT GPIOA
#define LD2 GPIO_PIN_2
#define LD12 GPIO_PIN_12
#define LD2_ON HAL_GPIO_WritePin(LD_PORT, LD2, GPIO_PIN_SET)
#define LD12_ON HAL_GPIO_WritePin(LD_PORT, LD12, GPIO_PIN_SET)
#define LD2_OFF HAL_GPIO_WritePin(LD_PORT, LD2, GPIO_PIN_RESET)
#define LD12_OFF HAL_GPIO_WritePin(LD_PORT, LD12, GPIO_PIN_RESET)
//---------------------------------------------------------
#define CS_GPIO_PORT GPIOA
#define CS_PIN GPIO_PIN_4
#define CS_ON HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_RESET)
#define CS_OFF HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_SET)
//---------------------------------------------------------
#define DUMMY_BYTE ((uint8_t)0x00)
//---------------------------------------------------------
#define LIS2DS12_WHO_AM_I_ADDR 0x0F
//---------------------------------------------------------
#define READWRITE_CMD ((uint8_t)0x80)
#define MULTIPLEBYTE_CMD ((uint8_t)0x40)
//---------------------------------------------------------
#define LIS2DS12_CTRL1                 ((uint8_t)0x20)
#define LIS2DS12_CTRL2                 ((uint8_t)0x21)
#define LIS2DS12_CTRL4                 ((uint8_t)0x23)
#define LIS2DS12_XL_ODR_100Hz_LP       ((uint8_t)0x0C)
#define LIS2DS12_XL_ODR_100Hz_HR       ((uint8_t)0x04)
//---------------------------------------------------------
#define RS_485_ON             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
#define RS_485_OFF            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//---------------------------------------------------------
#define LIS2DS12_FULLSCALE_2           ((uint8_t)0x00)
//---------------------------------------------------------
#define LIS2DS12_CTRL1_LSETTINGS       ((uint8_t)0x00)
//---------------------------------------------------------
#define LIS2DS12_SENSITIVITY_0_06G      0.06  /* 0.06 mg/digit*/
//---------------------------------------------------------
#define LIS2DS12_OUT_X_L               ((uint8_t)0x28)
#define LIS2DS12_OUT_X_H               ((uint8_t)0x29)
#define LIS2DS12_OUT_Y_L               ((uint8_t)0x2A)
#define LIS2DS12_OUT_Y_H               ((uint8_t)0x2B)
#define LIS2DS12_OUT_Z_L               ((uint8_t)0x2C)
#define LIS2DS12_OUT_Z_H               ((uint8_t)0x2D)
#define LIS2DS12_FIFO_THS              ((uint8_t)0x2E)
//---------------------------------------------------------
void Accel_Ini(void);
void Accel_ReadAcc(void);
//---------------------------------------------------------
#endif /* INC_LIS2DS12_H_ */
