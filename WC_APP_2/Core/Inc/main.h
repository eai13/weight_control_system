/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DRV1_DIR_0_Pin GPIO_PIN_11
#define DRV1_DIR_0_GPIO_Port GPIOC
#define LPTIM1_ENC_4_A_Pin GPIO_PIN_12
#define LPTIM1_ENC_4_A_GPIO_Port GPIOC
#define TIM15_PWM_1_Pin GPIO_PIN_1
#define TIM15_PWM_1_GPIO_Port GPIOC
#define LPTIM1_ENC_4_B_Pin GPIO_PIN_2
#define LPTIM1_ENC_4_B_GPIO_Port GPIOC
#define DRV4_DIR_0_Pin GPIO_PIN_4
#define DRV4_DIR_0_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOA
#define TIM3_ENC_3_A_Pin GPIO_PIN_6
#define TIM3_ENC_3_A_GPIO_Port GPIOA
#define TIM3_ENC_3_B_Pin GPIO_PIN_7
#define TIM3_ENC_3_B_GPIO_Port GPIOA
#define TIM2_ENC_1_A_Pin GPIO_PIN_4
#define TIM2_ENC_1_A_GPIO_Port GPIOC
#define TIM2_ENC_1_B_Pin GPIO_PIN_5
#define TIM2_ENC_1_B_GPIO_Port GPIOC
#define DRV4_DIR_1_Pin GPIO_PIN_1
#define DRV4_DIR_1_GPIO_Port GPIOB
#define TIM15_PWM_2_Pin GPIO_PIN_15
#define TIM15_PWM_2_GPIO_Port GPIOB
#define TIM1_ENC_2_A_Pin GPIO_PIN_8
#define TIM1_ENC_2_A_GPIO_Port GPIOA
#define DRV3_DIR_1_Pin GPIO_PIN_15
#define DRV3_DIR_1_GPIO_Port GPIOA
#define TIM1_ENC_2_B_Pin GPIO_PIN_9
#define TIM1_ENC_2_B_GPIO_Port GPIOC
#define TIM16_PWM_3_Pin GPIO_PIN_0
#define TIM16_PWM_3_GPIO_Port GPIOD
#define TIM17_PWM_4_Pin GPIO_PIN_1
#define TIM17_PWM_4_GPIO_Port GPIOD
#define DRV1_DIR_1_Pin GPIO_PIN_2
#define DRV1_DIR_1_GPIO_Port GPIOD
#define DRV2_DIR_0_Pin GPIO_PIN_3
#define DRV2_DIR_0_GPIO_Port GPIOD
#define DRV3_DIR_0_Pin GPIO_PIN_4
#define DRV3_DIR_0_GPIO_Port GPIOD
#define DRV2_DIR_1_Pin GPIO_PIN_7
#define DRV2_DIR_1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
