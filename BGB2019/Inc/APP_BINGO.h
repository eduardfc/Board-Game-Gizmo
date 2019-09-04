 #ifndef __APPBINGO_H
 #define __APPBINGO_H



 	//states for BINGO

#define B_STOP 0		// just up to start
#define B_WAIT 1		// waiting user ask for next number
#define B_SHUFFLE 3		// play or user key pressed, effect of shuffling numbers
#define B_RELEASE 5		// user just released key, shuffle a little more
#define B_SHOW	7		// showing drawn number
#define B_INFO 9		// display info (last drawn tokens and number of tokens inside/outside cage

//states for ROULETTE

#define RL_STOP 0		// just up to start, draw roulette but not ball
#define RL_ROLLING1 1		// a key is pressed -> RLT-> Fast  Ball-> zero
#define RL_ROLLING2 3		// NO key is pressed -> RLT-> Fast  Ball-> Fast
#define RL_ROLLING3 5		// NO key is pressed -> RLT-> Medium  Ball-> Fast
#define RL_ROLLING4 7		// NO key is pressed -> RLT-> Slow  Ball-> Medium
#define RL_ROLLING5 9		// NO key is pressed -> RLT-> Stop  Ball-> Slow
#define RL_ROLLING6 11		// NO key is pressed -> RLT-> Stop  Ball-> STOPPING
#define RL_END 13		// NO key is pressed -> RLT-> Stop  Ball-> Stop.  Will redraw roullete and ball
#define RL_SPEED1 50	// roullete and ball speed
#define RL_SPEED2 100	// roullete and ball speed






#define US_MODE 0X0010	//same as BIT_AUX flag in SOFT_FLAGS, used to indicate US (75) mode
#define UK_MODE 0X0000	//same as BIT_AUX flag in SOFT_FLAGS, used to indicate UK (90) mode

#define USBINGO 75 //tokens in US bingo
#define UKBINGO 90 //tokens in UK bingo

#define DRAWN_CHIPS 0
#define TOTAL_CHIPS 91

#define SINGLE0 0
#define DOUBLE0 1

const char MBINGO[6][11]={"BINGO MODE", "", " US",""," UK",""}; //itens for mode selection menu
const char MROULLETE[6][11]={" MODE", "", " US ?O OO@",""," UK ?O@",""}; //itens for mode selection menu

 #endif
