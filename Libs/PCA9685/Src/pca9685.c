#include "pca9685.h"
#include "main.h"

static I2C_HandleTypeDef *pca_i2c = NULL;

HAL_StatusTypeDef PCA9685_Init(I2C_HandleTypeDef *hi2c)
{
    pca_i2c = hi2c;
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_EnableOutputs(void)
{
    HAL_GPIO_WritePin(PWM_EN_GPIO_Port, PWM_EN_Pin, GPIO_PIN_RESET);
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_DisableOutputs(void)
{
    HAL_GPIO_WritePin(PWM_EN_GPIO_Port, PWM_EN_Pin, GPIO_PIN_SET);
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_SetPWMFreq(uint16_t freq_hz)
{
    (void)freq_hz;
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_SetRaw(uint8_t channel, uint16_t on, uint16_t off)
{
    (void)channel;
    (void)on;
    (void)off;
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_SetDuty(uint8_t channel, uint8_t duty_percent)
{
    (void)channel;
    (void)duty_percent;
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_AllOff(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_AllOn(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_Sleep(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_Wakeup(void)
{
    return HAL_OK;
}
