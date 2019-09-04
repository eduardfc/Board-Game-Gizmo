#include "stm32f1xx_hal.h"
#include "main.h"
#include "KEY_BUZZ_BATT.h"
#include "stm32f1xx_hal_tim.h"
#include <stdlib.h>
#include "LCD_ST7735.h"
TIM_HandleTypeDef htim3;

extern unsigned char TIMER_KEY_P1;
extern unsigned char TIMER_KEY_P2;
extern unsigned char TIMER_KEY_M;
extern unsigned int TIMER_4;
extern unsigned int SYS_FLAGS;
extern unsigned int SOFT_FLAGS;
char ANY_KEY( unsigned int group);
unsigned int BATT_HIST[4];
extern unsigned short V_BATT;
extern unsigned long int TIMER_BATT;
static unsigned char batt_status=0;
ADC_HandleTypeDef hadc1;

char GET_KEY ( unsigned int);
void GO_SLEEP(void);
void LCD_COLORWINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2, unsigned int color);
void WAIT_A_KEY (void);


void DISPLAY_BATT(void) {  //will draw _ _ _ according to charge
	char var;
	SYS_FLAGS &= ~BIT_BATT; // clear BATT attention flag
	LCD_COLORWINDOW (120,127, 159, 127, BLACK);
	if (batt_status) {
		for ( var = 0; var < batt_status; ++var) {
			LCD_COLORWINDOW ((var*15)+120, 127, (var*15)+129, 127, WHITE);

		}
	}
	else {
		// blinking red when critical level
		SOFT_FLAGS ^= BIT_BATT_BLINK;
		if (SOFT_FLAGS & BIT_BATT_BLINK )
			LCD_COLORWINDOW (120,127, 159, 127, RED);


	}

}
void CHECK_BATT(void)
//periodically check battery
{

	 HAL_ADC_Stop_DMA(&hadc1);
	 BATT_HIST[3]=BATT_HIST[2];  // store last 4 voltage values to calculate average
	 BATT_HIST[2]=BATT_HIST[1];
	 BATT_HIST[1]=BATT_HIST[0];

	 //ADC_RAW[0]  -> ADC reading from battery
	 //ADC_RAW[1]  -> ADC reading from 1200mV internal reference (Vrefint)

	 BATT_HIST[0]=(1200*ADC_RAW[0])/ADC_RAW[1];
	 HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_RAW, 2);
	 V_BATT=(BATT_HIST[0]+BATT_HIST[1]+BATT_HIST[2])/3;  //V_BATT -> battery voltage mV

	 switch (batt_status) {
		case 0:
			SYS_FLAGS |= BIT_BATT; //blink flag as status is critical
			if (V_BATT > (V_Batt_25 + V_Batt_THR)) {
				batt_status=1;
				SYS_FLAGS |= BIT_BATT; //set timer flag as vbat changed
			}
			break;

		case 1:
			if (V_BATT > (V_Batt_50 + V_Batt_THR)) {
				batt_status=2;
				SYS_FLAGS |= BIT_BATT; //set timer flag as vbat changed
			}
			if (V_BATT < (V_Batt_50 - V_Batt_THR)) {
				batt_status=0;
				SYS_FLAGS |= BIT_BATT;
			}
			break;

		case 2:
			if (V_BATT > (V_Batt_75 + V_Batt_THR)) {
				batt_status=3;
				SYS_FLAGS |= BIT_BATT;
			}
			if (V_BATT < (V_Batt_75 - V_Batt_THR)) {
				batt_status=1;
				SYS_FLAGS |= BIT_BATT;
			}
			break;

		case 3:

			if (V_BATT < (V_Batt_100 - V_Batt_THR)) {
				batt_status=2;
				SYS_FLAGS |= BIT_BATT;
			}
			break;

	}
}
void START_CHECK_BATT(void)
//battery routine startup
{
	TIMER_BATT= BATT_CYCLE;

}


void BUZZER (int pulse)
// sound the buzzer for pulse * 0.1ms; pulse=1 ~ 65534
{
	if (SOFT_FLAGS & BIT_BUZZER_ON){
		if(!(__HAL_TIM_GET_COUNTER(&htim3))){   //check if idle
			pulse++;  // 1 ms is the delay
			TIM3->ARR=pulse; // set the auto reload register of TIM3
			HAL_TIM_Base_Start(&htim3); //Start the timer
		}
	}
}


