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
#include "stm32l5xx_hal.h"

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
#define LCD_BACKLIGHT_CONTROL_Pin \
  GPIO_PIN_1  //!< Redefined as LCD_BACKLIGHT_GPIO_PIN in
              //!< Src/stm32l562e_discovery_lcd.c
#define LCD_BACKLIGHT_CONTROL_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOG
#define LED_RED_Pin GPIO_PIN_3
#define LED_RED_GPIO_Port GPIOD
#define LCD_TE_Pin \
  GPIO_PIN_8  //!< Tearing effect. Used to synchronize MCU to frame memory
              //!< writing. PA8
#define LCD_TE_GPIO_Port GPIOA
#define LCD_PWR_ON_Pin \
  GPIO_PIN_0  //!< Control of the LCD power (excluding backlight). PH0
#define LCD_PWR_ON_GPIO_Port GPIOH
#define BLE_RSTN_Pin GPIO_PIN_8
#define BLE_RSTN_GPIO_Port GPIOG
#define LCD_RST_Pin GPIO_PIN_14
#define LCD_RST_GPIO_Port GPIOF
#define AUDIO_RESETN_Pin GPIO_PIN_1
#define AUDIO_RESETN_GPIO_Port GPIOG
#define CTP_RST_Pin GPIO_PIN_15
#define CTP_RST_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
