/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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
#define PT1000_Pin GPIO_PIN_3
#define PT1000_GPIO_Port GPIOA
#define INPUT_ANALOG_Pin GPIO_PIN_5
#define INPUT_ANALOG_GPIO_Port GPIOA
#define DIGIN_Pin GPIO_PIN_7
#define DIGIN_GPIO_Port GPIOA
#define STAT2_Pin GPIO_PIN_14
#define STAT2_GPIO_Port GPIOB
#define STAT2_EXTI_IRQn EXTI15_10_IRQn
#define STAT1_Pin GPIO_PIN_8
#define STAT1_GPIO_Port GPIOA
#define STAT1_EXTI_IRQn EXTI9_5_IRQn
#define SWITCH_STATUS_Pin GPIO_PIN_10
#define SWITCH_STATUS_GPIO_Port GPIOA
#define SWITCH_STATUS_EXTI_IRQn EXTI15_10_IRQn
#define SHDN_Pin GPIO_PIN_4
#define SHDN_GPIO_Port GPIOB
#define STB_Pin GPIO_PIN_5
#define STB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define millis uwTick

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
