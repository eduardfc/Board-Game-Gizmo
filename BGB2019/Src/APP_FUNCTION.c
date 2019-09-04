#include "main.h"
#include "LCD_ST7735.h"
#include "APP_FUNCTION.h"
#include "FONT_number_xg.h"
#include "FONT_number_lg.h"
#include "FONT_number_ss.h"
#include "FONT_alpha_lg.h"
#include "FONT_alpha_tny.h"
#include "APP_SCORE.h"
#include "APP_CHRONO.h"
#include "stm32f1xx_hal.h"


#include <stdlib.h>

extern unsigned int TIMER_DICE;
extern unsigned int TIMER_4;
extern unsigned int TIMER_3;
extern unsigned int SYS_FLAGS;
extern unsigned int SOFT_FLAGS;
extern unsigned long int TIMER_TIC;
extern unsigned long int CHRONO_1;  // chronograph 1 (50ms~50days) used to mark time
extern laps_db CHRON[6]; //lap marks

unsigned char MENU;
unsigned char state_DICE;
unsigned char state_MUTE;
unsigned char state_MENU;
unsigned char ROLL_PHASE;
const char MRESET[6][11]={"SELECT", " PRESET", " RESET"," SET"," HOME"," CANCEL"};
// xxxx const char HELPtest[]={"0 2 4 6 8 0 2 4 6 8  1 3 5 7 9 1 3 5 7 9 The /Rcentral /Gprocessing /Sunit /W(CPU) /Bis the unit which performs most of the processing inside a computer. To control instructions and data flow to and from other parts of the computer, the CPU relies heavily on a chipset, which is a group of microchips located on the motherboard/Z."};
const char *HELP;  //pointer to help text array
int cursor, aux1, aux2;

extern	score_db PLAYERS [2];

//prototypes
void LCD_DICE (unsigned char d, unsigned char x, unsigned char y,unsigned int color,unsigned int pip, int click);
void LCD_LINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
void BUZZER (int pulse);
void DICE(void);
char GET_KEY ( unsigned int group);
void DISPLAY_BATT(void);
void GO_SLEEP(void);
void WAIT_NO_KEY (void);
void MUTE(void);
void LCD_INIT(void);
void NO_SPK();
void SPK();
void SLCTM_WRITE_ITEMS(const char ITEMS[6][11],unsigned int frg, unsigned int bkg);
void RELOAD (void);
void LCD_WORD (const char w[], unsigned char x, unsigned char y, unsigned char field);
char SELECTMENU (const char ITEMS[6][11],unsigned int frg,unsigned int bkg);
void WRITER(const char *TEXT);
void PRINTIT (const char w[], unsigned char *X, unsigned char *Y);
void WAIT_A_KEY (void);
void	SC_REDRAW(void);
void CHR_REDRAW (void);
void PAUSE (unsigned int t) ;
void MDIRECT (void);




void APP_FUNCTION_MAIN()  // main entrance point after a function key pressed
{
	SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag
	switch (GET_KEY (BIT_KEY_M)) {
			case 0:
				break;
			case 1:
				break;

			case 2:
				break;

			case 3:
				DICE ();
				break;

			case 4:
	//			*HELP=HELPtest;   //xxxx remover
				WRITER(HELP);
				break;


			case 5:
				break;

			case 6:
				RELOAD ();
				break;


			case 7:
				MUTE ();
				break;

			case 8:
				LCD_INIT();
				break;

}
}

