/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pca9685.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t rx_byte = 0;
volatile uint8_t uart_cmd_ready = 0;
volatile uint8_t uart_cmd = 0;
uint8_t pwm_freq_state = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void UART_Print(const char *msg)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

static void Print_Help(void)
{
    UART_Print("\r\nCommands:\r\n");
    UART_Print("h - help\r\n");
    UART_Print("o - enable PCA9685 outputs\r\n");
    UART_Print("x - disable PCA9685 outputs\r\n");
    UART_Print("t - test 4 channels: 25%, 50%, 75%, 100%\r\n");
    UART_Print("a - all channels off\r\n");
    UART_Print("b - all channels on\r\n");
    UART_Print("s - sleep mode\r\n");
    UART_Print("w - wakeup\r\n");
    UART_Print("d - fade demo on channel 0\r\n");
    UART_Print("f - set PWM frequency test: 50 Hz / 100 Hz / 200 Hz\r\n");
}

static void Test4_Channels(void)
{
    PCA9685_SetDuty(0, 25);
    PCA9685_SetDuty(1, 50);
    PCA9685_SetDuty(2, 75);
    PCA9685_SetDuty(3, 100);

    UART_Print("Test4: CH0=25%, CH1=50%, CH2=75%, CH3=100%\r\n");
}

static void Test_PWM_Frequency(void)
{
    if (pwm_freq_state == 0)
    {
        PCA9685_SetPWMFreq(50);
        UART_Print("PWM frequency set to 50 Hz\r\n");
        pwm_freq_state = 1;
    }
    else if (pwm_freq_state == 1)
    {
        PCA9685_SetPWMFreq(100);
        UART_Print("PWM frequency set to 100 Hz\r\n");
        pwm_freq_state = 2;
    }
    else
    {
        PCA9685_SetPWMFreq(200);
        UART_Print("PWM frequency set to 200 Hz\r\n");
        pwm_freq_state = 0;
    }
}

static void Demo_Fade_CH0(void)
{
    UART_Print("Fade demo on CH0 started\r\n");

    for (uint8_t duty = 0; duty <= 100; duty += 5)
    {
        PCA9685_SetDuty(0, duty);
        HAL_Delay(40);
    }

    for (int duty = 100; duty >= 0; duty -= 5)
    {
        PCA9685_SetDuty(0, duty);
        HAL_Delay(40);
    }

    UART_Print("Fade demo on CH0 finished\r\n");
}

static void Process_UART_Command(uint8_t cmd)
{
	if (cmd == '\r' || cmd == '\n')
	{
	    return;
	}
    switch (cmd)
    {
        case 'h':
        case 'H':
            Print_Help();
            break;

        case 'o':
        case 'O':
            PCA9685_EnableOutputs();
            UART_Print("Outputs enabled\r\n");
            break;

        case 'x':
        case 'X':
            PCA9685_DisableOutputs();
            UART_Print("Outputs disabled\r\n");
            break;

        case 't':
        case 'T':
            Test4_Channels();
            break;

        case 'f':
        case 'F':
            Test_PWM_Frequency();
            break;

        case 'a':
        case 'A':
            PCA9685_AllOff();
            UART_Print("All channels off\r\n");
            break;

        case 'b':
        case 'B':
            PCA9685_AllOn();
            UART_Print("All channels on\r\n");
            break;

        case 's':
        case 'S':
            PCA9685_Sleep();
            UART_Print("Sleep mode enabled\r\n");
            break;

        case 'w':
        case 'W':
            PCA9685_Wakeup();
            UART_Print("Wakeup done\r\n");
            break;

        case 'd':
        case 'D':
            Demo_Fade_CH0();
            break;

        default:
            UART_Print("Unknown command. Press h for help.\r\n");
            break;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(100);

  UART_Print("\r\nSTM32F407 PCA9685 LED Controller\r\n");

  if (PCA9685_Init(&hi2c1) != HAL_OK)
  {
      UART_Print("PCA9685 init ERROR\r\n");
      Error_Handler();
  }

  UART_Print("PCA9685 init OK\r\n");

  Print_Help();

  HAL_UART_Receive_IT(&huart3, &rx_byte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if (uart_cmd_ready)
      {
	      uart_cmd_ready = 0;
	      Process_UART_Command(uart_cmd);
      }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 50000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWM_EN_GPIO_Port, PWM_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PWM_EN_Pin */
  GPIO_InitStruct.Pin = PWM_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWM_EN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        uart_cmd = rx_byte;
        uart_cmd_ready = 1;

        HAL_UART_Receive_IT(&huart3, &rx_byte, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
