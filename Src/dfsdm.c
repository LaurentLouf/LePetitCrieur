/**
 ******************************************************************************
 * File Name          : DFSDM.c
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

/* Includes ------------------------------------------------------------------*/
#include "dfsdm.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
DFSDM_Channel_HandleTypeDef hdfsdm1_channel1;
DMA_HandleTypeDef hdma_dfsdm1_flt0;

static bool HAL_RCC_DFSDM1_CLK_ENABLED = false;
static bool DFSDM1_Initialized = false;

/**
 * \brief Initialize the DFSDM (Digital Filter Sigma-Delta Modulator)
 * \param[in] i_initialize_low_power_mode Initialize the module in low power
 * mode
 *
 * The MEMS microphone uses the clock provided by the DFSDM. It can uses clock
 * frequencies from 351kHz to 4.8MHz. So we enable the output clock and use a
 * prescaer value of 4 to get a frequency of
 * 11.294MHz (clock used by DFSDM) / 4 = 2.8235MHz, right in the range. Using 64
 * as oversampling value gives a final sampling frequency of 44.117kHz.
 *
 * In low power mode, when we will just use the analog watchdog, we use a lower
 * frequency to save power (the microphone consumes less at lower operating
 * frequencies). To do that, we use a greater prescaler value for the output
 * clock, 16, resulting in an output clock around 700kHz (low power mode for the
 * microphone is at operating frequencies between 351 and 815kHz).
 *
 * For the filter, a 3rd order filter and an oversampling of 64 samples gives an
 * output resolution of 1+3*log2(64) = 19 bits. For use of the data, see the
 * reference manual for DFSDM_FLTxJDATAR registers : only the 24MSB make the
 * sample data, the first 8 LSB only contain the DFSDM channel.
 *
 * The filter for the analog watchdog (awd) is also initialized but the analog
 * watchdog not started.
 */
void MX_DFSDM1_Init(bool i_initialize_low_power_mode) {
  if (HAL_DFSDM_ChannelGetState(&hdfsdm1_channel1) !=
      HAL_DFSDM_CHANNEL_STATE_RESET) {
    HAL_DFSDM_ChannelDeInit(&hdfsdm1_channel1);
    HAL_DFSDM_FilterDeInit(&hdfsdm1_filter0);
  }

  // The filter initialisation stays the same in low power and normal mode
  hdfsdm1_filter0.Instance = DFSDM1_Filter0;
  hdfsdm1_filter0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
  hdfsdm1_filter0.Init.RegularParam.FastMode = ENABLE;
  hdfsdm1_filter0.Init.RegularParam.DmaMode = ENABLE;
  hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
  hdfsdm1_filter0.Init.FilterParam.Oversampling = 64;
  hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;
  if (HAL_DFSDM_FilterInit(&hdfsdm1_filter0) != HAL_OK) {
    Error_Handler();
  }

  hdfsdm1_channel1.Instance = DFSDM1_Channel1;
  hdfsdm1_channel1.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hdfsdm1_channel1.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
  hdfsdm1_channel1.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  hdfsdm1_channel1.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
  hdfsdm1_channel1.Init.SerialInterface.SpiClock =
      DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hdfsdm1_channel1.Init.Awd.FilterOrder = DFSDM_CHANNEL_SINC3_ORDER;
  hdfsdm1_channel1.Init.Awd.Oversampling = 32;
  hdfsdm1_channel1.Init.Offset = 0;
  hdfsdm1_channel1.Init.RightBitShift = 0x00;
  hdfsdm1_channel1.Init.OutputClock.Activation = ENABLE;
  hdfsdm1_channel1.Init.OutputClock.Selection =
      DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  if (i_initialize_low_power_mode == true) {
    hdfsdm1_channel1.Init.OutputClock.Divider = 16;
  } else {
    hdfsdm1_channel1.Init.OutputClock.Divider = 4;
  }

  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter0, DFSDM_CHANNEL_1,
                                       DFSDM_CONTINUOUS_CONV_ON) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef* dfsdm_filterHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (DFSDM1_Initialized == false) {
    /* USER CODE BEGIN DFSDM1_MspInit 0 */

    /* USER CODE END DFSDM1_MspInit 0 */
    /** Initializes the peripherals clock
     */
    PeriphClkInit.PeriphClockSelection =
        RCC_PERIPHCLK_DFSDM1AUDIO | RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_DFSDM1;
    PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_SYSCLK;
    PeriphClkInit.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSAI1SOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 48;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
      Error_Handler();
    }

    /* DFSDM1 clock enable */
    if (HAL_RCC_DFSDM1_CLK_ENABLED == false) {
      __HAL_RCC_DFSDM1_CLK_ENABLE();
      HAL_RCC_DFSDM1_CLK_ENABLED = true;
    }

    __HAL_RCC_GPIOG_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**DFSDM1 GPIO Configuration
    PG7     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN DFSDM1_MspInit 1 */

    /* USER CODE END DFSDM1_MspInit 1 */
    DFSDM1_Initialized = true;
  }

  /* DFSDM1 DMA Init */
  /* DFSDM1_FLT0 Init */
  if (dfsdm_filterHandle->Instance == DFSDM1_Filter0) {
    hdma_dfsdm1_flt0.Instance = DMA1_Channel4;
    hdma_dfsdm1_flt0.Init.Request = DMA_REQUEST_DFSDM1_FLT0;
    hdma_dfsdm1_flt0.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dfsdm1_flt0.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dfsdm1_flt0.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dfsdm1_flt0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dfsdm1_flt0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dfsdm1_flt0.Init.Mode = DMA_CIRCULAR;
    hdma_dfsdm1_flt0.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_dfsdm1_flt0) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_DMA_ConfigChannelAttributes(&hdma_dfsdm1_flt0, DMA_CHANNEL_NPRIV) !=
        HAL_OK) {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested
     DMAs.
   */
    __HAL_LINKDMA(dfsdm_filterHandle, hdmaInj, hdma_dfsdm1_flt0);
    __HAL_LINKDMA(dfsdm_filterHandle, hdmaReg, hdma_dfsdm1_flt0);
  }
}

