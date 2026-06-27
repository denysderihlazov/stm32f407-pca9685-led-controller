#ifndef PCA9685_H
#define PCA9685_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define PCA9685_I2C_ADDR       (0x40 << 1)

#define PCA9685_MODE1          0x00
#define PCA9685_MODE2          0x01
#define PCA9685_LED0_ON_L      0x06
#define PCA9685_ALL_LED_ON_L   0xFA
#define PCA9685_ALL_LED_ON_H   0xFB
#define PCA9685_ALL_LED_OFF_L  0xFC
#define PCA9685_ALL_LED_OFF_H  0xFD
#define PCA9685_PRE_SCALE      0xFE

HAL_StatusTypeDef PCA9685_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef PCA9685_SetPWMFreq(uint16_t freq_hz);
HAL_StatusTypeDef PCA9685_SetRaw(uint8_t channel, uint16_t on, uint16_t off);
HAL_StatusTypeDef PCA9685_SetDuty(uint8_t channel, uint8_t duty_percent);

HAL_StatusTypeDef PCA9685_AllOff(void);
HAL_StatusTypeDef PCA9685_AllOn(void);

HAL_StatusTypeDef PCA9685_Sleep(void);
HAL_StatusTypeDef PCA9685_Wakeup(void);

HAL_StatusTypeDef PCA9685_EnableOutputs(void);
HAL_StatusTypeDef PCA9685_DisableOutputs(void);

#endif