void WRITER(const char *TEXT)
//this function will print a long sequence of chars. For example, the help instructions
//using the alpha_tny as font
//screen is divided in 20 columms by 8 lines
//if overlap line, will jump to next line
//if overlap page will wait to a key press then clean screen and star from start cursor position
//there are some special commands tha can be indicated by ~ followed by a char:
// /L -> next line			/P -> next page			/Z -> beep			// -> print /
// /B -> BLACK			/R -> RED			/S -> SKY BLUE			/G -> GREEN			/W -> WOOD BROWN
{
unsigned char XCURSOR=0;  //horizontal position in display
unsigned char YCURSOR=0;	//vertical position in display
int PNTR=0; //pointer to the letter in TEXT
char cc; //current char
char ONEW[21]; //one whole word to be printed
font=alpha_tny;  // only this font for simplicity
justify=LEFT;
frground=BLACK; //printing in black as default
bkground=WHITE;
LCD_COLORWINDOW (0, 0, 159, 127, BLACK);  //clear screen
LCD_COLORWINDOW (0, 0, 159, 125, WHITE);  //white as background
WAIT_NO_KEY();
cc=TEXT[PNTR]; //get first char
do {		// main loop

	if ((cc > ' ' && cc < '~') && (cc != '/'))   //~ is a special character that indicates next letter is a command
			{		//letters to be printed
		aux1=0;
		do {
			ONEW[aux1++]=cc;  //transfer a whole word to ONEW
			cc=TEXT[++PNTR]; //get next char

		} while ((cc > ' ' && cc < '~') && (cc != '/'));
		ONEW[aux1]='\0';  //Mark end of the string
		PRINTIT (ONEW, &XCURSOR, &YCURSOR);

			}
	else			//command to be executed
	{
		if (cc == ' ') // cc is a space character
			PRINTIT (" ", &XCURSOR, &YCURSOR); //print a space

		if (cc == '/')  // a command follows
		{
			cc=TEXT[++PNTR]; //get next char that represents a command

			switch (cc) {  // fetch for the command

				case 'L':		// to jump do next line
					XCURSOR = 0; //first collum of next line
					if (++ YCURSOR > MAXLIN) //now, advance to next line and check if fits in current page
					{   // if not, next page
						WAIT_A_KEY ();  //wait for a key pressed before changing page
						WAIT_NO_KEY (); //now wait release the key
						YCURSOR=0; // first line of next page
						LCD_COLORWINDOW (0, 0, 159, 126, WHITE);  //clear screen

					}

					break;

				case 'P':		//  wait key and next page
					XCURSOR = 0; //first collum
					WAIT_A_KEY ();  //wait for a key pressed before changing page
					WAIT_NO_KEY (); //now wait release the key
					YCURSOR=0; // first line of next page
					LCD_COLORWINDOW (0, 0, 159, 126, WHITE);  //clear screen
					break ;

				case 'Z':	//beep
					BUZZER(3000);
					break;

				case '/':  //print a /
				PRINTIT ("/", &XCURSOR, &YCURSOR); //print a space
				break;

				case 'B':	//BLACK
					frground=BLACK;
					break;

				case 'R':	//RED
					frground=RED;
					break;

				case 'S':	//SKY BLUE
					frground=BLUE;
					break;

				case 'G':	//GREEN
					frground=GREEN1;
					break;

				case 'W':	//WOOD BROWN
					frground=SWOOD;
					break;


			}

		}

	cc=TEXT[++PNTR]; //get next char
	}
} while (cc != 0); // repeat until end of text

WAIT_A_KEY ();  //wait for a key pressed before exit
WAIT_NO_KEY (); //wait no key pressed

	SOFT_FLAGS |=BIT_REDRAW;

}

void PRINTIT (const char w[], unsigned char *X, unsigned char *Y)
// to print the character(s) in w. But first will check current position and jump to next line or next page if necessary
// will also update the position of the cursor after printing making it ready for next char.
{

	for ( aux2=0; w[aux2] != '\0'; ++aux2); //first lets calculate the size of the word to be printed
											//could use the strlen() command in string.h
											//but just to the sake of didactic lets implement it using one line command loop
											//and aux2 will hold the lengh of the string in w (must and with \0)

	if (*X+aux2 > MAXCOL+1)  //test if the word fits in current line

	{		//if not jump to next line
		*X = 0; //first collum of next line
		if (++ *Y > MAXLIN) //now, advance to next line and check if fits in current page
		{   // if not, next page
			WAIT_A_KEY ();  //wait for a key pressed before changing page
			WAIT_NO_KEY (); //now wait release the key
			*Y=0; // first line of next page
			LCD_COLORWINDOW (0, 0, 159, 126, WHITE);  //clear screen

		}

	}

	LCD_WORD(w,*X * 8,*Y * 14,0);  //  print it at current cursor position
	*X += aux2; //and update horizontal position of cursor

}

void RELOAD (void)
{


switch (SELECTMENU(MRESET,GREY2,BLACK)) {
	case 1:  //PRESET

		SOFT_FLAGS |= BIT_PRESET;
		break;
	case 2:  //RESET

		SOFT_FLAGS |= BIT_RESET;
		break;
	case 3: //enter setting mode
		SOFT_FLAGS |= BIT_SET;
		break;

	case 4:  //EXIT

		SOFT_FLAGS |= BIT_RETURN;
		break;

}
SOFT_FLAGS |= BIT_REDRAW;  //renew screen
}


/********** SELECTMENU **********/
// a small menu with 5 options of 10 chars each
// @ key -> next    >| key -> select
// first line free for title
//will return number correspondent to selection

