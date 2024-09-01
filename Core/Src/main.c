/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"

#include "lcd.h"

#include <stdint.h>

#include <stdio.h>

#include <string.h>

#include "ui.h"

#include "scope.h"

#define DEBOUNCE_DELAY_MS 200
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t values[312];
uint16_t values_preScale[312];
uint16_t old_values[312];
uint16_t DMA_values[312];
uint16_t map_vals[312];
uint8_t RX_BUFFER[1] = {
    0
};
int16_t wave_color = 0xFFFF;
uint8_t trigger_mode = 0;
uint16_t trigger_level = 1000;
uint16_t old_trigger_level = 1000;
uint32_t timer_counter = 0;
uint32_t result = 0;
uint16_t selected_option = 0;

//This variable controls which menu the GUI is currently in
uint8_t menu_val = 0;

/*
menu_val = 0: Main screen with the 5 buttons and waveform
menu_val = 1: USB Mode
menu_val = 2: Trigger options menu
menu_val = 3: Scaling options menu
menu_val = 4: Miscellaneous options menu (Grid, Rolling, Color)
menu_val = 5: Measurements and cursors menu
menu_val = 6: Wave color menu
menu_val = 7: Trigger level menu
menu_val = 8: Move cursor X menu
menu_val = 9: Move cursor Y menu
menu_val = 10: X division menu
menu_val = 11: X offset menu
menu_val = 12: Y offset menu
menu_val = 13: Y division menu
*/

