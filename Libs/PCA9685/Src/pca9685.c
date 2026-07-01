#include "pca9685.h"
#include "main.h"

static I2C_HandleTypeDef *pca_i2c = NULL;

static HAL_StatusTypeDef PCA9685_WriteReg(uint8_t reg, uint8_t value)
{
    if (pca_i2c == NULL)
        return HAL_ERROR;

    return HAL_I2C_Mem_Write(pca_i2c,
                             PCA9685_I2C_ADDR,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             &value,
                             1,
                             HAL_MAX_DELAY);
}

static HAL_StatusTypeDef PCA9685_ReadReg(uint8_t reg, uint8_t *value)
{
    if (pca_i2c == NULL || value == NULL)
        return HAL_ERROR;

    return HAL_I2C_Mem_Read(pca_i2c,
                            PCA9685_I2C_ADDR,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            value,
                            1,
                            HAL_MAX_DELAY);
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

HAL_StatusTypeDef PCA9685_Sleep(void)
{
    uint8_t mode1;

    if (PCA9685_ReadReg(PCA9685_MODE1, &mode1) != HAL_OK)
        return HAL_ERROR;

    mode1 |= 0x10; // SLEEP bit

    return PCA9685_WriteReg(PCA9685_MODE1, mode1);
}

HAL_StatusTypeDef PCA9685_Wakeup(void)
{
    uint8_t mode1;

    if (PCA9685_ReadReg(PCA9685_MODE1, &mode1) != HAL_OK)
        return HAL_ERROR;

    mode1 &= (uint8_t)(~0x10); // clear SLEEP bit

    if (PCA9685_WriteReg(PCA9685_MODE1, mode1) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(1);

    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_SetPWMFreq(uint16_t freq_hz)
{
    if (freq_hz < 24)
        freq_hz = 24;

    if (freq_hz > 1526)
        freq_hz = 1526;

    /*
     * PCA9685 oscillator is usually 25 MHz.
     * prescale = round(25MHz / (4096 * freq)) - 1
     */
    uint32_t prescale_calc = 25000000UL;
    prescale_calc += (uint32_t)freq_hz * 2048UL; // rounding helper
    prescale_calc /= (uint32_t)freq_hz * 4096UL;

    if (prescale_calc > 0)
        prescale_calc -= 1;

    uint8_t prescale = (uint8_t)prescale_calc;

    uint8_t old_mode;

    if (PCA9685_ReadReg(PCA9685_MODE1, &old_mode) != HAL_OK)
        return HAL_ERROR;

    uint8_t sleep_mode = (old_mode & 0x7F) | 0x10;

    if (PCA9685_WriteReg(PCA9685_MODE1, sleep_mode) != HAL_OK)
        return HAL_ERROR;

    if (PCA9685_WriteReg(PCA9685_PRE_SCALE, prescale) != HAL_OK)
        return HAL_ERROR;

    if (PCA9685_WriteReg(PCA9685_MODE1, old_mode) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(1);

    return PCA9685_WriteReg(PCA9685_MODE1, old_mode | 0x80); // RESTART bit
}

HAL_StatusTypeDef PCA9685_SetRaw(uint8_t channel, uint16_t on, uint16_t off)
{
    if (pca_i2c == NULL)
        return HAL_ERROR;

    if (channel > 15)
        return HAL_ERROR;

    if (on > 4095)
        on = 4095;

    if (off > 4095)
        off = 4095;

    uint8_t reg = PCA9685_LED0_ON_L + (4U * channel);

    uint8_t data[4];

    data[0] = (uint8_t)(on & 0xFF);
    data[1] = (uint8_t)(on >> 8);
    data[2] = (uint8_t)(off & 0xFF);
    data[3] = (uint8_t)(off >> 8);

    return HAL_I2C_Mem_Write(pca_i2c,
                             PCA9685_I2C_ADDR,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             data,
                             4,
                             HAL_MAX_DELAY);
}

HAL_StatusTypeDef PCA9685_SetDuty(uint8_t channel, uint8_t duty_percent)
{
    if (duty_percent > 100)
        duty_percent = 100;

    if (duty_percent == 0)
        return PCA9685_SetRaw(channel, 0, 0);

    if (duty_percent == 100)
        return PCA9685_SetRaw(channel, 0, 4095);

    uint16_t off = (uint16_t)((4095UL * duty_percent) / 100UL);

    return PCA9685_SetRaw(channel, 0, off);
}

HAL_StatusTypeDef PCA9685_AllOff(void)
{
    HAL_StatusTypeDef status;

    for (uint8_t ch = 0; ch < 16; ch++)
    {
        status = PCA9685_SetDuty(ch, 0);

        if (status != HAL_OK)
            return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_AllOn(void)
{
    HAL_StatusTypeDef status;

    for (uint8_t ch = 0; ch < 16; ch++)
    {
        status = PCA9685_SetDuty(ch, 100);

        if (status != HAL_OK)
            return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef PCA9685_Init(I2C_HandleTypeDef *hi2c)
{
    pca_i2c = hi2c;

    PCA9685_DisableOutputs();

    HAL_Delay(10);

    if (HAL_I2C_IsDeviceReady(pca_i2c, PCA9685_I2C_ADDR, 3, 100) != HAL_OK)
        return HAL_ERROR;

    // MODE1 = normal mode, auto-increment enabled
    if (PCA9685_WriteReg(PCA9685_MODE1, 0x20) != HAL_OK)
        return HAL_ERROR;

    // MODE2 = totem pole output
    if (PCA9685_WriteReg(PCA9685_MODE2, 0x04) != HAL_OK)
        return HAL_ERROR;

    if (PCA9685_SetPWMFreq(100) != HAL_OK)
        return HAL_ERROR;

    if (PCA9685_AllOff() != HAL_OK)
        return HAL_ERROR;

    PCA9685_EnableOutputs();

    return HAL_OK;
}

