/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "APP_SCORE.h"
#include "LCD_ST7735.h"   //ST7735 basic library
#include "KEY_BUZZ_BATT.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "FONT_number_lg.h"
#include "FONT_number_xg.h"
#include "FONT_number_ss.h"
#include "FONT_alpha_lg.h"
#include "APP_FUNCTION.h"
#include <stdlib.h>
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

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

unsigned int SYS_FLAGS=0;     // 16 bit flags used to alarm timers overflow and user key


unsigned int SOFT_FLAGS=0;    // 16 bit general use flags

unsigned int TIMER_DICE; // soft timer used for DICE APP (1ms~1.09minutes)
unsigned long int TIMER_6;  // soft timer used for general use (1ms~50days..just in case :)
unsigned long int TIMER_5;  // soft timer used for general use (1ms~50days)
unsigned int TIMER_4; // soft timer used for general use (1ms~1.09minutes)
unsigned int TIMER_3; // soft timer used for general use (1ms~1.09minutes)
unsigned int TIMER_1;
unsigned int TIMER_2;
unsigned char TIMER_KEY_P1; // soft timer used for player 1 key debounce (1ms~255ms)
unsigned char TIMER_KEY_P2; // soft timer used for player 2 key debounce (1ms~255ms)
unsigned int TIMER_REPEAT_1; // XXXY timer to start repeat for P1;
unsigned int TIMER_REPEAT_2; // XXXY timer to start repeat for P2;
unsigned char TIMER_SPEED_REPT1; // XXXY speed of repeat por P1
unsigned char TIMER_SPEED_REPT2; // XXXY speed of repeat por P2
unsigned char TIMER_KEY_M;  // soft timer used for menu key debounce
unsigned long int TIMER_BATT; // soft timer used for BATT check (1ms~50days)
unsigned long int CHRONO_1;  // chronograph 1 (50ms~50days)
unsigned long int SCHRONO_1; // auxiliar for chrono. only step values multiple of CHRONO_x_ALARM
unsigned long int CHRONO_1_ALARM; //Chrono_1 will set alarm every CHRONO_1_ALARM (0=off)
unsigned long int CHRONO_2_ALARM; //Chrono_2 will set alarm every CHRONO_2_ALARM (0=off)
unsigned long int CHRONO_2;  // chronograph 2 (50ms~50days)
unsigned long int SCHRONO_2; // auxiliar for chrono. only step values multiple of CHRONO_x_ALARM
unsigned long int TIMER_TIC = 0;  // soft system running time. always on (50ms~50days in 1msec steps)
uint32_t aaa1 = 0x00U; //lixo
uint32_t aaa2 = 0x00U; //lixo
uint32_t aaa3 = 0x00U; //lixo
uint32_t aaa4 = 0x00U; //lixo
unsigned char LIXO = 1; //lixo
unsigned char LIXO1 = 1; //lixo
unsigned char LIXO2 = 1; //lixo
unsigned short V_BATT;


