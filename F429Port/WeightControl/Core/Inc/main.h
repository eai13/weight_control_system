/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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
#define LED_PROTOCOL_Pin GPIO_PIN_0
#define LED_PROTOCOL_GPIO_Port GPIOB
#define LED_ERROR_Pin GPIO_PIN_14
#define LED_ERROR_GPIO_Port GPIOB
#define DRV4_DIR_0_Pin GPIO_PIN_2
#define DRV4_DIR_0_GPIO_Port GPIOG
#define DRV4_DIR_1_Pin GPIO_PIN_3
#define DRV4_DIR_1_GPIO_Port GPIOG
#define DRV1_DIR_0_Pin GPIO_PIN_8
#define DRV1_DIR_0_GPIO_Port GPIOC
#define DRV1_DIR_1_Pin GPIO_PIN_9
#define DRV1_DIR_1_GPIO_Port GPIOC
#define DRV3_DIR_1_Pin GPIO_PIN_4
#define DRV3_DIR_1_GPIO_Port GPIOD
#define DRV3_DIR_0_Pin GPIO_PIN_5
#define DRV3_DIR_0_GPIO_Port GPIOD
#define DRV2_DIR_0_Pin GPIO_PIN_6
#define DRV2_DIR_0_GPIO_Port GPIOD
#define EMERGENCY_STOP_Pin GPIO_PIN_7
#define EMERGENCY_STOP_GPIO_Port GPIOD
#define DRV2_DIR_1_Pin GPIO_PIN_9
#define DRV2_DIR_1_GPIO_Port GPIOG
#define LED_CONTROL_Pin GPIO_PIN_7
#define LED_CONTROL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define LED_PROTOCOL_UP()     HAL_GPIO_WritePin(LED_PROTOCOL_GPIO_Port, LED_PROTOCOL_Pin, 1)
#define LED_PROTOCOL_DOWN()   HAL_GPIO_WritePin(LED_PROTOCOL_GPIO_Port, LED_PROTOCOL_Pin, 0)
#define LED_PROTOCOL_TOGGLE() HAL_GPIO_TogglePin(LED_PROTOCOL_GPIO_Port, LED_PROTOCOL_Pin)

#define LED_ERROR_UP()        HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, 1)
#define LED_ERROR_DOWN()      HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, 0)
#define LED_ERROR_TOGGLE()    HAL_GPIO_TogglePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin)

#define LED_CONTROL_UP()      HAL_GPIO_WritePin(LED_CONTROL_GPIO_Port, LED_CONTROL_Pin, 1)
#define LED_CONTROL_DOWN()    HAL_GPIO_WritePin(LED_CONTROL_GPIO_Port, LED_CONTROL_Pin, 0)
#define LED_CONTROL_TOGGLE()  HAL_GPIO_TogglePin(LED_CONTROL_GPIO_Port, LED_CONTROL_Pin)

#define _IS_EM_STOP_() \
  (HAL_GPIO_ReadPin(EMERGENCY_STOP_GPIO_Port, EMERGENCY_STOP_Pin) == 0)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