char SELECTMENU (const char ITEMS[6][11],unsigned int frg, unsigned int bkg)

{
	cursor=0;
	frground=frg;
	bkground=bkg;

	LCD_COLORWINDOW (0, 125, 159, 127, BLACK);
	LCD_COLORWINDOW (0, 0, 159, 125, bkground);  //clear screen

	font=alpha_lg;
 	justify=CENTER;
	LCD_WORD (ITEMS[0], 0,1,10);				//write title

	justify=LEFT;
	for (int i = 1; i < 6; ++i) {    			//write all items
			LCD_WORD (ITEMS[i],0,3+i*21,10);
		}

	SLCTM_WRITE_ITEMS( ITEMS, frg,  bkg);// position cursor to first valid position

	WAIT_NO_KEY (); //wait no key pressed
	TIMER_3=5000;  //timeout
	state_MENU = WAIT_U;

	//MAIN LOOP:
	do {
		 GO_SLEEP();	//wait something happens


		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt


		if (SYS_FLAGS &  (BIT_KEY_P1 | BIT_KEY_P2)) //Player Key status changed
			SYS_FLAGS &= !(BIT_KEY_P1 | BIT_KEY_P2);  //clear flag

		if (SYS_FLAGS &  BIT_KEY_M) //Menu Key status changed
		{
			SYS_FLAGS &= !(BIT_KEY_M);  //clear flag

				if ((GET_KEY (BIT_KEY_M))==6) // next key just pressed
				{
					TIMER_3=0; //stop timer
					SLCTM_WRITE_ITEMS( ITEMS, frg,  bkg);// advance cursor
					WAIT_NO_KEY (); //wait no key pressed
					TIMER_3=5000; //timeout
				}

				if ((GET_KEY (BIT_KEY_M))==5) // >| Key pressed
								{
									TIMER_3=0; //stop timer
									WAIT_NO_KEY (); //wait no key pressed
									state_MENU = SELECTED;
								}

			}

		 if (SYS_FLAGS &  BIT_TIMER_3) //Timeout
		 {
				SYS_FLAGS ^= BIT_TIMER_3;  //clear flag
				cursor=0;  // nothing selected
				state_MENU = SELECTED;
		 }


	} while (state_MENU != SELECTED);

	return (cursor);

}



void SLCTM_WRITE_ITEMS(const char ITEMS[6][11],unsigned int frg, unsigned int bkg)// write items, move cursor to next valid position. update cursor variable
{
	justify=LEFT;
	frground=frg;
	bkground=bkg;

		if (cursor)  // erase last cursor position
		{
			frground=frg;
			bkground=bkg;
			aux1=frg & 0x52AA;	//used to create a frame around cursor
			aux2=frg | 0x38e7; //used to create a frame around cursor

			LCD_WORD (ITEMS[cursor],0,3+cursor*21,10); //write item under current cursor position
			LCD_LINE (0,2+cursor*21,159,2+cursor*21,bkground);
			LCD_LINE (0,21+cursor*21,159,21+cursor*21,bkground);
		}

		frground=bkg;
		bkground=frg;

	do {	// find next cursor position and write item in reverse
		if (cursor==5) cursor=0;
		if (ITEMS[++cursor][0]){ LCD_WORD (ITEMS[cursor],0,3+cursor*21,10); //write item under cursor in reverse
		LCD_LINE (0,2+cursor*21,159,2+cursor*21,aux2); //draw a frame around cursor
		LCD_LINE (0,2+cursor*21,0,21+cursor*21,aux1);
		LCD_LINE (1,2+cursor*21,1,21+cursor*21,aux1);
		LCD_LINE (0,21+cursor*21,159,21+cursor*21,aux1);
		LCD_LINE (0,20+cursor*21,159,20+cursor*21,aux1);
		LCD_LINE (159,2+cursor*21,159,21+cursor*21,aux1);
		LCD_LINE (158,2+cursor*21,158,21+cursor*21,aux1);
		}

	} while (!ITEMS[cursor][0]);
}