int completed = 0;
int flag = 0;
int grid_flag = 0;
int draw_trigger_flag = 0;
int rolling_flag = 0;
int counter = 0;
int completed_adc = 0;
int show_cursors = 0;
int cursor_x = 50;
int cursor_y = 50;
int prev_cursor_x = 50;
int prev_cursor_y = 50;
int run_scope = 1;
int PSC = 0.0;
int ARR = 5999.0;
int x_div_option = 0;
int y_div_option = 0;
int x_shift = 50;
int y_shift = 0;
int usb_mode = 0;
int button_rotary = 0;
int rotary_divider = 24;
int clearFrame = 0;
int afe_toggle = 0;
/* USER CODE END 0 */
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */
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
    MX_SPI1_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();
    MX_I2C1_Init();
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN 2 */
    //initialize and start display, ADC, and timers
    LCD_init();
    HAL_TIM_Base_Start( & htim2);
    HAL_ADC_Start_DMA( & hadc1, (uint32_t * ) DMA_values, 312);
    HAL_TIM_OC_Start( & htim2, TIM_CHANNEL_1); //Debugging timer to see time between measurements
    //initialize to first menu
    menu_val = 0;
    selected_option = 0;
    grid_flag = 1;
    rolling_flag = 0;
    //initialize and start rotary encoder and draw initial GUI
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
    HAL_TIM_Encoder_Start_IT( & htim4, TIM_CHANNEL_ALL);
    scopeMenu();
    drawTriggerIcon(trigger_mode);
    /* USER CODE END 2 */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    //runs most of the UI elements

    //menu_val legend
    /*
    menu_val = 0: Main screen with the 5 buttons and waveform
    menu_val = 1: USB Mode
    menu_val = 2: Trigger options menu
    menu_val = 3: Scaling options menu
    menu_val = 4: Miscellaneous options menu (Grid, Rolling, Color)
    menu_val = 5: Measurements and cursors menu
    menu_val = 6: Wave color menu
    menu_val = 7: Trigger level menu
    menu_val = 8: Move cursor X menu
    menu_val = 9: Move cursor Y menu
    menu_val = 10: X division menu
    menu_val = 11: X offset menu
    menu_val = 12: Y offset menu
    menu_val = 13: Y division menu
    */

    while (1) {
        if (menu_val == 0) {
            selected_option = result % 5;
            init_scope();
            drawOscButtons(selected_option, values_preScale, cursor_x, cursor_y, afe_toggle);
            if (usb_mode == 1) {
                drawOscButtons(selected_option, values_preScale, cursor_x, cursor_y, afe_toggle);
                if (completed_adc) {
                    CDC_Transmit_FS((uint8_t * ) DMA_values, 624); // 624 = 312 * 2
                }
            }
        } else if (menu_val == 1) {
            drawMenu1();
        } else if (menu_val == 2) {
            selected_option = result % 3;
            drawMenu2(selected_option);
        } else if (menu_val == 3) {
            selected_option = result % 3;
            drawMenu3(selected_option);
        } else if (menu_val == 4) {
            selected_option = result % 3;
            drawMenu4(selected_option);
        } else if (menu_val == 5) {
            selected_option = result % 3;
            drawMenu5(selected_option);
        } else if (menu_val == 6) {
            selected_option = result % 5;
            wave_color = drawMenu6(selected_option);
        } else if (menu_val == 7) {
            rotary_divider = 5;
            old_trigger_level = selected_option;
            selected_option = (result % 159) + 31;
            drawTriggerLevel(selected_option, old_trigger_level);
            trigger_level = 4096 - (selected_option - 31) * 32;
            init_scope();
        } else if (menu_val == 8) {
            rotary_divider = 1;
            selected_option = (result % 314) + 3; //display is 3 to 316 pixels wide where graph is shown
            prev_cursor_x = cursor_x;
            cursor_x = selected_option;
            init_scope();
        } else if (menu_val == 9) {
            rotary_divider = 1;
            selected_option = (result % 159) + 31;
            prev_cursor_y = cursor_y;
            cursor_y = selected_option;
            init_scope();
        } else if (menu_val == 10) {
            selected_option = result % 11;
            x_div_option = selected_option;
            init_scope();
        } else if (menu_val == 11) {
            rotary_divider = 1;
            selected_option = (result % 314) + 3; //display is 3 to 316 pixels wide where graph is shown
            x_shift = selected_option;
            init_scope();
        } else if (menu_val == 12) {
            rotary_divider = 1;
            selected_option = result;
            y_shift = selected_option;
            init_scope();
        } else if (menu_val == 13) {
            //0.825/div, 0.50/div, 0.25/div, 0.1/div, 0.05/div
            //conv factor: 1, 1.65, 3.3, 8.25, 16.5
            selected_option = result % 5;
            y_div_option = selected_option;
            init_scope();
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
    RCC_OscInitTypeDef RCC_OscInitStruct = {
        0
    };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {
        0
    };
    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 15;
    RCC_OscInitStruct.PLL.PLLN = 144;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    if (HAL_RCC_OscConfig( & RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig( & RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}
/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {
    /* USER CODE BEGIN ADC1_Init 0 */
    /* USER CODE END ADC1_Init 0 */
    ADC_ChannelConfTypeDef sConfig = {
        0
    };
    /* USER CODE BEGIN ADC1_Init 1 */
    /* USER CODE END ADC1_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init( & hadc1) != HAL_OK) {
        Error_Handler();
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
     */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel( & hadc1, & sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */
    /* USER CODE END ADC1_Init 2 */
}
/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {
    /* USER CODE BEGIN I2C1_Init 0 */
    /* USER CODE END I2C1_Init 0 */
    /* USER CODE BEGIN I2C1_Init 1 */
    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init( & hi2c1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */
    /* USER CODE END I2C1_Init 2 */
}
/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {
    /* USER CODE BEGIN SPI1_Init 0 */
    /* USER CODE END SPI1_Init 0 */
    /* USER CODE BEGIN SPI1_Init 1 */
    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init( & hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */
    /* USER CODE END SPI1_Init 2 */
}
/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {
    /* USER CODE BEGIN TIM2_Init 0 */
    /* USER CODE END TIM2_Init 0 */
    TIM_ClockConfigTypeDef sClockSourceConfig = {
        0
    };
    TIM_MasterConfigTypeDef sMasterConfig = {
        0
    };
    TIM_OC_InitTypeDef sConfigOC = {
        0
    };
    /* USER CODE BEGIN TIM2_Init 1 */
    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 23;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init( & htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource( & htim2, & sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_OC_Init( & htim2) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization( & htim2, & sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_OC_ConfigChannel( & htim2, & sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */
    /* USER CODE END TIM2_Init 2 */
    HAL_TIM_MspPostInit( & htim2);
}
/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {
    /* USER CODE BEGIN TIM4_Init 0 */
    /* USER CODE END TIM4_Init 0 */
    TIM_Encoder_InitTypeDef sConfig = {
        0
    };
    TIM_MasterConfigTypeDef sMasterConfig = {
        0
    };
    /* USER CODE BEGIN TIM4_Init 1 */
    /* USER CODE END TIM4_Init 1 */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 21000;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0;
    sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0;
    if (HAL_TIM_Encoder_Init( & htim4, & sConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization( & htim4, & sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM4_Init 2 */
    /* USER CODE END TIM4_Init 2 */
}
/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();
    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {
        0
    };
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin | LCD_RST_Pin | LCD_CS_Pin, GPIO_PIN_SET);
    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, & GPIO_InitStruct);
    /*Configure GPIO pin : PA1 */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, & GPIO_InitStruct);
    /*Configure GPIO pins : LOGIC_CH1_Pin LOGIC_CH2_Pin */
    GPIO_InitStruct.Pin = LOGIC_CH1_Pin | LOGIC_CH2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, & GPIO_InitStruct);
    /*Configure GPIO pin : LCD_DC_Pin */
    GPIO_InitStruct.Pin = LCD_DC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_DC_GPIO_Port, & GPIO_InitStruct);
    /*Configure GPIO pins : LCD_RST_Pin LCD_CS_Pin */
    GPIO_InitStruct.Pin = LCD_RST_Pin | LCD_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, & GPIO_InitStruct);
    /*Configure GPIO pins : BTN_1_Pin BTN_2_Pin BTN_3_Pin BTN_4_Pin */
    GPIO_InitStruct.Pin = BTN_1_Pin | BTN_2_Pin | BTN_3_Pin | BTN_4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, & GPIO_InitStruct);
    /*Configure GPIO pin : ROTARY_Key_Pin */
    GPIO_InitStruct.Pin = ROTARY_Key_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ROTARY_Key_GPIO_Port, & GPIO_InitStruct);
    /*Configure GPIO pins : LOGIC_CH4_Pin LOGIC_CH3_Pin ROTARY_BTN_Pin */
    GPIO_InitStruct.Pin = LOGIC_CH4_Pin | LOGIC_CH3_Pin | ROTARY_BTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, & GPIO_InitStruct);
    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
//This function is responsible for drawing the waveform and restarting new measurements
void init_scope() {
    //Toggles drawing the grid, clearing frame, and trigger icon on the GUI
    if (grid_flag) {
        drawGrid();
    }
    if (clearFrame == 1) {
        clearMenuFrame();
        clearFrame = 0;
    } else if (clearFrame == 2) {
        clearFullFrame();
        clearFrame = 0;
    }
    if (draw_trigger_flag == 1) {
        drawTriggerIcon(trigger_mode);
        draw_trigger_flag = 0;
    }
    //Moves collected ADC values into new array, restarts ADC, then runs main oscilloscope functionality
    if (completed_adc == 1 && !rolling_flag && run_scope) {
        memmove(values, DMA_values, 312 * sizeof(uint16_t));
        memmove(values_preScale, DMA_values, 312 * sizeof(uint16_t));
        completed_adc = 0;
        startScope( & htim2, PSC, ARR);
        HAL_ADC_Start_DMA( & hadc1, (uint32_t * ) DMA_values, 312);
        mapScreen(values, y_shift, y_div_option, map_vals, afe_toggle);
        drawWaveform(values, old_values, wave_color);
        drawCursors(show_cursors, cursor_x, cursor_y, prev_cursor_x, prev_cursor_y, PSC, ARR, map_vals, values, afe_toggle);
        drawUsbMode(usb_mode);
        drawDiv(x_div_option, & PSC, & ARR, y_div_option);
        //updates waveform with previously stored values if ADC did not complete a transfer
    } else if (completed_adc == 0 && !rolling_flag && run_scope == 0) {
        drawWaveform(values, old_values, wave_color);
        drawCursors(show_cursors, cursor_x, cursor_y, prev_cursor_x, prev_cursor_y, PSC, ARR, map_vals, values, afe_toggle);
        drawUsbMode(usb_mode);
        drawDiv(x_div_option, & PSC, & ARR, y_div_option);
        //ROLLING MODE
    } else if (rolling_flag && run_scope) {
        if (counter == 312) {
            counter = 0;
        }
        UG_DrawLine(counter + 3, 31, counter + 3, 189, C_BLACK);
        LCD_DrawPixel(counter + 3, 189 - HAL_ADC_GetValue( & hadc1) / 26, wave_color);
        counter++;
    }
    //copies current waveform into old_values to be used for clearing
    memcpy(old_values, values, 312 * sizeof(uint16_t));
}
//ADC callback function, gets called once a full conversion of 312 values is measured and transferred via DMA
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc) {
    //stop ADC once one full conversion of 312 values is complete
    HAL_TIM_Base_Stop( & htim2);
    HAL_ADC_Stop_DMA( & hadc1);
    //Toggles GPIO Pin for debugging purposes, to measure time between measurements
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    //Trigger logic, checks if it is rising or falling with a 10% tolerance
    if ((DMA_values[x_shift] > trigger_level / 1.10 && DMA_values[x_shift] < trigger_level * 1.10) && (trigger_mode == 1) && (DMA_values[x_shift] > DMA_values[x_shift - 6]) && !rolling_flag) {
        completed_adc = 1;
    } else if ((DMA_values[x_shift] > trigger_level / 1.10 && DMA_values[x_shift] < trigger_level * 1.10) && (trigger_mode == 2) && (DMA_values[x_shift] < DMA_values[x_shift - 6]) && !rolling_flag) {
        completed_adc = 1;
    } else if (trigger_mode == 0) {
        completed_adc = 1;
    } else {
        startScope( & htim2, PSC, ARR);
        HAL_ADC_Start_DMA( & hadc1, (uint32_t * ) DMA_values, 312);
    }
}

//menu_val legend
/*
menu_val = 0: Main screen with the 5 buttons and waveform
menu_val = 1: USB Mode
menu_val = 2: Trigger options menu
menu_val = 3: Scaling options menu
menu_val = 4: Miscellaneous options menu (Grid, Rolling, Color)
menu_val = 5: Measurements and cursors menu
menu_val = 6: Wave color menu
menu_val = 7: Trigger level menu
menu_val = 8: Move cursor X menu
menu_val = 9: Move cursor Y menu
menu_val = 10: X division menu
menu_val = 11: X offset menu
menu_val = 12: Y offset menu
menu_val = 13: Y division menu
*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    if ((current_time - last_interrupt_time) > DEBOUNCE_DELAY_MS) {
        last_interrupt_time = current_time;
        if (GPIO_Pin == ROTARY_Key_Pin) {
            //main osc menu, go to one of the 5 buttons
            if (menu_val == 0) {
                if (selected_option == 0) {
                    usb_mode = !usb_mode;
                    menu_val = 0;
                    clearFrame = 2;
                    counter = 0;
                    startScope( & htim2, PSC, ARR);
                } else if (selected_option == 1) {
                    menu_val = 2;
                    stopScope( & htim2);
                } else if (selected_option == 2) {
                    menu_val = 3;
                    stopScope( & htim2);
                } else if (selected_option == 3) {
                    menu_val = 4;
                    stopScope( & htim2);
                } else if (selected_option == 4) {
                    menu_val = 5;
                    stopScope( & htim2);
                }
            } else if (menu_val == 2) {
                if (selected_option == 0) {
                    menu_val = 7;
                    clearFrame = 2;
                    counter = 0;
                    startScope( & htim2, PSC, ARR);
                } else if (selected_option == 1) {
                    if (trigger_mode == 2 || trigger_mode == 0) {
                        trigger_mode = 1;
                        draw_trigger_flag = 1;
                        menu_val = 0;
                        clearFrame = 2;
                        counter = 0;
                        startScope( & htim2, PSC, ARR);
                    } else {
                        trigger_mode = 0;
                        draw_trigger_flag = 1;
                        menu_val = 0;
                        clearFrame = 2;
                        counter = 0;
                        startScope( & htim2, PSC, ARR);
                    }
                } else if (selected_option == 2) {
                    if (trigger_mode == 1 || trigger_mode == 0) {
                        trigger_mode = 2;
                        draw_trigger_flag = 1;
                        menu_val = 0;
                        clearFrame = 2;
                        counter = 0;
                        startScope( & htim2, PSC, ARR);
                    } else {
                        trigger_mode = 0;
                        draw_trigger_flag = 1;
                        menu_val = 0;
                        clearFrame = 2;
                        counter = 0;
                        startScope( & htim2, PSC, ARR);
                    }
                }
            } else if (menu_val == 3) {
                if (selected_option == 1) {
                    menu_val = 10;
                    selected_option = 4;
                    clearFrame = 2;
                    counter = 0;
                    HAL_TIM_Base_Stop( & htim4);
                    __HAL_TIM_SET_COUNTER( & htim4, 0);
                    HAL_TIM_Base_Start( & htim4);
                } else if (selected_option == 0) {
                    menu_val = 11;
                    selected_option = 4;
                    clearFrame = 2;
                    counter = 0;
                } else if (selected_option == 2) {
                    menu_val = 13;
                    selected_option = 4;
                    clearFrame = 2;
                    counter = 0;
                    HAL_TIM_Base_Stop( & htim4);
                    __HAL_TIM_SET_COUNTER( & htim4, 0);
                    HAL_TIM_Base_Start( & htim4);
                }
            } else if (menu_val == 4) {
                if (selected_option == 0) {
                    grid_flag = !grid_flag;
                    menu_val = 0;
                    clearFrame = 2;
                    counter = 0;
                    startScope( & htim2, PSC, ARR);
                } else if (selected_option == 1) {
                    rolling_flag = !rolling_flag;
                    menu_val = 0;
                    clearFrame = 2;
                    counter = 0;
                    HAL_ADC_Start_DMA( & hadc1, (uint32_t * ) DMA_values, 312);
                } else if (selected_option == 2) {
                    menu_val = 6;
                }
            } else if (menu_val == 5) {
                if (selected_option == 0) {
                    menu_val = 0;
                    selected_option = 4;
                    clearFrame = 2;
                    counter = 0;
                    show_cursors = !show_cursors;
                    startScope( & htim2, PSC, ARR);
                } else if (selected_option == 1) {
                    menu_val = 8;
                    show_cursors = 1;
                    clearFrame = 2;
                    counter = 0;
                    startScope( & htim2, PSC, ARR);
                } else if (selected_option == 2) {
                    menu_val = 9;
                    show_cursors = 1;
                    clearFrame = 2;
                    counter = 0;
                    startScope( & htim2, PSC, ARR);
                }
            } else if (menu_val == 6) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 7) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 8) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 9) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 10) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 11) {
                menu_val = 12;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
                __HAL_TIM_SET_COUNTER( & htim4, 0);
            } else if (menu_val == 12) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 13) {
                menu_val = 0;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            }
            //BACK BUTTON
        } else if (GPIO_Pin == BTN_1_Pin) {
            if (menu_val == 1) {
                menu_val = 0;
                selected_option = 0;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 2) {
                menu_val = 0;
                selected_option = 1;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 3) {
                menu_val = 0;
                selected_option = 2;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 4) {
                menu_val = 0;
                selected_option = 3;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 5) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 6) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 7) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 8) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 9) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 10) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 11) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 12) {
                menu_val = 0;
                selected_option = 4;
                clearFrame = 2;
                counter = 0;
                rotary_divider = 24;
                startScope( & htim2, PSC, ARR);
            } else if (menu_val == 13) {
                menu_val = 0;
                clearFrame = 2;
                counter = 0;
                startScope( & htim2, PSC, ARR);
            }
            //RESET X BUTTON
        } else if (GPIO_Pin == BTN_3_Pin) { //resets the x/y dividers and offsets
            x_shift = 0;
            x_div_option = 0;
            y_shift = 0;
            y_div_option = 0;
            rotary_divider = 24;
            menu_val = 0;
            clearFrame = 2;
            counter = 0;
            startScope( & htim2, PSC, ARR);
            //RESET Y BUTTON
        } else if (GPIO_Pin == BTN_4_Pin) { //toggles analog front end adjustments in graph
        	afe_toggle = !afe_toggle;
            rotary_divider = 24;
            menu_val = 0;
            clearFrame = 2;
            counter = 0;
            startScope( & htim2, PSC, ARR);
        } else if (GPIO_Pin == ROTARY_Key_Pin) {
            button_rotary++;
            //RUN/STOP BUTTOn
        } else if (GPIO_Pin == BTN_2_Pin) { //pauses or resumes the scope from displaying new waveforms
            run_scope = !run_scope;
        }
    }
}
//Timer callback function for the rotary encoder to update the rotate position
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef * htim) {
    timer_counter = __HAL_TIM_GET_COUNTER(htim);
    result = timer_counter / rotary_divider;
}
/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
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
void assert_failed(uint8_t * file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