void PAUSE (unsigned int t)  //Pause for t ms or change in key status. mess with timer 4
{
	TIMER_4=t;
	do {
		GO_SLEEP();	//wait something happens
		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

	}
	while (!( ANY_KEY(BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M)) && !(SYS_FLAGS & BIT_TIMER_4));
	SYS_FLAGS &= ~(BIT_TIMER_4) ; //reset timer flag but not key flag
}

void WAIT_A_KEY (void) // will return only after any key pressed. reset all key flags. mess with timer 4
{
	do {
		TIMER_4 =  253; //check at least every 253ms
		GO_SLEEP();	//wait something happens
		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt
		SYS_FLAGS=0;
		}
		while (! ANY_KEY(BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M));
	SYS_FLAGS &= ~(BIT_KEY_M ^ BIT_KEY_P1  ^ BIT_KEY_P2 ^ BIT_TIMER_4); // clear all key flags, tim.4
	TIMER_4=0;
}

void WAIT_NO_KEY (void) // will return only after no key pressed. reset all key flags. mess with timer 4
{
	do {
		TIMER_4 =  253; //check at least every 253ms
		GO_SLEEP();	//wait something happens
		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt
		SYS_FLAGS=0;
		}
		while (ANY_KEY(BIT_KEY_P1 + BIT_KEY_P2 +BIT_KEY_M));
	SYS_FLAGS &= ~(BIT_KEY_M ^ BIT_KEY_P1  ^ BIT_KEY_P2 ^ BIT_TIMER_4); // clear all key flags, tim.4
	TIMER_4=0;
}


short GET_KEY_VALUE (unsigned int group)  //complement of GET_KEY, this returns the function of the pressed Player key
{
	switch (GET_KEY (group)) {
		case 1:
			return (1);

		case 2:
			return (5);

		case 3:
			return (10);

		case 4:
			return (50);

		case 5:
			return (-1);

		case 6:
			return (-5);

		case 7:
			return (-10);

		case 8:
			return (-50);

		default:
			return (0);

	}
}

char GET_KEY ( unsigned int group)
//return the pressed KEY from group. group MUST be one of  BIT_KEY_P1, BIT_KEY_P2, BIT_KEY_M
{
if (!ANY_KEY(group)) return (0); // return zero if no key pressed

unsigned int mask=0;
unsigned char KEY;
switch (group) {
case BIT_KEY_P1: mask=INT_KEY_PLAYER1_Pin; 	break;
case BIT_KEY_P2: mask=INT_KEY_PLAYER2_Pin;	break;
case BIT_KEY_M: mask=INT_KEY_MENU_Pin; 		break;


}


int save_key_state = GPIOA->IDR; //read all port a at once
save_key_state &= (INT_KEY_PLAYER1_Pin|INT_KEY_PLAYER2_Pin|INT_KEY_MENU_Pin); // save_key_state has the state of each key group


HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); //will mess with key lines so disable interrupt for a while
									//so must find key fast or will block other keypress
/*
 BINARY SEARCH
 1->MASK OUT
 ?->TEST
 x->IRRELEVANT

 	 	 	 	 	 	 	 1111????
 	 	 	 	 	 Y/					\N
 	 	 	 xxxx11??					11??xxxx
 	 	 	 Y/		\N					Y/		\N
 	 xxxxxxxx1?		xxxx1?xx	xx1?xxxx		1?xxxxxx
 	 Y/		\N 		Y/		\N	Y/		\N 		Y/		\N
 key1		2		3		4	5		6		7		8

 */



