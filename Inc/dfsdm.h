/**
 ******************************************************************************
 * File Name          : DFSDM.h
 * Description        : This file provides code for the configuration
 *                      of the DFSDM instances.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __dfsdm_H
#define __dfsdm_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
extern DFSDM_Channel_HandleTypeDef hdfsdm1_channel1;

/* USER CODE BEGIN Private defines */
#define DFSDM_LOW_POWER_MODE true
#define DFSDM_NORMAL_MODE false
#define DFSDM_ANALOG_WATCHDOG_MIN_THRESHOLD -8388000
/* USER CODE END Private defines */

void MX_DFSDM1_Init(bool i_initialize_low_power_mode);

/* USER CODE BEGIN Prototypes */
void DFSDM_set_analog_watchdog_thresholds(
    DFSDM_Filter_AwdParamTypeDef* io_dfsdm_analog_watchdog_parameters,
    int32_t i_high_threshold, int32_t i_low_threshold);

void DFSDM_activate_analog_watchdog(int32_t i_high_threshold,
                                    int32_t i_low_threshold);
void DFSDM_deactivate_analog_watchdog(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ dfsdm_H */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