/********** MUTE CONTROL **********/
void MUTE(void)
{
	state_MUTE=KEYM_PRESSED;
	SOFT_FLAGS ^= BIT_BUZZER_ON;   //invert speaker flag
	if (SOFT_FLAGS & BIT_BUZZER_ON)  // test speaker flag
	{
		BUZZER(555);
		SPK();

	}
	else
		NO_SPK();

	TIMER_4=270; //will check key every 270ms

		do {
		GO_SLEEP();
		 if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

		 if (SYS_FLAGS & BIT_TIMER_4)  // timer ends
		 {
			 SYS_FLAGS ^= BIT_TIMER_4; //clear flag
			 switch (state_MUTE) {
			 case KEYM_PRESSED:
			 {
			 	if ((GET_KEY (BIT_KEY_M))==7) // check if key is pressed

			 		TIMER_4=270; //will check key every 270ms
			 	else // key just released
			 	{
			 		state_MUTE=KEYM_RELEASED;
			 		TIMER_4=2005; //will display for 2 seconds before return
			 	}
		 }
			 break;

			 case KEYM_RELEASED:
				 state_MUTE=END_MUTE; //after time after release key, exit
			 }
		 }
			if (SYS_FLAGS & BIT_KEY_M)  // a function key changed
			{
				SYS_FLAGS ^= BIT_KEY_M;  //clear flag
				switch (state_MUTE) {
					case KEYM_PRESSED:
					{
					 	if ((GET_KEY (BIT_KEY_M))==7) // check if key is pressed

					 		TIMER_4=270; //will check key every 270ms
					 	else // key just released
					 	{
					 		state_MUTE=KEYM_RELEASED;
					 		TIMER_4=2005; //will display for 2 seconds before return
					 	}
					}
					break;
					case KEYM_RELEASED:
					{
					 	if ((GET_KEY (BIT_KEY_M))==7) // check if key is pressed again

					 	{
					 		state_MUTE=KEYM_PRESSED;
					 		SOFT_FLAGS ^= BIT_BUZZER_ON;   //invert speaker flag
					 		if (SOFT_FLAGS & BIT_BUZZER_ON)  // test speaker flag
					 			{
					 			BUZZER(555);
					 			SPK();

					 			}

					 		else
					 			NO_SPK();

					 		TIMER_4=270; //will check key every 270ms
					 	}
					 	else

							 state_MUTE=END_MUTE; //another M key pressed, so exit


						 	}
					}
			}

		} while (state_MUTE != END_MUTE);
		WAIT_NO_KEY (); //wait no key pressed

			SOFT_FLAGS |=BIT_REDRAW;


}

	 void SPK ()
	 {

			LCD_COLORWINDOW (43,27,117,101,WHITE);  // white board
			LCD_COLORWINDOW (33,17,127,27,RED);	// red frame
			LCD_COLORWINDOW (33,101,127,111,RED);
			LCD_COLORWINDOW (33,27,43,101,RED);
			LCD_COLORWINDOW (117,27,127,101,RED);

			for (char n=0; n<24 ; n++ )
				LCD_LINE (n+77,61-n,n+77,67+n,BLACK);  //spk cone

			LCD_COLORWINDOW (74,51,59,77,BLACK);  //spk magnet

	 }

	 void NO_SPK()
	 {
		 	 SPK();
			//LCD_LINE (43,101,117,27,BLACK); //no bar
			 //LCD_LINE (43,100,117,26,BLACK);
			 //LCD_LINE (43,102,117,28,BLACK);
			 //LCD_LINE (43,99,117,25,BLACK);
			 //LCD_LINE (43,103,117,29,BLACK);
		 	LCD_LINE (53,91,107,37,BLACK); //no bar
		 	LCD_LINE (53,37,107,91,BLACK); //no bar
		 	LCD_LINE (53,92,107,38,BLACK); //no bar
		    LCD_LINE (53,38,107,92,BLACK);
		 	LCD_LINE (53,93,107,39,WHITE); //no bar
		 	LCD_LINE (53,39,107,93,WHITE);
	 }


