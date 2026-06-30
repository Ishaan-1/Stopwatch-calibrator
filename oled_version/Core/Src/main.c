/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
#include <string.h>
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
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
typedef enum { SW_READY, SW_RUNNING, SW_PAUSED } SwState;

SwState  swState   = SW_READY;
uint32_t startMs   = 0;
uint32_t elapsedMs = 0;

#define MAX_LAPS 10
uint32_t laps[MAX_LAPS];
uint8_t  lapCount = 0;

uint8_t rxByte = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uartSend(const char* str) {
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void printTime(uint32_t ms) {
	uint32_t millis = ms % 1000;
    uint32_t secs   = (ms / 1000)    % 60;
    uint32_t mins   = (ms / 60000)   % 60;
    uint32_t hours  = (ms / 3600000);
    char buf[32];
    snprintf(buf, sizeof(buf), "%0lu:%02lu:%02lu.%03lu\r\n",
             (unsigned long)hours,
             (unsigned long)mins,
             (unsigned long)secs,
             (unsigned long)millis);
    uartSend(buf);
}

void drawWatch(uint32_t totalMs) {
	uint32_t millis = totalMs % 1000;
    uint32_t secs   = (totalMs / 1000)    % 60;
    uint32_t mins   = (totalMs / 60000)   % 60;
    uint32_t hours  = (totalMs / 3600000);

    char timeBuf[20];
    snprintf(timeBuf, sizeof(timeBuf), "%0lu:%02lu:%02lu.%03lu",
             (unsigned long)hours,
             (unsigned long)mins,
             (unsigned long)secs,
             (unsigned long)millis);

    ssd1306_Fill(Black);

    ssd1306_SetCursor(0, 8);
    ssd1306_WriteString(timeBuf, Font_11x18, White);

    ssd1306_SetCursor(0, 52);
    if (lapCount > 0) {
        uint32_t lt  = laps[lapCount - 1];
        uint32_t lcs = (lt / 10)      % 100;
        uint32_t lss = (lt / 1000)    % 60;
        uint32_t lmm = (lt / 60000)   % 60;
        uint32_t lhh = (lt / 3600000);
        char lapBuf[28];
        snprintf(lapBuf, sizeof(lapBuf), "L%u %02lu:%02lu:%02lu.%02lu",
                 lapCount,
                 (unsigned long)lhh,
                 (unsigned long)lmm,
                 (unsigned long)lss,
                 (unsigned long)lcs);
        ssd1306_WriteString(lapBuf, Font_7x10, White);
    } else {
        char* status =
            (swState == SW_RUNNING) ? "RUNNING" :
            (swState == SW_PAUSED)  ? "PAUSED"  : "READY";
        ssd1306_WriteString(status, Font_7x10, White);
    }

    ssd1306_UpdateScreen();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  drawWatch(0);

  uartSend("Stopwatch ready. Commands: s=start/stop  r=reset  l=lap\r\n");

  HAL_UART_Receive_IT(&huart1, &rxByte, 1);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (swState == SW_RUNNING) {
        uint32_t total = elapsedMs + (HAL_GetTick() - startMs);
        drawWatch(total);
    }
    HAL_Delay(16);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  */
static void MX_SPI1_Init(void)
{
  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
  * @brief USART1 Initialization Function
  */
static void MX_USART1_UART_Init(void)
{
  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, RES_pin_Pin|DC_pin_Pin|CS_pin_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = RES_pin_Pin|DC_pin_Pin|CS_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance != USART1) return;

    char cmd = (char)rxByte;

    if (cmd == 's' || cmd == 'S') {
        if (swState == SW_RUNNING) {
            elapsedMs += HAL_GetTick() - startMs;
            swState = SW_PAUSED;
            uartSend("STOPPED at ");
            printTime(elapsedMs);
            drawWatch(elapsedMs);
        } else {
            startMs = HAL_GetTick();
            swState = SW_RUNNING;
            uartSend("STARTED\r\n");
        }
    }
    else if (cmd == 'r' || cmd == 'R') {
        swState   = SW_READY;
        elapsedMs = 0;
        startMs   = 0;
        lapCount  = 0;
        drawWatch(0);
        uartSend("RESET\r\n");
    }
    else if (cmd == 'l' || cmd == 'L') {
        if (swState == SW_RUNNING && lapCount < MAX_LAPS) {
            uint32_t lapTime = elapsedMs + (HAL_GetTick() - startMs);
            laps[lapCount++] = lapTime;
            char buf[16];
            snprintf(buf, sizeof(buf), "LAP %u: ", lapCount);
            uartSend(buf);
            printTime(lapTime);
        }
    }

    HAL_UART_Receive_IT(&huart1, &rxByte, 1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
