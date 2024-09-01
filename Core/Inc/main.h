/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void init_scope(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LOGIC_CH1_Pin GPIO_PIN_3
#define LOGIC_CH1_GPIO_Port GPIOA
#define LOGIC_CH2_Pin GPIO_PIN_4
#define LOGIC_CH2_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA
#define SDO_Pin GPIO_PIN_7
#define SDO_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_0
#define LCD_DC_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOB
#define BTN_1_Pin GPIO_PIN_12
#define BTN_1_GPIO_Port GPIOB
#define BTN_1_EXTI_IRQn EXTI15_10_IRQn
#define BTN_2_Pin GPIO_PIN_13
#define BTN_2_GPIO_Port GPIOB
#define BTN_2_EXTI_IRQn EXTI15_10_IRQn
#define BTN_3_Pin GPIO_PIN_14
#define BTN_3_GPIO_Port GPIOB
#define BTN_3_EXTI_IRQn EXTI15_10_IRQn
#define BTN_4_Pin GPIO_PIN_15
#define BTN_4_GPIO_Port GPIOB
#define BTN_4_EXTI_IRQn EXTI15_10_IRQn
#define ROTARY_Key_Pin GPIO_PIN_10
#define ROTARY_Key_GPIO_Port GPIOA
#define ROTARY_Key_EXTI_IRQn EXTI15_10_IRQn
#define LOGIC_CH4_Pin GPIO_PIN_3
#define LOGIC_CH4_GPIO_Port GPIOB
#define LOGIC_CH3_Pin GPIO_PIN_4
#define LOGIC_CH3_GPIO_Port GPIOB
#define ROTARY_BTN_Pin GPIO_PIN_5
#define ROTARY_BTN_GPIO_Port GPIOB
#define ROTARY_SD2_Pin GPIO_PIN_6
#define ROTARY_SD2_GPIO_Port GPIOB
#define ROTARY_SD1_Pin GPIO_PIN_7
#define ROTARY_SD1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