/**
 * \brief
 *
 * \param dfsdm_channelHandle
 *
 * Called by HAL_DFSDM_ChannelInit
 */
void HAL_DFSDM_ChannelMspInit(
    DFSDM_Channel_HandleTypeDef* dfsdm_channelHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (DFSDM1_Initialized == false) {
    /* USER CODE BEGIN DFSDM1_MspInit 0 */

    /* USER CODE END DFSDM1_MspInit 0 */
    /** Initializes the peripherals clock
     */
    PeriphClkInit.PeriphClockSelection =
        RCC_PERIPHCLK_DFSDM1AUDIO | RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_DFSDM1;
    PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_SYSCLK;
    PeriphClkInit.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSAI1SOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 48;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
      Error_Handler();
    }

    /* DFSDM1 clock enable */
    if (HAL_RCC_DFSDM1_CLK_ENABLED == false) {
      __HAL_RCC_DFSDM1_CLK_ENABLE();
      HAL_RCC_DFSDM1_CLK_ENABLED = true;
    }

    __HAL_RCC_GPIOG_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**DFSDM1 GPIO Configuration
    PG7     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN DFSDM1_MspInit 1 */

    /* USER CODE END DFSDM1_MspInit 1 */
    DFSDM1_Initialized = true;
  }
}

void HAL_DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef* dfsdm_filterHandle) {
  if (DFSDM1_Initialized == true) {
    /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

    /* USER CODE END DFSDM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DFSDM1_CLK_DISABLE();
    HAL_RCC_DFSDM1_CLK_ENABLED = false;

    /**DFSDM1 GPIO Configuration
    PG7     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    /* USER CODE BEGIN DFSDM1_MspDeInit 1 */
    DFSDM1_Initialized = false;
    /* USER CODE END DFSDM1_MspDeInit 1 */
  }

  /* DFSDM1 DMA DeInit */
  HAL_DMA_DeInit(dfsdm_filterHandle->hdmaInj);
  HAL_DMA_DeInit(dfsdm_filterHandle->hdmaReg);
}

