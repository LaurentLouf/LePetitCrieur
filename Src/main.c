/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "crc.h"
#include "dfsdm.h"
#include "dma.h"
#include "fmc.h"
#include "gpio.h"
#include "octospi.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32_lcd.h"
#include "stm32l562e_discovery_lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RECORD_BUFFER_SIZE 2048  //!< Maximum number of samples for FFT is 1024
#define NUMBER_BUFFER_SAVE \
  323  //!< We want to save 15s of sound after first detection of high noise.
       //!< We're sampling at 44.1kHz, which thus requires 661500 samples, or
       //!< 323 buffers of 2048 samples.

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
int32_t record_buffer[RECORD_BUFFER_SIZE];
__IO uint32_t DmaRecHalfBuffCplt = 0;
__IO uint32_t DmaRecBuffCplt = 0;
uint16_t number_half_buffer_saved;
bool flag_save_buffer = false;
bool flag_display_lcd_info = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LCD_Init(void);
void LCD_DeInit(void);
void LCD_Display_Microphone_Info_Init(void);
void LCD_Display_Microphone_Info_Update(uint32_t i_extreme_detected,
                                        uint32_t i_tick_begin);
void turn_off_user_leds(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
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
  MX_DFSDM1_Init();
  MX_FMC_Init();
  MX_OCTOSPI1_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  LCD_DeInit();
  // Start DFSDM conversions
  if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(
                    &hdfsdm1_filter0, record_buffer, RECORD_BUFFER_SIZE)) {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (DmaRecHalfBuffCplt == 1 || DmaRecBuffCplt == 1) {
      uint16_t i_start_save_buffer = 0;
      if (DmaRecBuffCplt == 1) {
        i_start_save_buffer = 0;
        DmaRecBuffCplt = 0;
      } else {
        i_start_save_buffer = RECORD_BUFFER_SIZE / 2;
        DmaRecHalfBuffCplt = 0;
      }

      // Perform actions when it's required to save the record buffers
      if (flag_save_buffer == true) {
        // save buffer
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        number_half_buffer_saved++;
        if (number_half_buffer_saved == 2 * NUMBER_BUFFER_SAVE) {
          flag_save_buffer = false;
        }
      }

      // If the display is no longer needed, turn it off
      if (flag_display_lcd_info == false) {
        LCD_DeInit();
      }

      // If there's no action currently performed, go back to sleep
      if (flag_save_buffer == false && flag_display_lcd_info == false) {
        turn_off_user_leds();
        HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
      }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void LCD_Init(void) {
  LCD_UTILS_Drv_t lcdDrv;

  /* Initialize the LCD */
  if (BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT) != BSP_ERROR_NONE) {
    Error_Handler();
  }

  /* Set UTIL_LCD functions */
  lcdDrv.DrawBitmap = BSP_LCD_DrawBitmap;
  lcdDrv.FillRGBRect = BSP_LCD_FillRGBRect;
  lcdDrv.DrawHLine = BSP_LCD_DrawHLine;
  lcdDrv.DrawVLine = BSP_LCD_DrawVLine;
  lcdDrv.FillRect = BSP_LCD_FillRect;
  lcdDrv.GetPixel = BSP_LCD_ReadPixel;
  lcdDrv.SetPixel = BSP_LCD_WritePixel;
  lcdDrv.GetXSize = BSP_LCD_GetXSize;
  lcdDrv.GetYSize = BSP_LCD_GetYSize;
  lcdDrv.SetLayer = BSP_LCD_SetActiveLayer;
  lcdDrv.GetFormat = BSP_LCD_GetFormat;
  UTIL_LCD_SetFuncDriver(&lcdDrv);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set the display on */
  if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) {
    Error_Handler();
  }
}

void LCD_DeInit(void) {
  /* Deinitialize the LCD */
  if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE) {
    Error_Handler();
  }
}

void turn_off_user_leds(void) {
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Half regular conversion complete callback.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  if (hdfsdm_filter == &hdfsdm1_filter0) {
    DmaRecHalfBuffCplt = 1;
  }
}

void LCD_Display_Microphone_Info_Init(void) {
  UTIL_LCD_SetFont(&Font12);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"Max detected", LEFT_MODE);
  UTIL_LCD_DrawHLine(0, 30, 240, UTIL_LCD_COLOR_DARKBLUE);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Time", LEFT_MODE);
}

void LCD_Display_Microphone_Info_Update(uint32_t i_extreme_detected,
                                        uint32_t i_tick_begin) {
  char message_detected[64] = {'\0'}, message_time[64] = {'\0'};
  sprintf(message_detected, "%010lu", i_extreme_detected);

  UTIL_LCD_SetFont(&Font12);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)message_detected, RIGHT_MODE);
  sprintf(message_time, "%03lu", HAL_GetTick() - i_tick_begin);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)message_time, RIGHT_MODE);
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this
  function using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  if (hdfsdm_filter == &hdfsdm1_filter0) {
    DmaRecBuffCplt = 1;
  }
}

/**
 * @brief  Filter analog watchdog callback.
 * @param  hdfsdm_filter DFSDM filter handle.
 * @param  Channel Corresponding channel.
 * @param  Threshold Low or high threshold has been reached.
 * @retval None
 */
void HAL_DFSDM_FilterAwdCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter,
                                 uint32_t Channel, uint32_t Threshold) {
  if (hdfsdm_filter == &hdfsdm1_filter0 && Channel == 1 &&
      flag_save_buffer == false) {
    flag_save_buffer = true;
    number_half_buffer_saved = 0;
  }
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1) {
    /* Toggle LED_RED with a period of one second */
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    HAL_Delay(1000);
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
   */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