/********** Roll a die **********/
void DICE(void)
{
	srand(TIMER_TIC);    //  create a seed for random number. xxx considerar colocar em local unico



	LCD_COLORWINDOW (0,27,159,101,TOPTABLE);  //draw a dice tray
	LCD_COLORWINDOW (0,27,10,101,SWOOD);
	LCD_COLORWINDOW (159,101,149,27,SWOOD);
	LCD_COLORWINDOW (0,17,159,27,SWOOD);
	LCD_COLORWINDOW (0,101,159,111,SWOOD);

	state_DICE=SHAKING_DIE;
	LCD_DICE (rand() % 6 +1,53,37,WHITE,BLACK,HBUZZ);  // roll die
	TIMER_DICE = ROLLSPEED;	//set timer for next roll

do {		// main loop

	 GO_SLEEP();	//wait something happens

	 if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

	 if (SYS_FLAGS & BIT_TIMER_DICE)  // timer ends
	 {
		 SYS_FLAGS ^= BIT_TIMER_DICE; //clear flag
		 switch (state_DICE) {
			case SHAKING_DIE:  //holding dice key is like shaking die
			{
				if ((GET_KEY (BIT_KEY_M))!=3) // check if key is really pressed
				{
					state_DICE=THROW_DIE; //  release die
					ROLL_PHASE=(rand() % 16); //control rolling after throw

				}
					LCD_DICE (rand() % 6 +1,53,37,WHITE,BLACK,HBUZZ);  // roll die
					TIMER_DICE = ROLLSPEED;	//set timer for next roll
			}
				break;
			case (THROW_DIE):
		{
				if (++ROLL_PHASE < 20) //will roll dice for a while after thrown
				{
					LCD_DICE (rand() % 6 +1,53,37,WHITE,BLACK,RBUZZ);
					TIMER_DICE = ROLLSPEED + ROLL_PHASE; // decreasing speed
				}
				else
				{
					state_DICE=STOP_ROLL;
					TIMER_DICE = WAIT_STOP;
				}

		}
			break;
			case STOP_ROLL: // finishing roll
			state_DICE = END_DICE;

		 	 	 	 	 	 }
	 }
		if (SYS_FLAGS & BIT_KEY_M)  // a function key changed
		{
			SYS_FLAGS ^= BIT_KEY_M;  //clear flag
			switch (state_DICE) {
				case SHAKING_DIE:
				{
					if ((GET_KEY (BIT_KEY_M))!=3)  //check if just released die
					{
						state_DICE=THROW_DIE;
						ROLL_PHASE=(rand() % 16); //control rolling after throw

					}
				}

					break;


				case THROW_DIE:
				{
					if ((GET_KEY(BIT_KEY_M))==3)  //pressed again
					{
					TIMER_DICE = ROLLSPEED;
					state_DICE= SHAKING_DIE; //return to shake xxx talvez desenhar um 2º dado
					}
				}

					break;

				case STOP_ROLL:
				{
					if ((GET_KEY(BIT_KEY_M))==3)  //pressed again
					{
					TIMER_DICE = ROLLSPEED;
					state_DICE= SHAKING_DIE; //return to shake xxx talvez desenhar um 2º dado
					}
					else
					{
					TIMER_DICE=1; //force exit if any key other MENU then dice key
					state_DICE=STOP_ROLL;
					}

				}
			}
		}
} while (state_DICE != END_DICE);

WAIT_NO_KEY (); //wait no key pressed

	SOFT_FLAGS |=BIT_REDRAW;

}
/********** Draw a die **********/

void LCD_DICE (unsigned char d, unsigned char x, unsigned char y,unsigned int color,unsigned int pip, int click)
// d= 0 ~ 6 / x,y = top left corner / color= dice color / pip= pip color / click = a click length
{
	LCD_COLORWINDOW (x,y,x+54, y+54, color);

	switch (d) {
		case 1:
			LCD_DOT11 (x+27,y+27,pip);
			break;
		case 2:
			LCD_DOT11 (x+11,y+43,pip);
			LCD_DOT11 (x+43,y+11,pip);
			break;
		case 3:
			LCD_DOT11 (x+11,y+43,pip);
			LCD_DOT11 (x+43,y+11,pip);
			LCD_DOT11 (x+27,y+27,pip);
			break;
		case 4:
			LCD_DOT11 (x+11,y+43,pip);
			LCD_DOT11 (x+43,y+11,pip);
			LCD_DOT11 (x+11,y+11,pip);
			LCD_DOT11 (x+43,y+43,pip);
			break;
		case 5:
			LCD_DOT11 (x+11,y+43,pip);
			LCD_DOT11 (x+43,y+11,pip);
			LCD_DOT11 (x+11,y+11,pip);
			LCD_DOT11 (x+43,y+43,pip);
			LCD_DOT11 (x+27,y+27,pip);
			break;
		case 6:
			LCD_DOT11 (x+11,y+43,pip);
			LCD_DOT11 (x+43,y+11,pip);
			LCD_DOT11 (x+11,y+11,pip);
			LCD_DOT11 (x+43,y+43,pip);
			LCD_DOT11 (x+11,y+27,pip);
			LCD_DOT11 (x+43,y+27,pip);
			break;
		default:
			break;
	}
BUZZER (click); // make a click noise  xxx talvez fazer diferente quando larga o dado

}





