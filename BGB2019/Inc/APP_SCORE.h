
 #ifndef __APPSCORE_H
 #define __APPSCORE_H
 

typedef struct  {
	short SCORE; //updated score
	unsigned int COLOR; //this player score color
	short ADDING; //value being added
	short MEM1; // last added values
	short MEM2;
	short MEM3;
	short FIRST; //first value memory, used to preset to a know value
	short TIMERMASK; //general timer mask for each player
	unsigned int *TIMERADRESS; //and the timer address
	unsigned int KEY;  //mask for player KEY
	unsigned char STATE; //current state
	unsigned char X; // display collum to write
	char JUSTIFY; // If left or right
} score_db;

 	//states for state_general

#define IDLE 0		//states for state_Px
#define KEYPRESSED 1//a key is pressed and hold
#define KEYRELEASED 3//a key was presses and is just releases
#define YADD  3//line to write ADD
#define YSCORE 35 //line to write SCORE
#define YMEM1 84 //line to write MEM1
#define YMEM2  98 //line to write MEM2
#define YMEM3 112 //line to write MEM3

#define TIMEtoREPEAT 2000;  //ms to repeat after keypress
#define TIMEREPEATSPEED 120; //ms key repeat period   xxx colocar em main ou key
#define TIMEtoGOtoIDLE 5000; //ms to return back to IDLE state after key pressed/released

 /* Prototypes   */
void LCD_COLORWINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2, unsigned int color);
void SC_REDRAW(void);
 
 #endif
