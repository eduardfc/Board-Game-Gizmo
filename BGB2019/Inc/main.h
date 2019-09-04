/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_1_Pin GPIO_PIN_0
#define KEY_1_GPIO_Port GPIOA
#define KEY_2_Pin GPIO_PIN_1
#define KEY_2_GPIO_Port GPIOA
#define KEY_3_Pin GPIO_PIN_2
#define KEY_3_GPIO_Port GPIOA
#define KEY_4_Pin GPIO_PIN_3
#define KEY_4_GPIO_Port GPIOA
#define KEY_5_Pin GPIO_PIN_4
#define KEY_5_GPIO_Port GPIOA
#define KEY_6_Pin GPIO_PIN_5
#define KEY_6_GPIO_Port GPIOA
#define KEY_7_Pin GPIO_PIN_6
#define KEY_7_GPIO_Port GPIOA
#define KEY_8_Pin GPIO_PIN_7
#define KEY_8_GPIO_Port GPIOA
#define Battery_Pin GPIO_PIN_1
#define Battery_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_CLK_Pin GPIO_PIN_13
#define LCD_CLK_GPIO_Port GPIOB
#define LCD_DC_A0_Pin GPIO_PIN_14
#define LCD_DC_A0_GPIO_Port GPIOB
#define LCD_SDA_Pin GPIO_PIN_15
#define LCD_SDA_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOA
#define INT_KEY_MENU_Pin GPIO_PIN_10
#define INT_KEY_MENU_GPIO_Port GPIOA
#define INT_KEY_MENU_EXTI_IRQn EXTI15_10_IRQn
#define INT_KEY_PLAYER1_Pin GPIO_PIN_11
#define INT_KEY_PLAYER1_GPIO_Port GPIOA
#define INT_KEY_PLAYER1_EXTI_IRQn EXTI15_10_IRQn
#define INT_KEY_PLAYER2_Pin GPIO_PIN_12
#define INT_KEY_PLAYER2_GPIO_Port GPIOA
#define INT_KEY_PLAYER2_EXTI_IRQn EXTI15_10_IRQn
#define BUZZER_Pin GPIO_PIN_4
#define BUZZER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */


#define BIT_KEY_P1 0X8000     // player 1 key change state SYS_F flag mask
#define BIT_KEY_P2 0X4000     // player 2 key change state SYS_F flag mask
#define BIT_KEY_M 0X2000      // menu key change state SYS_F flag mask
#define BIT_TIMER_DICE 0X1000   // DICE APP timer alarm SYS_F flag mask
#define BIT_TIMER_6 0X0800    //  timer 6 alarm SYS_F flag mask
#define BIT_TIMER_5 0X0400    //  timer 5 alarm SYS_F flag mask
#define BIT_TIMER_4 0X0200    //  timer 4 alarm SYS_F flag mask
#define BIT_TIMER_3 0X0100    //  timer 3 alarm SYS_F flag mask
#define BIT_TIMER_2 0X0080    //  timer 2 alarm SYS_F flag mask
#define BIT_TIMER_1 0X0040    //  timer 1 alarm SYS_F flag mask
#define BIT_CHRONO_1_ALARM 0x0020	//Cyclic alarm for Chrono 1 set
#define BIT_CHRONO_2_ALARM 0x0010 	//Cyclic alarm for Chrono 2 set
#define BIT_BATT 0X0001     //  BATTERY timer SYS_F flag mask

#define BIT_DICE 0X8000     // indicates that DICE app is running
#define BIT_REDRAW 0X4000     // app must redraw screen
#define BIT_CHRONO_2_RUN 0X2000 // run CHRONO 2
#define BIT_CHRONO_1_RUN 0X1000 // run CHRONO 1
#define BIT_BUZZER_ON 0X0800     // sound / mute
#define BIT_BATT_BLINK 0X0400			// used in batt routine
#define BIT_RETURN 0X0200		//exit from app and return to MENU
#define BIT_PRESET 0X0100	// reload a preset value
#define BIT_RESET 0X0080	// reset all values
#define BIT_SET 0X0040		//enter setting mode
#define BIT_KREPEAT 0X0020  // when set, repeat key if key keep pressed
#define BIT_AUX 0X0010	//auxiliar flag for app general use
#define BIT_AUX1 0X0004	//auxiliar flag for app general use
#define BIT_AUX2 0X0008	//auxiliar flag 2 for app general use

#define DEBOUNCE_TICS 11   //defines time of debounce mask



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
