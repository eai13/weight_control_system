/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "lptim.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g071xx.h"
#include "stm32g0xx_hal_adc.h"
#include <stdint.h>
#include "global_config.h"
#include "debug.h"
#include "24C02C.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t current[4];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  __set_PRIMASK(1);
  SCB->VTOR = 0x0800E000;
  __set_PRIMASK(0);
  __enable_irq();
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM14_Init();
  MX_LPTIM1_Init();
  MX_TIM1_Init();
  MX_TIM15_Init();
  MX_CRC_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(DRV1_DIR_0_GPIO_Port, DRV1_DIR_0_Pin, 0);
  HAL_GPIO_WritePin(DRV1_DIR_1_GPIO_Port, DRV1_DIR_1_Pin, 0);
  HAL_GPIO_WritePin(DRV2_DIR_0_GPIO_Port, DRV2_DIR_0_Pin, 0);
  HAL_GPIO_WritePin(DRV2_DIR_1_GPIO_Port, DRV2_DIR_1_Pin, 0);
  HAL_GPIO_WritePin(DRV3_DIR_0_GPIO_Port, DRV3_DIR_0_Pin, 0);
  HAL_GPIO_WritePin(DRV3_DIR_1_GPIO_Port, DRV3_DIR_1_Pin, 0);
  HAL_GPIO_WritePin(DRV4_DIR_0_GPIO_Port, DRV4_DIR_0_Pin, 0);
  HAL_GPIO_WritePin(DRV4_DIR_1_GPIO_Port, DRV4_DIR_1_Pin, 0);

  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, current, 4);
  HAL_TIM_Base_Start(&htim6);

  HAL_TIM_Encoder_Start(&ENCODER_1_TIMER, TIM_CHANNEL_ALL);
  ENCODER_1_COUNT = 0x7FFFFFFF;
  HAL_TIM_Encoder_Start(&ENCODER_2_TIMER, TIM_CHANNEL_ALL);
  ENCODER_2_COUNT = 0x7FFF;
  HAL_TIM_Encoder_Start(&ENCODER_3_TIMER, TIM_CHANNEL_ALL);
  ENCODER_3_COUNT = 0x7FFF;
  HAL_LPTIM_Encoder_Start(&ENCODER_4_TIMER, 65535);
  HAL_LPTIM_Counter_Start_IT(&ENCODER_4_TIMER, LPTIM1->ARR);

  HAL_TIM_PWM_Start(&PWM_1_TIMER, PWM_1_CHANNEL);
  PWM_1_DUTY = 0;
  HAL_TIM_PWM_Start(&PWM_2_TIMER, PWM_2_CHANNEL);
  PWM_2_DUTY = 0;
  HAL_TIM_PWM_Start(&PWM_3_TIMER, PWM_3_CHANNEL);
  PWM_3_DUTY = 0;
  HAL_TIM_PWM_Start(&PWM_4_TIMER, PWM_4_CHANNEL);
  PWM_4_DUTY = 0;

  print_in("APP2 Program Started\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPTIM1|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_TIM15|RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLKSOURCE_PCLK1;
  PeriphClkInit.Tim15ClockSelection = RCC_TIM15CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
