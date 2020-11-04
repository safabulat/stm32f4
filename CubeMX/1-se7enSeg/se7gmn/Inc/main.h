/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#define segA_Pin GPIO_PIN_12
#define segA_GPIO_Port GPIOB
#define segB_Pin GPIO_PIN_13
#define segB_GPIO_Port GPIOB
#define segC_Pin GPIO_PIN_14
#define segC_GPIO_Port GPIOB
#define segD_Pin GPIO_PIN_15
#define segD_GPIO_Port GPIOB
#define segE_Pin GPIO_PIN_8
#define segE_GPIO_Port GPIOD
#define segF_Pin GPIO_PIN_9
#define segF_GPIO_Port GPIOD
#define segG_Pin GPIO_PIN_10
#define segG_GPIO_Port GPIOD
#define segDP_Pin GPIO_PIN_11
#define segDP_GPIO_Port GPIOD
#define ledYes_Pin GPIO_PIN_12
#define ledYes_GPIO_Port GPIOD
#define ledNo_Pin GPIO_PIN_14
#define ledNo_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