GPIOA->ODR |= 0x00f0; //test bits 1,2,3,4
// BUZZER(500); //lixo xxx
STABILITY
		if (GPIOA->IDR & mask)
		{		// must be  5,6, 7 or 8
			GPIOA->ODR &= 0xffCf; //test bits 5,6
// BUZZER(500); //lixo xxx
			STABILITY

			if (GPIOA->IDR & mask)
			{	// must be 7,8
				GPIOA->ODR &= 0xff8f; //test bits 6
// BUZZER(500); //lixo xxx
				STABILITY

				if (GPIOA->IDR & mask)
					KEY=8;	// is 8
				else
					KEY=7; 	// is 7
			}
			else
			{ 	// must be 5,6
				GPIOA->ODR |= 0x0020; //test bit 5
// BUZZER(500); //lixo xxx
				STABILITY

				if (GPIOA->IDR & mask)
					KEY=6;	// is 6
				else
					KEY=5; // is 5
			}
			}
		else
		{	// must be  1, 2, 3 or 4
			GPIOA->ODR |= 0x000C; //test bits 1,2
// BUZZER(500); //lixo xxx
			STABILITY
			if (GPIOA->IDR & mask)
			{ // must be 3, 4
				GPIOA->ODR &= 0xfff8; //test bit 3
// BUZZER(500); //lixo xxx
				STABILITY

				if (GPIOA->IDR & mask)
					KEY=4; // is 4
				else
					KEY=3; //is 3
			}
			else
			{ // must be 1, 2
				GPIOA->ODR |= 0x0002; //test bit 0
// BUZZER(500); //lixo xxx
				STABILITY

				if (GPIOA->IDR & mask)
					KEY=2;	// is 2
				else
					KEY=1; // is 1
			}
		}

		GPIOA->ODR &= 0xff00;  // restore key lines
		STABILITY

__HAL_GPIO_EXTI_CLEAR_IT(INT_KEY_PLAYER1_Pin | INT_KEY_PLAYER2_Pin | INT_KEY_MENU_Pin); //clear all line interrupt
HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);		// restore interrupt


int save_key_state_new = GPIOA->IDR; //read all port a at once
save_key_state_new ^= save_key_state; // 1= has changed 0=no changed
if ((!TIMER_KEY_P1) && (save_key_state_new & INT_KEY_PLAYER1_Pin) ) TIMER_KEY_P1=DEBOUNCE_TICS; // if changed state, set flag
if ((!TIMER_KEY_P2) && (save_key_state_new & INT_KEY_PLAYER2_Pin) ) TIMER_KEY_P2=DEBOUNCE_TICS;
if ((!TIMER_KEY_M) && (save_key_state_new & INT_KEY_MENU_Pin) ) TIMER_KEY_M=DEBOUNCE_TICS;


return (KEY);

}

char ANY_KEY( unsigned int group)
/*return 1 if a key from group is pressed. group may be BIT_KEY_P1, BIT_KEY_P2, BIT_KEY_M,
or any combination using | so (BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M) check for any key of any
group*/

{
	int porta = GPIOA->IDR ^ 0xffff;  //read all bit from PORTA at once and invert
	if ((group & BIT_KEY_P1) && (porta & INT_KEY_PLAYER1_Pin)) return (1);
	if ((group & BIT_KEY_P2) && (porta & INT_KEY_PLAYER2_Pin)) return (1);
	if ((group & BIT_KEY_M) && (porta & INT_KEY_MENU_Pin)) return (1);

return (0);
}

void ARM_KEY_PLAYER1() {      // enable pin
                                //xxx rever apenas setando bits para simplificar
                  //sem usar HAL
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = INT_KEY_PLAYER1_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void DESARM_KEY_PLAYER1() {   // disable pin

HAL_GPIO_DeInit(GPIOA, INT_KEY_PLAYER1_Pin);
}


void ARM_KEY_PLAYER2() {      // enable pin
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = INT_KEY_PLAYER2_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DESARM_KEY_PLAYER2() {   // disable pin
//GPIO_InitTypeDef GPIO_InitStruct;
//GPIO_InitStruct.Pin = INT_KEY_PLAYER2_Pin;
//GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_DeInit(GPIOA, INT_KEY_PLAYER2_Pin);
}

void ARM_KEY_MENU() {      // enable pin
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = INT_KEY_MENU_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DESARM_KEY_MENU() {   // disable pin
//GPIO_InitTypeDef GPIO_InitStruct;
//GPIO_InitStruct.Pin = INT_KEY_MENU_Pin;
//GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_DeInit(GPIOA, INT_KEY_MENU_Pin);
}