const char MHOME1[6][11]={"HOME MENU", "TIMER","CHRONO","SCOREBOARD", "CHESSCLOCK","     MORE>"}; //itens for mode APP menu
const char MHOME2[6][11]={"HOME MENU", "HOURGLASS", "B I N G O","ROULETTE","","     MORE>"}; //itens for mode APP menu

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void DISPLAY_BATT(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void SystemClock_Config(void);
static void MX_SPI2_Init(void);
void LCD_PLOT(unsigned char X, unsigned char Y, unsigned int color);
void LCD_COLORWINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2, unsigned int color);
void LCD_LINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
void ARM_KEY_PLAYER1();
void DESARM_KEY_PLAYER1();
void ARM_KEY_PLAYER2();
void DESARM_KEY_PLAYER2();
void ARM_KEY_MENU();
void DESARM_KEY_MENU();
void LCD_NUMBER (int n, unsigned char x, unsigned char y, unsigned char field);
void GO_SLEEP(void);
void LCD_WORD (char w[], unsigned char x, unsigned char y, unsigned char field);
char ANY_KEY( unsigned int group);
char GET_KEY ( unsigned int group);
void BUZZER (int pulse);
void CHECK_BATT(void);
void START_CHECK_BATT(void);
void SCORE(void);
void CHRONOGRAPH (void);
void RTIMER (void);
void CHESS (void);
void HOURGLASS (void);
void BINGO (void);
void ROULETTE (void);
char SELECTMENU (const char ITEMS[6][11],unsigned int frg, unsigned int bkg);
void TOILET (void);
void WAIT_NO_KEY (void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  LCD_INIT();
  SOFT_FLAGS |= BIT_BUZZER_ON;  // turn speaker on
  HAL_TIM_Base_Start_IT(&htim4); //Start the timer
  LCD_COLORWINDOW (0, 0, 159, 127, BLACK);
  HAL_TIM_OnePulse_Start(&htim3, TIM_CHANNEL_1);
  START_CHECK_BATT();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  do {
	  	switch (SELECTMENU(MHOME1,WHITE,BLUE0))
	  	{
	  		case 1:
	  			RTIMER();

	  			break;
	  		case 2:
	  			CHRONOGRAPH();
	  			break;
	  		case 3:
	  			SCORE();
	  			break;
	  		case 4:
	  			CHESS();
	  			break;

	  	}
	  	switch (SELECTMENU(MHOME2,WHITE,BLUE0))
	  	{
	  		case 1:
	  			HOURGLASS();
	  			break;
	  		case 2:
	  			BINGO();
	  			break;
	  		case 3:
	  			ROULETTE();
	  			break;

	  	}

	  } while (1);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7200;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 150;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim3, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 36002;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, KEY_1_Pin|KEY_2_Pin|KEY_3_Pin|KEY_4_Pin 
                          |KEY_5_Pin|KEY_6_Pin|KEY_7_Pin|KEY_8_Pin 
                          |LCD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DC_A0_GPIO_Port, LCD_DC_A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_1_Pin KEY_2_Pin KEY_3_Pin KEY_4_Pin 
                           KEY_5_Pin KEY_6_Pin KEY_7_Pin KEY_8_Pin */
  GPIO_InitStruct.Pin = KEY_1_Pin|KEY_2_Pin|KEY_3_Pin|KEY_4_Pin 
                          |KEY_5_Pin|KEY_6_Pin|KEY_7_Pin|KEY_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DC_A0_Pin */
  GPIO_InitStruct.Pin = LCD_DC_A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_DC_A0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_KEY_MENU_Pin INT_KEY_PLAYER1_Pin INT_KEY_PLAYER2_Pin */
  GPIO_InitStruct.Pin = INT_KEY_MENU_Pin|INT_KEY_PLAYER1_Pin|INT_KEY_PLAYER2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void TOILET (void)
//clear all timers alarms and flags
{
	WAIT_NO_KEY (); //wait no key pressed
	SYS_FLAGS &= BIT_BATT; // clear all sys flags but battery
	SOFT_FLAGS &= (BIT_BUZZER_ON | BIT_BATT_BLINK);  // clear all soft  flags but buzzer and batt
	TIMER_1=0;  //clear all timers and alarms
	TIMER_2=0;
	TIMER_3=0;
	TIMER_4=0;
	TIMER_5=0;
	TIMER_6=0;

	CHRONO_1=0;
	SCHRONO_1=0;
	CHRONO_2=0;
	SCHRONO_2=0;
	CHRONO_1_ALARM = 0;
	CHRONO_2_ALARM = 0;
}

void GO_SLEEP(void) {		// if no pending flags, then go sleep and save batt

	do {

	//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  //turn LED off indicating Im sleeping
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_SuspendTick();   // turn systick off before sleep
		HAL_PWR_EnterSLEEPMode( 0, PWR_SLEEPENTRY_WFE);   // go sleep
		HAL_ResumeTick();   //resume systick
	//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  //tunr LED on indicating Im awake
		//	if (!TIMER_BATT) {
	//		TIMER_BATT= BATT_CYCLE;
	//		// SYS_FLAGS ^= BIT_BATT;
	//		CHECK_BATT();
	//	}
	} while (!SYS_FLAGS);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  //tunr LED on indicating Im awake


}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
 // This callback is automatically called by the HAL on the UEV event
 if(htim->Instance == TIM4) {

 if (TIMER_KEY_P1)            //debounce timer
 {
  if (!--TIMER_KEY_P1) {       //if debouce time ends
    SYS_FLAGS |= BIT_KEY_P1;  //set key flag
    // if (ANY_KEY(BIT_KEY_P1 )) BUZZER (KEYTONE);  //buzzer key feedback
    //The_Key = GET_KEY (BIT_KEY_P1);	// The_Key = the key (1~8) that waked up cpu
    //The_Group=BIT_KEY_P1;			// and The_Group = P1, P2 or M
    if (GET_KEY (BIT_KEY_P1))
    	{
    	BUZZER (KEYTONE);
    	 if (SOFT_FLAGS & BIT_KREPEAT) TIMER_REPEAT_1 = TIMEtoREPEAT;     //xxxy
    	}

 }
 }

 if (TIMER_REPEAT_1)     //yxxx if player key is held pressed then repeat
 {
	 if (!--TIMER_REPEAT_1)
	 {
		 if (GET_KEY (BIT_KEY_P1)) TIMER_SPEED_REPT1=TIMEREPEATSPEED; //start repeat key
	 }
 }

 if (TIMER_SPEED_REPT1)  //repeating key press while hold    xxxy
 {
	 if (!--TIMER_SPEED_REPT1)  //time to repeat
	 {
		 if (GET_KEY (BIT_KEY_P1)) // and key still  pressed
		 {
			 TIMER_SPEED_REPT1=TIMEREPEATSPEED; //prepare to repeat again
			 SYS_FLAGS |= BIT_KEY_P1;  //set key flag
		 }
	 }

 }

 if (TIMER_KEY_P2)            //debounce timer
 {
  if (!--TIMER_KEY_P2) {       //if debouce time ends
    SYS_FLAGS |= BIT_KEY_P2;  //set key flag
       if (GET_KEY (BIT_KEY_P2))
    	{
    	BUZZER (KEYTONE);
    	if (SOFT_FLAGS & BIT_KREPEAT) TIMER_REPEAT_2 = TIMEtoREPEAT;     //xxxy
    	}
 }
 }

 if (TIMER_REPEAT_2)     //yxxx if player key is held pressed then repeat
 {
	 if (!--TIMER_REPEAT_2)
	 {
		 if (GET_KEY (BIT_KEY_P2)) TIMER_SPEED_REPT2=TIMEREPEATSPEED; //start repeat key
	 }
 }

 if (TIMER_SPEED_REPT2)  //repeating key press while hold    xxxy
 {
	 if (!--TIMER_SPEED_REPT2)  //time to repeat
	 {
		 if (GET_KEY (BIT_KEY_P2)) // and key still  pressed
		 {
			 TIMER_SPEED_REPT2=TIMEREPEATSPEED; //prepare to repeat again
			 SYS_FLAGS |= BIT_KEY_P2;  //set key flag
		 }
	 }

 }




  if (TIMER_KEY_M)           //debounce timer
  if (!--TIMER_KEY_M) {       //if debouce time ends
    SYS_FLAGS |= BIT_KEY_M;  //set key flag
    //The_Key = GET_KEY (BIT_KEY_M);
    //The_Group=BIT_KEY_M;
    if (GET_KEY (BIT_KEY_M)) BUZZER (KEYTONE);
 }

  if (TIMER_DICE)                 //if timer running
  if (!--TIMER_DICE) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_DICE;  //set timer flag
  }

 if (TIMER_1)                 //if timer running
  if (!--TIMER_1) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_1;  //set timer flag
  }

 if (TIMER_2)                 //if timer running
  if (!--TIMER_2) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_2;  //set timer flag
  }

   if (TIMER_3)                 //if timer running
  if (!--TIMER_3) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_3;  //set timer flag
  }

   if (TIMER_4)                 //if timer running
  if (!--TIMER_4) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_4;  //set timer flag
  }

   if (TIMER_5)                 //if timer running
  if (!--TIMER_5) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_5;  //set timer flag
  }

   if (TIMER_6)                 //if timer running
  if (!--TIMER_6) {            //and just ends
    SYS_FLAGS |= BIT_TIMER_6;  //set timer flag
  }

   if (TIMER_BATT)                 //if timer running
  if (!--TIMER_BATT) {            //and just ends
  //  SYS_FLAGS |= BIT_BATT;  //set timer flag
    TIMER_BATT = BATT_CYCLE;
    CHECK_BATT();
  }



   if (SOFT_FLAGS & BIT_CHRONO_1_RUN) // chronograph, runs if enabled
   {
	   ++CHRONO_1;
	   if (CHRONO_1_ALARM) // if chrono 1 is multiple of chrono 1 alarm, then set flag
	   {
		   if (!(CHRONO_1 % CHRONO_1_ALARM))
			   {
			   SCHRONO_1=CHRONO_1;
			   SYS_FLAGS |= BIT_CHRONO_1_ALARM;
			   }

	   }
   }