void HAL_DFSDM_ChannelMspDeInit(
    DFSDM_Channel_HandleTypeDef* dfsdm_channelHandle) {
  if (DFSDM1_Initialized == false) {
    /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

    /* USER CODE END DFSDM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DFSDM1_CLK_DISABLE();
    HAL_RCC_DFSDM1_CLK_ENABLED = false;

    /**DFSDM1 GPIO Configuration
    PG7     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    /* USER CODE BEGIN DFSDM1_MspDeInit 1 */
    DFSDM1_Initialized = false;
    /* USER CODE END DFSDM1_MspDeInit 1 */
  }

  /* DFSDM1 interrupt Deinit */
  HAL_NVIC_DisableIRQ(DFSDM1_FLT0_IRQn);
}

/* USER CODE BEGIN 1 */
/**
 * \brief Set the analog watchdog thresholds
 *
 * \param[inout] io_dfsdm_analog_watchdog_parameters Structure containing the
 * parameters of the analog watchdog
 * \param[in] i_high_threshold Value for the high threshold
 * \param[in] i_low_threshold Value for the low threshold
 *
 * Set the analog watchdog thresholds using two different way to set the values,
 * depending on where the data is coming from. If the data come the DFSDM
 * filter, the thresholds are considered on 24 bits. But if the data comes from
 * the analog watchdog own filter, they are considered to be on 16 bits, but
 * coded on 24 bits where the 16MSB are considered and the 8 LSB discarded. So
 * perform this shifting here.
 *
 */
void DFSDM_set_analog_watchdog_thresholds(
    DFSDM_Filter_AwdParamTypeDef* io_dfsdm_analog_watchdog_parameters,
    int32_t i_high_threshold, int32_t i_low_threshold) {
  if (io_dfsdm_analog_watchdog_parameters->DataSource ==
      DFSDM_FILTER_AWD_CHANNEL_DATA) {
    if (i_high_threshold > __INT16_MAX__) {
      io_dfsdm_analog_watchdog_parameters->HighThreshold = __INT16_MAX__ * 256;
    } else {
      io_dfsdm_analog_watchdog_parameters->HighThreshold =
          i_high_threshold * 256;
    }

    if (i_low_threshold < (-__INT16_MAX__ - 1)) {
      io_dfsdm_analog_watchdog_parameters->LowThreshold =
          (-__INT16_MAX__ - 1) * 256;
    } else {
      io_dfsdm_analog_watchdog_parameters->LowThreshold = i_low_threshold * 256;
    }
  } else {
    io_dfsdm_analog_watchdog_parameters->HighThreshold = i_high_threshold;
    io_dfsdm_analog_watchdog_parameters->LowThreshold = i_low_threshold;
  }
}

void DFSDM_activate_analog_watchdog(int32_t i_high_threshold,
                                    int32_t i_low_threshold) {
  DFSDM_Filter_AwdParamTypeDef hdsfdm1_awd;
  hdsfdm1_awd.DataSource = DFSDM_FILTER_AWD_CHANNEL_DATA;
  hdsfdm1_awd.Channel = DFSDM_CHANNEL_1;
  hdsfdm1_awd.HighBreakSignal = DFSDM_NO_BREAK_SIGNAL;
  hdsfdm1_awd.LowBreakSignal = DFSDM_NO_BREAK_SIGNAL;
  DFSDM_set_analog_watchdog_thresholds(&hdsfdm1_awd, i_high_threshold,
                                       i_low_threshold);

  if (HAL_DFSDM_FilterAwdStart_IT(&hdfsdm1_filter0, &hdsfdm1_awd) != HAL_OK) {
    Error_Handler();
  }

  /* DFSDM1 interrupt Init */
  HAL_NVIC_SetPriority(DFSDM1_FLT0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DFSDM1_FLT0_IRQn);
}

void DFSDM_deactivate_analog_watchdog(void) {
  HAL_DFSDM_FilterAwdStop_IT(&hdfsdm1_filter0);
  HAL_NVIC_DisableIRQ(DFSDM1_FLT0_IRQn);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF
 * FILE****/