if (SOFT_FLAGS & BIT_CHRONO_2_RUN) // chronograph, runs if enabled
{
	   ++CHRONO_2;
	   if (CHRONO_2_ALARM) // if chrono 2 is multiple of chrono 2 alarm, then set flag
	   {
		   if (!(CHRONO_2 % CHRONO_2_ALARM))
			   {
			   SCHRONO_2=CHRONO_2;
			   SYS_FLAGS |= BIT_CHRONO_2_ALARM;
			   }
	   }
}

  ++ TIMER_TIC; // system timer always running
rand(); //just to improve rand function

}

 }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // ISR (interrupt) routine after key press/release


  if(GPIO_Pin == INT_KEY_PLAYER1_Pin)
  //  if (!TIMER_KEY_P1){
 //   DESARM_KEY_PLAYER1();  //disable further interrupt for while (debouncing)  xxx
    TIMER_KEY_P1=DEBOUNCE_TICS;    // debounce mask time
 // }

  if(GPIO_Pin == INT_KEY_PLAYER2_Pin)
    TIMER_KEY_P2=DEBOUNCE_TICS;    // debounce mask time


  if(GPIO_Pin == INT_KEY_MENU_Pin)
	   TIMER_KEY_M=DEBOUNCE_TICS;     // debounce mask time

}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
