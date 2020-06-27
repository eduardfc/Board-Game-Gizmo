#include "main.h"
#include "LCD_ST7735.h"
#include "APP_SCORE.h"
#include "APP_CHRONO.h"
#include "FONT_number_xg.h"
#include "FONT_number_lg.h"
#include "FONT_number_ss.h"
#include "FONT_alpha_lg.h"
#include "FONT_number_tny.h"

#include <stdlib.h>

extern unsigned long int CHRONO_1;  // chronograph 1 (50ms~50days) used to mark time
extern unsigned long int SCHRONO_1; // auxiliar for chrono. only step values multiple of CHRONO_x_ALARM
extern unsigned long int CHRONO_2;  // chronograph 2 (50ms~50days) used to mark time
extern unsigned long int SCHRONO_2; // auxiliar for chrono. only step values multiple of CHRONO_x_ALARM
unsigned long int CHRONO_AUX;
extern unsigned long int CHRONO_1_ALARM;
extern unsigned long int CHRONO_2_ALARM;
extern unsigned int TIMER_1;
extern unsigned int TIMER_2;
laps_db CHRON[6]; //lap marks
const char MHORGLASS[6][11]={"MODE", " NORMAL", " CHESS","","",""}; //itens for mode selection menu
extern unsigned int SYS_FLAGS;
extern unsigned int SOFT_FLAGS;
extern const char *HELP;  //pointer to help text array
extern unsigned short V_BATT;

unsigned char HOR, MIN, SEC; // HH:MM:SEC (HH= 0~99)
unsigned char state_chrono;
unsigned char lapmode;
unsigned int bgcolor; //bargraph color
int i,j,k, yh2,xh2,yh1,xh1;// general use
unsigned int turn; // will keep the turn number in chess;
unsigned char auxdelay=0; // auxiliar ti keep delay counting
const char LAPSET[6][11]={"MEM MODE", " ABSOLUTE", " DELTA","",""," CANCEL"};
chess_db CPLAYERS [2] = {   //Array to store players timers and etc.
		{COLORP1 , &CHRONO_1 , &SCHRONO_1 , &CHRONO_1_ALARM , 0,0, 0, BIT_CHRONO_1_RUN, BIT_CHRONO_1_ALARM, BIT_KEY_P1,0,0,LEFT},
		{COLORP2 , &CHRONO_2 , &SCHRONO_2 , &CHRONO_2_ALARM , 0, 0, 0, BIT_CHRONO_2_RUN, BIT_CHRONO_2_ALARM, BIT_KEY_P2,0,108,RIGHT}
};

const char HELP_CHRONO[]={"/W    CHRONOGRAPH/L/BA simple time keeper that counts to 99:99:99./L/WKeys:/L/B&: Start//Stop./LP1,P2: Record a lap or current time./L%/GPRESET:/B Zero keeping records./L%/GRESET:/B Zero everything./L%/GSET: /BSet record mode to ABSOLUTE or DELTA (diferential)./Z"};
const char HELP_TIMER[]={"/R       TIMER/L/BA countdown timer that counts from 99:99:99./L/RKeys:/L/B&: Start//Stop./LP1,P2: Restart counting from initial value./L%/GPRESET:/B Restart paused to initial value./L%/GRESET:/B Restart paused to default value./L%/GSET: /BSet initial value using:/LP1: Set Minutes and Hours/LP2: Set Seconds/L&Enter value/Z"};
const char HELP_HGLASS[]={"/S     HOURGLASS/L/BA simple hourglass timing from 00:05 to 59:59 in two modes:/L/SNormal mode:/BStart all sand in one side/L/BKeys:/L/B&: Start//Pause./LP1,P2: Start//Flip hourglass./L/SChess mode:/BSplits sand between each hourglass side. Each side is the remaining time for each player./L/BKeys:/L/B&: Randonly start P1//P2 or Start//Pause/./LP1,P2: Start//Flip counting against other player./L%/GPRESET:/B Restart./L%/GRESET:/B Restart default value./L%/GSET: /BSet initial value using:/LP1: Set Minutes/LP2: Set Seconds/L&Enter value/L Set normal or chess mode./Z"};

char ANY_KEY( unsigned int);
char GET_KEY_VALUE (unsigned int);
void CHR_REDRAW (void);
void GO_SLEEP(void);
void DISPLAY_BATT(void);
void LCD_NUMBER (int n, unsigned char x, unsigned char y, unsigned char field);
void APP_FUNCTION_MAIN();
void CNT_TO_HMS (unsigned char *HOR, unsigned char *MIN, unsigned char *SEC, unsigned long int CHRONO);
void CHR_LAPS (void);
void LCD_LINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
void CHR_HHMMSS (unsigned long int CHRONO, unsigned char y);
void BUZZER (int pulse);
char GET_KEY ( unsigned int group);
void WAIT_NO_KEY (void);
void BARGRAPH (unsigned long int num, unsigned long int max, unsigned int LED_ON, unsigned int LED_OFF, unsigned int BCKGRND);
void RT_REDRAW (void);
void CHR_STARTIME (unsigned long int CHRONO);
char SELECTMENU (const char ITEMS[6][11],unsigned int frg, unsigned int bkg);
void CHESS_REDRAW(char mode);
void LCD_WORD (const char w[], unsigned char x, unsigned char y, unsigned char field);
void CHR_HMMSS_V (unsigned long int CHRONO, unsigned char x);
void CHR_DELAY_SS (unsigned char DELAY, unsigned char x);
void LCD_FRAME(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
void DRAWFLAG (char player, unsigned int color);
void HG_REDRAW(int x, int lvup, int lvdw, int cx);
void  DRAW_HOURGLASS (int x, int LVLUP, int LVDWN, unsigned int SNDCLR) ;
void LCD_PLOT(unsigned char X, unsigned char Y, unsigned int color);
void UPD_HGLVL_BT (int x, int LVL, unsigned int SNDCLR);
void UPD_HGLVL_TP (int x, int LVL, unsigned int bckgcolor);
void LCD_DOTLINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color, unsigned int bkcolor, unsigned char densit);
void CHR_MMSS (unsigned long int CHRONO, unsigned char x, unsigned char y, char size);
void DRAW_HOURGLASS_H ( int LVLUP, int LVDWN, unsigned int SNDCLR);
void TOILET (void);


void HOURGLASS (void)
{
	CHRONO_1=0; //reset chrononometer
	SCHRONO_1=0;
	CHRONO_1_ALARM = 100; //CHRONO_1 generates interrupt every 100x1ms= 0.25s
	CHRONO_AUX=180000; // var for the start time, default = 3 min
	unsigned int lvlbk=MAXSANDLEVEL;  //sand level aux
	unsigned int lvlhg=0;  //sand level botton hourglass
	int hx = 50; // horizontal position of hg
	int cx = 110; // horizontal position of mm:ss
	SOFT_FLAGS &= ~CHESS_MODE;  // start not in chess mode
	HELP=HELP_HGLASS; //set help file



	SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
	state_chrono = C_STOP;  // begin program with chrono stopped

	SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off


	 HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg, cx);



	do {
		GO_SLEEP();	//wait something happens



		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt



		if (SYS_FLAGS & BIT_KEY_M) // a function key is pressed
		{
			SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag

				if ((GET_KEY (BIT_KEY_M))==5)  // play key pressed
				{
					switch (state_chrono) {

						case C_UPDATE:   //exit the time update
							state_chrono=C_MUPDATE; //exit start time edit mode
							bkground=BLACK;
							frground=YELLOW;

							if (SELECTMENU(MHORGLASS,GREY2,BLACK)==1)   //enter mode selection menu
							{
								SOFT_FLAGS &= ~CHESS_MODE;   //set mode to normal
								CHRONO_1=0; // start 100%  0%
								SCHRONO_1=0;
								lvlhg=0;
								lvlbk=0;

							}
							else
							{
								SOFT_FLAGS |= CHESS_MODE;   //set mode to chess
								lvlhg=MAXSANDLEVEL/2;   // start 50% 50% each side if chess mode
								lvlbk=0;
								CHRONO_1=CHRONO_AUX/2;
								SCHRONO_1=CHRONO_1;
							}

							SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off

						case C_MUPDATE:
							state_chrono=C_STOP; //exit edit mode

							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
							TIMER_1=0; //stop time out timer



							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
							break;

						case C_RUN: 	//running in normal mode, will pause
						SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //pause chrono
						state_chrono = C_PAUSE; //pause chrono is not the same as stop the chrono (when chrono -> time= zero)
						HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

						break;

						case C_P1RUN:  // P1 running in chess mode , will pause
							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //pause chrono
							state_chrono = C_P1PAUSE; //pause chrono is not the same as stop the chrono (when chrono -> time= zero)
							SCHRONO_1=CHRONO_AUX-SCHRONO_1+999; // change the position of time shown in horizontal hourglass
							HG_REDRAW(hx,lvlhg,MAXSANDLEVEL-lvlhg, cx);
							SCHRONO_1=CHRONO_AUX-SCHRONO_1+999;  //return back to the correct position

							break;

						case C_P2RUN:  // P2 running in chess mode , will pause
							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //pause chrono
							state_chrono = C_P2PAUSE; //pause chrono is not the same as stop the chrono (when chrono -> time= zero)
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

							break;

						case C_PAUSE:  // if paused, restart (only by pressing PlayPause key, will not restart by Px Key)
							BUZZER (3000);
							state_chrono = C_RUN;
							SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
							break;
						case C_P1PAUSE:// if paused, restart (only by pressing PlayPause key, will not restart by Px Key)
							BUZZER (3000);
							state_chrono = C_P1RUN;
							SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
							break;
						case C_P2PAUSE:// if paused, restart (only by pressing PlayPause key, will not restart by Px Key)
							BUZZER (3000);
							state_chrono = C_P2RUN;
							SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
							break;

						case C_STOP:   //when stopped and press play, randomly choose a start side in chess mode. or just run in normal mode
							BUZZER (3000);

							if (SOFT_FLAGS & CHESS_MODE)  //if chess mode, first choose which Player side to run
							{
								if (rand()%2)
								{
									state_chrono = C_P1RUN;
									hx=0; // horizontal position of hg
									cx=60;  // horizontal position of mm:ss
								}
								else
								{
									state_chrono = C_P2RUN;
									hx=110;
									cx=73;
								}
								lvlhg=MAXSANDLEVEL/2;   // start 50% each side
								lvlbk=0;
								CHRONO_1=CHRONO_AUX/2;
								SCHRONO_1=CHRONO_1;


							}
							else   //or start in normal mode
							{
								state_chrono = C_RUN;
								hx = 50;
								cx = 110;
								lvlbk=MAXSANDLEVEL;   // start 100%
								lvlhg=0;
								CHRONO_1=0;
								SCHRONO_1=0;
							}
							SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

					}



				}
				else	APP_FUNCTION_MAIN(); //other function key pressed


		}

		if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
			{
			if (SOFT_FLAGS & BIT_RESET) SOFT_FLAGS &= ~CHESS_MODE; //if reset, mode = normal mode

			CHRONO_1=0; //set chrononometer to start value
			SCHRONO_1=0;
			lvlbk=MAXSANDLEVEL;  //sand level aux
			lvlhg=0;  //sand level botton hourglass


			if (SOFT_FLAGS & CHESS_MODE)   //check if chess mode mode to define sand position
			{

				lvlhg=MAXSANDLEVEL/2;   // start 50% 50% each side if chess mode
				lvlbk=0;
				CHRONO_1=CHRONO_AUX/2;
				SCHRONO_1=CHRONO_1;

			}
			else
			{
					CHRONO_1=0; // start 100%  0% if normal mode
					SCHRONO_1=0;
					lvlhg=0;
					lvlbk=0;

			}

				SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
				state_chrono = C_STOP;  // chrono stopped

				if (SOFT_FLAGS & BIT_RESET) // if RESET clear start and chrono, if PRESET clear chrono only
				{
					CHRONO_AUX=180000;
					SOFT_FLAGS &= ~CHESS_MODE;  // start not in chess mode

				}


				SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag

				HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
			}

		if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
		{
			SOFT_FLAGS &= ~BIT_REDRAW;
			HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

		}

		if (SOFT_FLAGS & BIT_SET)  //Setting mode
			{
			SOFT_FLAGS &= ~BIT_SET;  //clear flag

			SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;

			state_chrono=C_UPDATE; //enter start time edit mode
			bkground=BLACK;
			frground=YELLOW;
			LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear edition area

			CHR_MMSS (CHRONO_AUX,20,50,1); // print actual start value

			CHRONO_1=0;   //restart chrono1 just to flash screen while editing
			SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start chrono

			TIMER_1=7000; //will exit edit mode after 7s time out

			SOFT_FLAGS |= BIT_KREPEAT; // set player key repeat on
			}


		if (SYS_FLAGS & (BIT_KEY_P1 | BIT_KEY_P2)) // check player key pressed
		{
			SYS_FLAGS &= ~ (BIT_KEY_P1 | BIT_KEY_P2); //reset player  KEY flag
			if (ANY_KEY (BIT_KEY_P1 | BIT_KEY_P2)) //yes! a player key was pressed
			{
				switch (state_chrono) //only update start time if chrono is stopped and SET mode
				{
					case C_RUN:		//rotate hourglass when Px pressed and normal mode
					//case C_PAUSE:
						CHRONO_1=CHRONO_AUX-CHRONO_1; // chrono = passed  time
						SCHRONO_1=CHRONO_1+999;
						SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
						state_chrono = C_STOP;  // stop counter to draw hourglass horizontaly
						lvlbk= (MAXSANDLEVEL * SCHRONO_1)/CHRONO_AUX;  //calculate sand level
						lvlhg=lvlbk;

						HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);  //draw hourglass horizontally just to rotate effect

						SOFT_FLAGS |= BIT_CHRONO_1_RUN;
						state_chrono = C_RUN;  // run counter
						HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

						break;


					case C_STOP:
						BUZZER (3000);
						if (SOFT_FLAGS & CHESS_MODE)  //if chess mode, first choose which Player side to run
						{
							if (ANY_KEY (BIT_KEY_P2))  //P2 key pressed so start P1 timer
							{
								state_chrono = C_P1RUN;
								hx=0; // horizontal position of hg
								cx=60;  // horizontal position of mm:ss
							}
							else
							{
								state_chrono = C_P2RUN;
								hx=110;
								cx=73;
							}
						}
						else   //or start in normal mode
						{
							state_chrono = C_RUN;
							hx = 50;
							cx = 110;
							lvlbk=MAXSANDLEVEL;   // start 100%
							lvlhg=0;
							CHRONO_1=0;
							SCHRONO_1=0;
						}
						SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start
						HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);

						break;

					case C_P1RUN:		//rotate hourglass when P1 pressed and normal mode
					//case C_P1PAUSE:   //exit from pause pressing PlayPause key only
						if (ANY_KEY (BIT_KEY_P1))  //only P1 key can rotate while P1 time running
						{
							CHRONO_1=CHRONO_AUX-CHRONO_1; // chrono = passed  time
							SCHRONO_1=CHRONO_1+999;
							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
							state_chrono = C_STOP;  // stop counter

							lvlbk= (MAXSANDLEVEL * SCHRONO_1)/CHRONO_AUX;  //calculate sand level
							lvlhg=lvlbk;
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);  //draw hourglass horizontally just to rotate effect

							hx=110;  //position hourglass to P2
							cx=73;

							SOFT_FLAGS |= BIT_CHRONO_1_RUN;
							state_chrono = C_P2RUN;  // run counter  P2 side
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
						}

						break;

					case C_P2RUN:		//rotate hourglass when P2 pressed and normal mode
					//case C_P1PAUSE:   //exit from pause pressing PlayPause key only
						if (ANY_KEY (BIT_KEY_P2))  //only P2 key can rotate while P2 time running
						{
							CHRONO_1=CHRONO_AUX-CHRONO_1; // chrono = passed  time
							SCHRONO_1=CHRONO_1+999;
							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
							state_chrono = C_STOP;  // stop counter
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);  //draw hourglass horizontally just to rotate effect

							lvlbk= (MAXSANDLEVEL * SCHRONO_1)/CHRONO_AUX;  //calculate sand level
							lvlhg=lvlbk;

							hx=0;  //position hourglass to P2
							cx=60;

							SOFT_FLAGS |= BIT_CHRONO_1_RUN;
							state_chrono = C_P1RUN;  // run counter  P1 side
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
						}

						break;

					case C_PAUSE:	// if paused, can not run again by pressing Px key. so flash and buzzer as error
					case C_P1PAUSE:
					case C_P2PAUSE:
						for (i=0;i<5;i++)
						{
							BUZZER (211);
							HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg,cx);
						}
						break;

					case C_UPDATE:
						TIMER_1=7000; //will exit edit mode after 7s time out
						CHRONO_AUX +=(GET_KEY_VALUE(BIT_KEY_P1)*60000+GET_KEY_VALUE(BIT_KEY_P2)*1000); //update initial value according to pressed key

																	// keep value between 0 and 59m59s
						if (CHRONO_AUX > 12599000) CHRONO_AUX=5000; //chrono_aux is unsigned, so if subtract bellow zero then result big positive number, fix to 0:00
						if (CHRONO_AUX > 3599000) CHRONO_AUX=3599000; // if add above 59:59, fix to limit
						if (CHRONO_AUX < 5000) CHRONO_AUX=5000; //5s is the minimum

						bkground=BLACK;
						frground=YELLOW;
						// CHR_HHMMSS (CHRONO_AUX,50); // print actual start value
						CHR_MMSS (CHRONO_AUX,20,50,1); // print actual start value

						break;
				}

			}

		}




		if (SYS_FLAGS & BIT_CHRONO_1_ALARM)   // Print CHRONO
			{
			SYS_FLAGS &= ~BIT_CHRONO_1_ALARM;

			switch (state_chrono) {
				case C_RUN: // if running, check, print counting and update sand
				case C_P1RUN:
				case C_P2RUN:

					if (!(SCHRONO_1%1000)) {
						frground=YELLOW;
						CHR_MMSS (CHRONO_AUX-SCHRONO_1+999, cx, 30,0); // print remaining time
						frground=GREY1;
						CHR_MMSS (SCHRONO_1, cx, 85,0); // print upcounting time
					}

					lvlbk= (MAXSANDLEVEL * SCHRONO_1)/CHRONO_AUX;  //calculate sand level
					LCD_DOTLINE (hx+24,63,hx+24,119-lvlhg,YELLOW,BLACK,5);  //draw falling sand
					if (lvlbk!=lvlhg) {				// if different of last value, update
						lvlhg=lvlbk;
						UPD_HGLVL_BT (hx,lvlhg,YELLOW);
						UPD_HGLVL_TP (hx,MAXSANDLEVEL-lvlhg+1,BLACK);

					}



					if ((CHRONO_AUX-SCHRONO_1+999)<11000)  //if less than 10 s beep once
					{
						if (!((SCHRONO_1/100)%10))
							BUZZER (100);
						if ((CHRONO_AUX-SCHRONO_1+999)<6000) //if less than 5 s beep twice
						{
							if ((SCHRONO_1/100)%10)
								BUZZER (50);
						}
					}


					if (!(CHRONO_AUX > SCHRONO_1))  //TIME OUT!!!
					{
						SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
								BUZZER (7000);
								state_chrono = C_STOP; //state running
								LCD_LINE(hx+24,63,hx+24,119-lvlhg,BLACK);  //erase falling sand

								if (SOFT_FLAGS & CHESS_MODE)  //set time and sand for next run
								{

									lvlhg=MAXSANDLEVEL/2;   // start 50% 50% each side if chess mode
									lvlbk=0;
									CHRONO_1=CHRONO_AUX/2;
									SCHRONO_1=CHRONO_1;


								}
								else   //or start in normal mode
								{

									lvlhg=MAXSANDLEVEL/2;   // start 100% 0%
									lvlbk=0;
									CHRONO_1=CHRONO_AUX/2;
									SCHRONO_1=CHRONO_1;
								}




								// HG_REDRAW(hx,MAXSANDLEVEL-lvlhg, lvlhg);

					}


					break;

				case C_UPDATE: //if updating, flash count
					if ((SCHRONO_1/100)%10) frground=YELLOW;
					else frground=GREY1;

					bkground=BLACK;
					CHR_MMSS (CHRONO_AUX,20,50,1); // print actual start value

					break;
			}

			}

		if (SYS_FLAGS & BIT_TIMER_1)  //start time edition time out
			{
			SYS_FLAGS ^= BIT_TIMER_1; //clear flag
			if (state_chrono == C_UPDATE)
			{
				state_chrono=C_STOP; //exit start time edit mode
				bkground=BLACK;
				frground=YELLOW;
				LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear edition area
				SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
				CHRONO_1=0; // reset
				SCHRONO_1=0;
				HG_REDRAW(hx,lvlhg, MAXSANDLEVEL-lvlhg,cx);

			}

			}




	} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

	TOILET ();



}

void HG_REDRAW(int x, int lvup, int lvdw, int cx)
{
	if ((state_chrono==C_P1RUN)|| (state_chrono==C_P2RUN)|| (state_chrono==C_RUN)) // if running, draw a vertical hourglass
	{

		DRAW_HOURGLASS (x, lvup, lvdw, YELLOW);
		bkground=BLACK;
		frground=YELLOW;
		CHR_MMSS (CHRONO_AUX-SCHRONO_1+999, cx, 30,0);
		frground=GREY1;
		CHR_MMSS (SCHRONO_1, cx, 85,0);
	}
	else		// if stopped, draw a horizontal hourglass. This is the way to pause a real hourglass  :)
	{
		DRAW_HOURGLASS_H ( lvup, lvdw, YELLOW);
		frground=RED;
		bkground=BLACK;
		CHR_MMSS (CHRONO_AUX, 66, 14,0); // print HOURGLASS total time
		frground=GREY1;
		CHR_MMSS (SCHRONO_1, 25, 100,0);   // print botton hourglas time
		frground=YELLOW;
		CHR_MMSS (CHRONO_AUX-SCHRONO_1+999, 107, 100,0); // print remaining time


	}


}

void DRAW_HOURGLASS (int x, int LVLUP, int LVDWN, unsigned int SNDCLR)  // draw a hourglass on X
{

	LCD_COLORWINDOW (0, 0, 159, 126, BLACK);

	LCD_LINE(x,3,x,43,GREY2);
	LCD_LINE(x,43,x+20,63,GREY2);
	LCD_LINE(x,3,x+48,3,GREY2);
	LCD_LINE(x+48,3,x+48,43,GREY2);
	LCD_LINE(x+28,63,x+48,43,GREY2);

	LCD_LINE(x+20,63,x,83,GREY2);
	LCD_LINE(x,83,x,123,GREY2);
	LCD_LINE(x,123,x+48,123,GREY2);
	LCD_LINE(x+48,123,x+48,83,GREY2);
	LCD_LINE(x+48,83,x+28,63,GREY2);




	for (k=0; k < LVLUP; k++)

	{
		if (k<20) LCD_LINE (24+x-k,63-k,24+x+k,63-k,SNDCLR);
		else LCD_LINE (x+4,63-k,x+44,63-k,SNDCLR);
	}

	for (k=0; k < LVDWN; k++)
	{
		if (k<36) LCD_LINE (x+4,119-k,x+44,119-k,SNDCLR);
		else LCD_LINE (x+4+k-36,119-k,x+44-k+36,119-k,SNDCLR);
	}

}



void DRAW_HOURGLASS_H ( int LVLUP, int LVDWN, unsigned int SNDCLR)
// when stopped draw a horizontal hourglass
{

	LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //xxxx delete

	LCD_LINE(19,39,59,39,GREY2);  // The contorn of the hourglass
	LCD_LINE(59,39,79,59,GREY2);
	LCD_LINE(79,59,99,39,GREY2);
	LCD_LINE(99,39,139,39,GREY2);
	LCD_LINE(139,39,139,87,GREY2);

	LCD_LINE(139,87,99,87,GREY2);
	LCD_LINE(99,87,79,67,GREY2);
	LCD_LINE(79,67,59,87,GREY2);
	LCD_LINE(59,87,19,87,GREY2);
	LCD_LINE(19,87,19,39,GREY2);

LVLUP=(LVLUP*21)/45;  // adjust levels to fit display
LVDWN=(LVDWN*21)/45;
if (LVLUP) ++LVLUP;
if (LVDWN) ++LVDWN;


	for (k=0; k < 22; k++)			// the sand

	{
		if (k<LVLUP) LCD_LINE (99-k,84-k,135,84-k,SNDCLR);

		if (k<LVDWN) LCD_LINE (23,84-k,58+k,84-k,SNDCLR);

	}
}



void UPD_HGLVL_TP (int x, int LVL, unsigned int bckgcolor)// update sand level of bottom hourglass
//erase sand (triangle shaped) from level to top
{
	if (LVL>0)
	{
		if (LVL<38)
		{
		LCD_LINE (x+24,64-LVL,x+4,44-LVL,bckgcolor);
		LCD_LINE (x+24,64-LVL,x+44,44-LVL,bckgcolor );
		}
		else
		{
			LCD_LINE (x+24,64-LVL,x+LVL-34,7,bckgcolor);
			LCD_LINE (x+24,64-LVL, x+82-LVL,7,bckgcolor);
		}

		if (LVL<16)
		{

		LCD_LINE (x+4-LVL+16,64-LVL-5,x+44+LVL-16,64-LVL-5,bckgcolor);
		}
		else
		{
			LCD_LINE (x+4,64-LVL-5,x+44,64-LVL-5,bckgcolor);

		}

	}

}

void UPD_HGLVL_BT (int x, int LVL, unsigned int SNDCLR)  // update sand level of bottom hourglass
//draw sand (triangle shaped) from level to bottom)
{
    if (LVL>0)
    {
    	if (LVL<22)
    		{
    		LCD_LINE (x+24,120-LVL,x+25-LVL,119, SNDCLR);
    		LCD_LINE (x+24,120-LVL,x+23+LVL,119, SNDCLR);
    		}
    	else
    	{
    		LCD_LINE (x+24,120-LVL,x+4,119-LVL+21, SNDCLR);
    		LCD_LINE (x+24,120-LVL,x+44,119-LVL+21, SNDCLR);
    	}

    }
}

void CHESS (void)
{
	for (i=0; i<2 ;i++)		//setting intial values
	{
		*CPLAYERS[i].CHRONO=0;
		*CPLAYERS[i].SCHRONO=0;
		*CPLAYERS[i].CHRONO_ALARM=1000;
		CPLAYERS[i].TIME_MAIN_SET=DFTIMEMAIN; //initial main set
		CPLAYERS[i].TIME_MAIN=DFTIMEMAIN; //initial main
		CPLAYERS[i].TIME_DELAY=DFTIMEDELAY; //initial delay
		CPLAYERS[i].BONUS = 0;
		SOFT_FLAGS &= ~(CPLAYERS[i].F_CHRONO_RUN);
	}

	state_chrono = C_STOP;  // begin program with chrono stopped

	SOFT_FLAGS |= BIT_KREPEAT; // set player key repeat on
	SOFT_FLAGS &= ~(BURST1 | BURST2); // clear BURST flags (Burst flags indicate who first reached zero)

	turn=0; //reset turn number

	CHESS_REDRAW(2);

	do {
		 GO_SLEEP();	//wait something happens

		 if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt


			if (SYS_FLAGS & BIT_KEY_M) // a function key is pressed
			{
				SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag
				if (ANY_KEY(BIT_KEY_M))
				{
					switch (state_chrono) {

						case C_BUPDATE:  //if editing delay, will exit edition
							TIMER_1=0; //stop flashing timer
							SYS_FLAGS &= ~BIT_TIMER_1; //clear flag
							CHESS_REDRAW(0);
							state_chrono=C_STOP; //return from editing
							SOFT_FLAGS &= ~(BURST1 | BURST2); // clear BURST flags (Burst flags indicate who first reached zero)

							break;

						case C_DUPDATE:  //if editing delay, will edit bonus
							state_chrono=C_BUPDATE; //edit BONUS

							LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear screen

							font=alpha_lg;   // write BONUS
							bkground=BLACK;
							frground=WHITE;
							justify=LEFT;

							LCD_WORD ("BONUS",40,19,1);

							for (i=0; i<2 ;i++)		//print both players DELAY
							{
								frground=CPLAYERS[i].PLAY_COLOR;
								CHR_DELAY_SS (CPLAYERS[i].BONUS, CPLAYERS[i].X + 52);

							}

							break;

						case C_SUPDATE:  //if editing seconds, will edit delay
							state_chrono=C_DUPDATE; //edit delay

							LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear screen

							font=alpha_lg;   // write DELAY
							bkground=BLACK;
							frground=WHITE;
							justify=LEFT;

							LCD_WORD ("DELAY",40,19,1);

							for (i=0; i<2 ;i++)		//print both players DELAY
							{
								frground=CPLAYERS[i].PLAY_COLOR;
								CHR_DELAY_SS (CPLAYERS[i].TIME_DELAY, CPLAYERS[i].X + 52);

							}

							break;

						case C_MUPDATE:  //if editing minutes, will edit seconds
							state_chrono=C_SUPDATE; //edit seconds
							CHESS_REDRAW(2);
							break;

						case C_HUPDATE:  //if editing hours, will edit minutes
							state_chrono=C_MUPDATE; //edit minutes
							CHESS_REDRAW(2);
							break;

						default:  //execute a function of the MENU

							if ((GET_KEY (BIT_KEY_M))==5)  // play key pressed
							{
								if (state_chrono & (C_P1RUN | C_P2RUN | C_P1DELAY | C_P2DELAY))  // if main or delay running, chron stop
								{
									SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
									SOFT_FLAGS &= ~BIT_CHRONO_2_RUN;
									TIMER_2=0; //stop delay
									state_chrono = C_STOP; //state stopped
									SOFT_FLAGS |= BIT_KREPEAT; // set player key repeat on
									CHESS_REDRAW(0);

								}

							}
							else	if (!(state_chrono & (C_P1RUN | C_P2RUN | C_P1DELAY | C_P2DELAY))) APP_FUNCTION_MAIN(); //other function key pressed

					}
				}
			}


			if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset / preset
				{
				state_chrono = C_STOP;  // chrono stopped
				turn=0; //reset turn number
				SOFT_FLAGS &= ~(BURST1 | BURST2); // clear BURST flags (Burst flags indicate who first reached zero)

				for (i=0; i<2 ;i++)
				{
					SOFT_FLAGS &= ~(CPLAYERS[i].F_CHRONO_RUN);
					*CPLAYERS[i].CHRONO=0;  //if PRESET, clear chrono counter
					*CPLAYERS[i].SCHRONO=0;


					if (SOFT_FLAGS & BIT_RESET) // if RESET clear start and chrono, reset everything to default
						{
						  CPLAYERS[i].TIME_MAIN_SET=DFTIMEMAIN; //if reset default main
						  CPLAYERS[i].TIME_DELAY=DFTIMEDELAY; //default delay
						  CPLAYERS[i].BONUS = 0; //default bonus

						}
					CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value
				}

					SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag

					CHESS_REDRAW(0);

				}


			if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
			{
				SOFT_FLAGS &= ~BIT_REDRAW;
				CHESS_REDRAW(0);
			}

			if (SOFT_FLAGS & BIT_SET)  //Setting mode
				{
				SOFT_FLAGS &= ~BIT_SET;  //clear flag

				SOFT_FLAGS &= ~(BIT_CHRONO_1_RUN | BIT_CHRONO_2_RUN); //stop counters



				state_chrono=C_HUPDATE; //enter main time edit mode

				CHESS_REDRAW(2);   //print setting value

				TIMER_1=500; // flash screen
				}



					for (i=0 ; i < 2 ; i++)
					{
						if (i) j=0; else j=1; //j to point to other player
					//	if ((SYS_FLAGS & CPLAYERS[i].F_KEY) && (ANY_KEY(CPLAYERS[i].F_KEY))) // check player key pressed to adjust  time or change chrono for other player
						if (SYS_FLAGS & CPLAYERS[i].F_KEY) // check player key pressed to adjust  time or change chrono for other player
						{

								switch (state_chrono)
								{
								case C_STOP:  // start other player counter
									SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off
								case C_P1RUN:
								case C_P2RUN:
								case C_P1DELAY:
								case C_P2DELAY:
									if (((state_chrono==C_P1RUN) || (state_chrono==C_P1DELAY))&&(!(ANY_KEY(BIT_KEY_P1)))) break; //exit if P2 pressed again
									if (((state_chrono==C_P2RUN) || (state_chrono==C_P2DELAY))&&(!(ANY_KEY(BIT_KEY_P2)))) break; //exit if P1 pressed again
									//BUZZER (333);
									turn++; //increment turn number
									if ((state_chrono==C_P2DELAY) && (CPLAYERS[1].TIME_MAIN > *CPLAYERS[1].SCHRONO)) CPLAYERS[1].TIME_MAIN += 1000 * CPLAYERS[1].BONUS; //add bonus
									if ((state_chrono==C_P2RUN) && (CPLAYERS[1].TIME_MAIN > *CPLAYERS[1].SCHRONO)) CPLAYERS[1].TIME_MAIN += 1000 * CPLAYERS[1].BONUS; //add bonus
									if ((state_chrono==C_P1DELAY) && (CPLAYERS[0].TIME_MAIN > *CPLAYERS[0].SCHRONO)) CPLAYERS[0].TIME_MAIN += 1000 * CPLAYERS[0].BONUS; //add bonus
									if ((state_chrono==C_P1RUN ) && (CPLAYERS[0].TIME_MAIN > *CPLAYERS[0].SCHRONO)) CPLAYERS[0].TIME_MAIN += 1000 * CPLAYERS[0].BONUS; //add bonus
									CHESS_REDRAW(0);
									if (CPLAYERS[j].TIME_DELAY)  //if delay configured, enter delay timing
									{

										font=alpha_lg;   // write delay
										bkground=BLACK;
										frground=WHITE;

										if (i){
											SOFT_FLAGS &= ~BIT_CHRONO_2_RUN; //P2 must stop
											justify=LEFT;
											LCD_COLORWINDOW (0, 15, 55, 126, BLACK); //clear screen side of p1
											LCD_WORD ("DLY",0,60,3);

											state_chrono=C_P1DELAY;
											font=alpha_lg;
											frground=PINK;
											CHR_DELAY_SS (CPLAYERS[0].TIME_DELAY, CPLAYERS[0].X + 52);
											auxdelay=CPLAYERS[0].TIME_DELAY; //...decrementing auxdelay


										}
										else {
											SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //P1 must stop
											justify=RIGHT;
											LCD_COLORWINDOW (105, 15, 159, 126, BLACK); //clear screen side of p2
										LCD_WORD ("DLY",110,60,3);

										state_chrono=C_P2DELAY;
										font=alpha_lg;
										frground=PINK;
										CHR_DELAY_SS (CPLAYERS[1].TIME_DELAY, CPLAYERS[1].X + 52);
										auxdelay=CPLAYERS[1].TIME_DELAY; //...decrementing auxdelay

										}

											TIMER_2 = 1000;  //star delay using T2 to count 1 seconds..

								}
									else
									{
									if (i)  // start P1
									{
										SOFT_FLAGS |= BIT_CHRONO_1_RUN;
										SOFT_FLAGS &= ~BIT_CHRONO_2_RUN; //P2 must stop
										state_chrono = C_P1RUN;
									}
									else // start P2
									{
										SOFT_FLAGS |= BIT_CHRONO_2_RUN;
										SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //P1 must stop
										state_chrono = C_P2RUN;

									}
 }
									break;

								case C_HUPDATE:  //edit hour
									CHRONO_AUX= CPLAYERS[i].TIME_MAIN_SET + 3600000*GET_KEY_VALUE(CPLAYERS[i].F_KEY); // Add HOURS..
									if  (CHRONO_AUX < 35999001) CPLAYERS[i].TIME_MAIN_SET = CHRONO_AUX; //.. if in range
									frground=CPLAYERS[i].PLAY_COLOR;
									CHR_HMMSS_V (CPLAYERS [i].TIME_MAIN_SET, CPLAYERS[i].X + 52);

									CHRONO_1=0;  //something edited so must reset counters
									SCHRONO_1=0;
									CHRONO_2=0;
									SCHRONO_2=0;
									turn=0; //reset turn number
									CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value

									break;

								case C_MUPDATE:  //edit minutes
									CHRONO_AUX= CPLAYERS[i].TIME_MAIN_SET + 60000*GET_KEY_VALUE(CPLAYERS[i].F_KEY); // Add minutes..
									if  (CHRONO_AUX < 35999001) CPLAYERS[i].TIME_MAIN_SET = CHRONO_AUX; //.. if in range
									frground=CPLAYERS[i].PLAY_COLOR;
									CHR_HMMSS_V (CPLAYERS [i].TIME_MAIN_SET, CPLAYERS[i].X + 52);
									CHRONO_1=0;  //something edited so must reset counters
									SCHRONO_1=0;
									CHRONO_2=0;
									SCHRONO_2=0;
									turn=0; //reset turn number
									CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value
									break;

								case C_SUPDATE:  //edit seconds
									CHRONO_AUX= CPLAYERS[i].TIME_MAIN_SET + 1000*GET_KEY_VALUE(CPLAYERS[i].F_KEY); // Add seconds..
									if  (CHRONO_AUX < 35999001) CPLAYERS[i].TIME_MAIN_SET = CHRONO_AUX; //.. if in range
									frground=CPLAYERS[i].PLAY_COLOR;
									CHR_HMMSS_V (CPLAYERS [i].TIME_MAIN_SET, CPLAYERS[i].X + 52);
									CHRONO_1=0;  //something edited so must reset counters
									SCHRONO_1=0;
									CHRONO_2=0;
									SCHRONO_2=0;
									turn=0; //reset turn number
									CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value

									break;

								case C_BUPDATE:  //edit bonus
									CHRONO_AUX= CPLAYERS[i].BONUS + GET_KEY_VALUE(CPLAYERS[i].F_KEY); // Add seconds to bonus..
									if  (CHRONO_AUX < 100) CPLAYERS[i].BONUS= CHRONO_AUX; //.. if in range
									frground=CPLAYERS[i].PLAY_COLOR;
									CHR_DELAY_SS (CPLAYERS[i].BONUS, CPLAYERS[i].X + 52);
									CHRONO_1=0;  //something edited so must reset counters
									SCHRONO_1=0;
									CHRONO_2=0;
									SCHRONO_2=0;
									turn=0; //reset turn number
									CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value

									break;

								case C_DUPDATE:  //edit delay
									CHRONO_AUX= CPLAYERS[i].TIME_DELAY + GET_KEY_VALUE(CPLAYERS[i].F_KEY); // Add seconds to delay..
									if  (CHRONO_AUX < 100) CPLAYERS[i].TIME_DELAY = CHRONO_AUX; //.. if in range
									frground=CPLAYERS[i].PLAY_COLOR;
									CHR_DELAY_SS (CPLAYERS[i].TIME_DELAY, CPLAYERS[i].X + 52);
									CHRONO_1=0;  //something edited so must reset counters
									SCHRONO_1=0;
									CHRONO_2=0;
									SCHRONO_2=0;
									turn=0; //reset turn number
									CPLAYERS[i].TIME_MAIN=CPLAYERS[i].TIME_MAIN_SET; //and preset main value to setting value
									break;

								}

								SYS_FLAGS &= ~ CPLAYERS[i].F_KEY; //reset player  KEY flag
								}
					}


					if (SYS_FLAGS & BIT_TIMER_1)   // flash entry to indicate editing (cursor like)
					{
						SYS_FLAGS &= ~BIT_TIMER_1; //clear timer 1 alarm
						TIMER_1=500; //prepare next flash
						bkground=BLACK;
					SOFT_FLAGS^=BIT_AUX; //invert color
						if (SOFT_FLAGS & BIT_AUX) frground=WHITE;  //invert color
						else frground=GREY1;
						font=alpha_lg;   // write H MM SS DELAY

							switch (state_chrono)
							{
							case C_HUPDATE:
								LCD_WORD ("H",72,19,1);

								break;

							case C_MUPDATE:
								LCD_WORD ("MM",64,54,2);

								break;

							case C_SUPDATE:
							   	LCD_WORD ("SS",64,89,2);

								break;

							case C_DUPDATE:

								LCD_WORD ("DELAY",40,19,1);
								break;

							case C_BUPDATE:

								LCD_WORD ("BONUS",40,19,1);
								break;

							}

					}


					if (SYS_FLAGS & BIT_TIMER_2)   // process delay
					{
						SYS_FLAGS &= ~BIT_TIMER_2; //clear timer 2 alarm
						if ((state_chrono==C_P1DELAY) || (state_chrono==C_P2DELAY)) // check if state is delay
										{
											i = (state_chrono == C_P1DELAY) ? 0 : 1;


												if (--auxdelay == 0)   //reach 0 start main time and buzz
												{
													BUZZER (333);

													CHESS_REDRAW(0);

													if (i)  // start P1
													{
														SOFT_FLAGS |= BIT_CHRONO_2_RUN;
														SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //P1 must stop
														state_chrono = C_P2RUN;


													}
													else // start P2
													{
														SOFT_FLAGS |= BIT_CHRONO_1_RUN;
														SOFT_FLAGS &= ~BIT_CHRONO_2_RUN; //P2 must stop
														state_chrono = C_P1RUN;
													}
												}
												else  // if delay not 0, print delay
												{	TIMER_2=1000; //restart T2
													bkground=BLACK;
													frground=PINK;  //print delay in seconds
													CHR_DELAY_SS (auxdelay, CPLAYERS[i].X + 52); //print remaing delay
												}
										}
					}


					if ((state_chrono==C_P1RUN) || (state_chrono==C_P2RUN)) //process chrono alarm
					{
						i = (state_chrono == C_P1RUN) ? 0 : 1;

						if (SYS_FLAGS & CPLAYERS[i].F_ALARM)   // chrono alarm for player
						{
							SYS_FLAGS &= ~CPLAYERS[i].F_ALARM; //clear flag

							if (CPLAYERS[i].TIME_MAIN == *CPLAYERS[i].SCHRONO) {  //reach 0 start up counting
								BUZZER (7000);
								if ((SOFT_FLAGS  & (BURST1 | BURST2)) == 0)  // if no player reached 0 before...
								{
									if (i) SOFT_FLAGS|=BURST1;   //..then set this player first to burst flag to 1
									else
										SOFT_FLAGS|=BURST2;
								}
							DRAWFLAG(i,RED); //SET red flag
							}
							bkground=BLACK;
							frground=CPLAYERS[i].PLAY_COLOR;
							if (CPLAYERS[i].TIME_MAIN > *CPLAYERS[i].SCHRONO)  // counting down
							{
								if (CPLAYERS[i].TIME_MAIN - *CPLAYERS[i].SCHRONO < 11000) //if less than 10 s beep
								{
									BUZZER (250);
									SOFT_FLAGS^=BIT_AUX; //flash flag
									if (SOFT_FLAGS & BIT_AUX) DRAWFLAG(i,RED);
									else DRAWFLAG(i,BLACK);
								}
								CHR_HMMSS_V (CPLAYERS [i].TIME_MAIN - *CPLAYERS[i].SCHRONO , CPLAYERS[i].X + 52); //print remaing time (downcounting)
							}
							else
								CHR_HMMSS_V (*CPLAYERS[i].SCHRONO - CPLAYERS [i].TIME_MAIN, CPLAYERS[i].X + 52); //print upcounting time


						}
					}

	} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

	TOILET ();


}

void RTIMER (void)
{
	CHRONO_1=0; //reset chrononometer
	SCHRONO_1=0;
	CHRONO_1_ALARM = 500; //CHRONO_1 generates interrupt every 500x1ms= 0.5s
	CHRONO_AUX=0; // var for the start time
	bgcolor=GREEN2;// initial bargraph color
	HELP=HELP_TIMER; //set help file


	SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
	state_chrono = C_STOP;  // begin program with chrono stopped

	SOFT_FLAGS |= BIT_KREPEAT; // set player key repeat on


	RT_REDRAW();




	do {
		 GO_SLEEP();	//wait something happens

		 if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

			if (SYS_FLAGS & BIT_KEY_M) // a function key is pressed
				{
				if (state_chrono==C_UPDATE)  //if updating, will exit update
				{
					if (ANY_KEY(BIT_KEY_M))
					{
						state_chrono=C_STOP; //exit start time edit mode
						bkground=BLACK;
						frground=YELLOW;
						LCD_COLORWINDOW (0, 0, 159, 91, BLACK); //clear edition area
						SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
						TIMER_1=0; //stop time out timer
						CHRONO_1=0; // reset
						SCHRONO_1=0;
						RT_REDRAW();
					}
				}
				else
				{
					if ((GET_KEY (BIT_KEY_M))==5)  // play key pressed
					{
						SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag
						if (CHRONO_AUX)  // if there is a start value set, start counting
						{
							SOFT_FLAGS ^= BIT_CHRONO_1_RUN; //start or stop chrono
							if (SOFT_FLAGS & BIT_CHRONO_1_RUN)
							{
								state_chrono = C_RUN; //state running
								BUZZER (3000);
							}
							else state_chrono = C_STOP; //state stopped
						}
						else // if there is not a start value, then flash Start value
						{
							BUZZER (4000);
							bkground=BLACK;
							for (i=0; i<30; i++)
							{
									bkground=YELLOW;
									frground=BLACK;
									CHR_STARTIME (CHRONO_AUX);
									frground=GREY2;
									bkground=BLACK;
									CHR_STARTIME (CHRONO_AUX);
							}
						}
					}
					else	APP_FUNCTION_MAIN(); //other function key pressed
				}
				}

			if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
				{

				CHRONO_1=0; //set chrononometer to start value
				SCHRONO_1=0;
				bgcolor=GREEN2;// initial bargraph color

					SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
					state_chrono = C_STOP;  // chrono stopped

					if (SOFT_FLAGS & BIT_RESET) // if RESET clear start and chrono, if PRESET clear chrono only
					{
						CHRONO_AUX=0;

					}


					SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag

					RT_REDRAW();

				}


			if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
			{
				SOFT_FLAGS &= ~BIT_REDRAW;
				RT_REDRAW();
			}

			if (SOFT_FLAGS & BIT_SET)  //Setting mode
				{
				SOFT_FLAGS &= ~BIT_SET;  //clear flag

				SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;

				state_chrono=C_UPDATE; //enter start time edit mode
				bkground=BLACK;
				frground=YELLOW;
				LCD_COLORWINDOW (0, 0, 159, 91, BLACK); //clear edition area
				bgcolor=GREEN2;// initial bargraph color


				CHR_HHMMSS (CHRONO_AUX,50); // print actual start value

				CHRONO_1=0;   //restart chrono1 just to flash screen while editing
				SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start chrono

				TIMER_1=7000; //will exit edit mode after 7s time out
				}

			if (SYS_FLAGS & (BIT_KEY_P1 | BIT_KEY_P2)) // check player key pressed to adjust start time
			{
				SYS_FLAGS &= ~ (BIT_KEY_P1 | BIT_KEY_P2); //reset player  KEY flag
				if (ANY_KEY (BIT_KEY_P1 | BIT_KEY_P2)) //yes! a player key was pressed
				{
					switch (state_chrono) //if running when pressing P1 or P2 -> restart with initial value
					{
						case C_RUN:
					//		BUZZER (3000); //error, can not update start time if running
					//		 for (i=0; i<3; i++) // flash counter
					//		 {
					//				bkground=YELLOW;
					//				frground=BLACK;
					//				CHR_HHMMSS (CHRONO_AUX-SCHRONO_1+999,0); //Print CHRONO_1 as HHMMSS
					//				bkground=BLACK;
					//				frground=PINK;
					//				CHR_HHMMSS (CHRONO_AUX-SCHRONO_1+999,0); //Print CHRONO_1 as HHMMSS
					//		 }

							BUZZER (1500);
							CHRONO_1=0; //set chrononometer to start value
							SCHRONO_1=0;
							bgcolor=GREEN2;// initial bargraph color


							break;

						case C_STOP:
							if (CHRONO_AUX)  // if there is a start value set, start counting
							{
								BUZZER (3000);
								SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start chrono

									state_chrono = C_RUN; //state running

							}
							else // if there is not a start value, then flash Start value
							{
								BUZZER (4000);
								bkground=BLACK;
								for (i=0; i<30; i++)
								{
										bkground=YELLOW;
										frground=BLACK;
										CHR_STARTIME (CHRONO_AUX);
										frground=GREY2;
										bkground=BLACK;
										CHR_STARTIME (CHRONO_AUX);
								}
							}
							break;

	//					case C_STOP:
	//						state_chrono=C_UPDATE; //enter start time edit mode
	//						bkground=BLACK;
	//						frground=YELLOW;
	//						LCD_COLORWINDOW (0, 0, 159, 91, BLACK); //clear edition area
//
	//						CHR_HHMMSS (CHRONO_AUX,50); // print actual start value
//
	//						CHRONO_1=0;   //restart chrono1 just to flash screen while editing
		//					SOFT_FLAGS |= BIT_CHRONO_1_RUN; //start chrono
//
	//						TIMER_1=7000; //will exit edit mode after 7s time out
		//					break;

						case C_UPDATE:
							TIMER_1=7000; //will exit edit mode after 7s time out
							CHRONO_AUX +=(GET_KEY_VALUE(BIT_KEY_P1)*60000+GET_KEY_VALUE(BIT_KEY_P2)*1000); //update initial value according to pressed key
							if (CHRONO_AUX > 359999000) CHRONO_AUX=0; // keep value between 0 and 99h59m59s

							bkground=BLACK;
							frground=YELLOW;
							CHR_HHMMSS (CHRONO_AUX,50); // print actual start value

							break;
					}

				}

			}

			if (SYS_FLAGS & BIT_CHRONO_1_ALARM)   // Print CHRONO
				{
				SYS_FLAGS &= ~BIT_CHRONO_1_ALARM;
				//bgcolor=GREEN2;
				switch (state_chrono) {
					case C_RUN: // if running, check, print counting and update bargraph
						bkground=BLACK;
						frground=PINK;
						if ((CHRONO_AUX-SCHRONO_1+999)<11000)  //if less than 10 s beep once
						{   bgcolor=YELLOW;
							if (!((SCHRONO_1/100)%10))
								BUZZER (250);
							if ((CHRONO_AUX-SCHRONO_1+999)<6000) //if less than 5 s beep twice
							{bgcolor=RED;
								if ((SCHRONO_1/100)%10)
									BUZZER (250);
							}
						}

					CHR_HHMMSS (CHRONO_AUX-SCHRONO_1+999,0); //Print regressive counting as HHMMSS


						BARGRAPH (SCHRONO_1,CHRONO_AUX,bgcolor, GREY0, BLACK); //print bargraph

						if (!(CHRONO_AUX > SCHRONO_1))  //TIME OUT!!!
						{
							SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
									state_chrono = C_STOP; //state running
									CHRONO_1=0; //zero chrono
									SCHRONO_1=0;
									BUZZER (7000);

									for (i=0; i <10 ; i++)
									{
										bkground=BLACK;
										frground=YELLOW;
										CHR_HHMMSS (0,0); // print 0
										BARGRAPH (CHRONO_AUX,CHRONO_AUX,BLACK, GREY0, BLACK); //print bargraph
										bkground=BLACK;
										frground=PINK;
										CHR_HHMMSS (0,0); // print 0
										BARGRAPH (CHRONO_AUX,CHRONO_AUX,GREEN2, GREY0, BLACK); //print bargraph
									}
									bgcolor=GREEN2;
						}





						break;

					case C_UPDATE: //if updating, flash count
						if ((SCHRONO_1/100)%10) frground=YELLOW;
						else frground=GREY1;

						bkground=BLACK;
						CHR_HHMMSS (CHRONO_AUX,50); // print actual start value

						break;
				}

				}


			if (SYS_FLAGS & BIT_TIMER_1)  //start time edition time out
				{
				SYS_FLAGS ^= BIT_TIMER_1; //clear flag
				if (state_chrono == C_UPDATE)
				{
					state_chrono=C_STOP; //exit start time edit mode
					bkground=BLACK;
					frground=YELLOW;
					LCD_COLORWINDOW (0, 0, 159, 91, BLACK); //clear edition area
					SOFT_FLAGS &= ~BIT_CHRONO_1_RUN; //stop chrono
					CHRONO_1=0; // reset
					SCHRONO_1=0;
					RT_REDRAW();

				}

				}

	} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

	TOILET ();

}


void CHRONOGRAPH (void) {

CHRONO_1=0; //reset chrononometer
SCHRONO_1=0;
CHRONO_1_ALARM = 500; //CHRONO_1 generates interrupt every 500x1ms= 0.5s
lapmode=ABSOLUTE;  //when press player key, store absolute value
HELP=HELP_CHRONO; //set help file


for (i = 0; i < 6; ++i)  //prepare lap marks memory
{
	CHRON[i].CHRON=0;
	CHRON[i].LAP=0;
}

SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
state_chrono = C_STOP;  // begin program with chrono stopped


CHR_REDRAW();

WAIT_NO_KEY (); //wait no key pressed
SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off

do {
	 GO_SLEEP();	//wait something happens


	if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

	if (SYS_FLAGS & BIT_KEY_M)
		{
		 if ((GET_KEY (BIT_KEY_M))==5)  // play key pressed
		 {
			 SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag
			 SOFT_FLAGS ^= BIT_CHRONO_1_RUN; //start or stop chrono
			 if (SOFT_FLAGS & BIT_CHRONO_1_RUN)
			 {
				 state_chrono = C_RUN; //state running
				 BUZZER (3000);
			 }
			 else state_chrono = C_STOP; //state stopped
		 }
		 else	APP_FUNCTION_MAIN(); //other function key pressed
		}

	if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
	{
		CHRONO_1=0; //reset chrononometer
		SCHRONO_1=0;


		SOFT_FLAGS &= ~BIT_CHRONO_1_RUN;
		state_chrono = C_STOP;  // chrono stopped

		if (SOFT_FLAGS & BIT_RESET) // if RESET clear laps and chrono, if PRESET clear chrono only
		{
			for (i = 0; i < 6; ++i)  //reset lap marks memory
			{
				CHRON[i].CHRON=0;
				CHRON[i].LAP=0;
			}
		}

		SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag

		CHR_REDRAW();

	}

	
	if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
	{
		SOFT_FLAGS &= ~BIT_REDRAW;
		CHR_REDRAW();
	}

	if (SOFT_FLAGS & BIT_SET)  //Setting mode
		{
		SOFT_FLAGS &= ~BIT_SET;  //clear flag

		switch (SELECTMENU(LAPSET,GREY2,BLACK)) {
			case 1:
				lapmode=ABSOLUTE; // memory displayed in absolute form
			break;
			case 2:
				lapmode=DIFFERENTIAL; // memory displayed in diff form
			break;

		}
		CHR_REDRAW();
		}

	if (SYS_FLAGS & (BIT_KEY_P1 | BIT_KEY_P2)) // check player key pressed then update lap
	{
		SYS_FLAGS &= ~ (BIT_KEY_P1 | BIT_KEY_P2); //reset player  KEY flag
		if (ANY_KEY (BIT_KEY_P1 | BIT_KEY_P2)) //yes! a player key was pressed
		{
			for (i = 5; i > 0; --i)  //update lap marks memory
			{
				CHRON[i].CHRON=CHRON[i-1].CHRON;
				CHRON[i].LAP=CHRON[i-1].LAP;
			}
			CHRON[0].CHRON=SCHRONO_1;
			CHRON[0].LAP=CHRON[1].LAP + 1;
			bkground=BLACK;
			frground=GREY2;
			CHR_LAPS();
		}

	}


	if (SYS_FLAGS & BIT_CHRONO_1_ALARM)   // Print CHRONO_1
		{
		bkground=BLACK;
		frground=YELLOW;
		if (CHRONO_1 > 359999000)
			{CHRONO_1 = 0; //99H59M59S the maximum count
			SCHRONO_1=0;
			}
		CHR_HHMMSS (SCHRONO_1,0); //Print CHRONO_1 as HHMMSS

		LCD_NUMBER (V_BATT*5,120,80,0);   //* FOR TEST ONLY!!  used to display battery voltage in mV while cronometer is running

		SYS_FLAGS &= ~BIT_CHRONO_1_ALARM;
		}





} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

TOILET ();

}

void DRAWFLAG (char player, unsigned int color)
{
	LCD_FRAME (0,0,79,13,WHITE); //draw flag box

	LCD_FRAME (80,0,159,13,WHITE);

	if (player) LCD_COLORWINDOW (81,1,158,12,color);
	else LCD_COLORWINDOW (1,1,78,12,color);

	if (SOFT_FLAGS & BURST2)  //if player was the first to reach zero, then draw flag (1)
	{
		yh1=frground;  //save frground and bkground
		yh2=bkground;
		font=numbrtny;
		justify=CENTER;
		frground=BLACK;
		bkground=RED;
		LCD_NUMBER (1,39,2,1);
		frground=yh1;
		bkground=yh2;
	}

	if (SOFT_FLAGS & BURST1)  //if player was the first to reach zero, then draw flag (1)
	{
		yh1=frground;
		yh2=bkground;
		font=numbrtny;
		justify=CENTER;
		frground=BLACK;
		bkground=RED;
		LCD_NUMBER (1,119,2,1);
		frground=yh1;
		bkground=yh2;
	}
}

void BARGRAPH ( unsigned long int num, unsigned long int max, unsigned int LED_ON, unsigned int LED_OFF, unsigned int BCKGRND)
// will draw a bargraph with num/max * 20 LEDs on. Colors LED_ON, LED_OFF, BCKGRND
{   unsigned long int b;
	//LCD_COLORWINDOW (0, 92, 159, 125, BCKGRND); //clear bargraph

	//if (max) { b=((num*20)+max/2)/max; } //number of lit leds
	if (max) { b=(num*20)/max; } //number of lit leds
	else  {b=20;}

	for (char a=0; a<b; a++)
	{
		LCD_COLORWINDOW (a*8,92, a*8+6,124, LED_ON);
	}
	for (char a=b; a< 20; a++)
	{
		LCD_COLORWINDOW (a*8,92, a*8+6,124, LED_OFF);
	}

}

void CHESS_REDRAW(char mode) 	//redraw chess clock
								// mode = 0 -> show running time
								// mode =1 -> show main time including Added bonus
								// mode =2 -> show main time (set time)
{
	LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear screen

	DRAWFLAG(0,BLACK); //draw flag box

	DRAWFLAG(1,BLACK);


	font=nmbrSS;   // print the turn number
	bkground=BLACK;
	frground=GREEN2;
	justify=CENTER;
	LCD_NUMBER ((((turn*10)/2)+9)/10,68, 115,  3);

	font=alpha_lg;   // write H MM SS
	bkground=BLACK;
	frground=WHITE;
	justify=LEFT;

	LCD_WORD ("H",72,19,1);
	LCD_WORD ("MM",64,54,2);
	LCD_WORD ("SS",64,89,4);

	for (int j=0; j<2 ;j++)		//print both players time
	{

			frground=CPLAYERS [j].PLAY_COLOR;
			if (mode==1)   // print setting time + Bonnus

				CHR_HMMSS_V (CPLAYERS [j].TIME_MAIN, CPLAYERS[j].X + 52);

			 if (mode==0) 		// print running time
			{
				if (CPLAYERS [j].TIME_MAIN > *CPLAYERS[j].SCHRONO)


					CHR_HMMSS_V (CPLAYERS [j].TIME_MAIN - *CPLAYERS[j].SCHRONO, CPLAYERS[j].X + 52);

				else
				{
					CHR_HMMSS_V (*CPLAYERS[j].SCHRONO - CPLAYERS [j].TIME_MAIN  , CPLAYERS[j].X + 52);
				DRAWFLAG (j,RED);
				}
			}

				if (mode==2)   // print setting time

					CHR_HMMSS_V (CPLAYERS [j].TIME_MAIN_SET, CPLAYERS[j].X + 52);



	}





}

void RT_REDRAW (void) {
	LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear screen

	bkground=BLACK;
	frground=PINK;
	CHR_HHMMSS (CHRONO_AUX-CHRONO_1+999,0); //Print time lasting as HHMMSS

	LCD_LINE (0,41,159,41,WHITE); //draw a separator line

	BARGRAPH (SCHRONO_1,CHRONO_AUX,bgcolor, GREY0, BLACK); //print bargraph

	bkground=BLACK;
	frground=GREY2;
	font=nmbrSS;
	justify=RIGHT;
	CHR_STARTIME ( CHRONO_AUX); // display start time as HH MM SS
	LCD_NUMBER (HOR,110, 75,  2);
	if (HOR<10) LCD_NUMBER(0,101,75,1); //print a 0 if hour < 10

	LCD_NUMBER (MIN,134, 75,  2);
	if (MIN<10) LCD_NUMBER(0,125,75,1); //print a 0 if min < 10

	LCD_NUMBER (SEC,158, 75,  2);
	if (SEC<10) LCD_NUMBER(0,149,75,1); //print a 0 if min < 10

}

void CHR_REDRAW (void) {

	LCD_COLORWINDOW (0, 0, 159, 126, BLACK); //clear screen


	bkground=BLACK;
	frground=YELLOW;
	CHR_HHMMSS (CHRONO_1,0); //Print CHRONO_1 as HHMMSS

	LCD_LINE (0,41,159,41,WHITE); //draw a separator line

	bkground=BLACK;
	frground=GREY2;
	CHR_LAPS(); //print lap marks

}

void CHR_STARTIME (unsigned long int CHRONO) // display start time as HH MM SS
{
	CNT_TO_HMS(&HOR, &MIN, &SEC, CHRONO);
	LCD_NUMBER (HOR,110, 75,  2);
	if (HOR<10) LCD_NUMBER(0,101,75,1); //print a 0 if hour < 10

	LCD_NUMBER (MIN,134, 75,  2);
	if (MIN<10) LCD_NUMBER(0,125,75,1); //print a 0 if min < 10

	LCD_NUMBER (SEC,158, 75,  2);
	if (SEC<10) LCD_NUMBER(0,149,75,1); //print a 0 if min < 10
}

void CHR_DELAY_SS (unsigned char DELAY, unsigned char x) // display delay SS on colum x
{

	font=nmbrXL;
	justify=RIGHT;


	if (DELAY<10)
	{
		LCD_NUMBER (DELAY, x, 86, 1);   // :SS
		LCD_NUMBER(0,x-26,86,1); //print a 0 if DELAY < 10
	}
	else LCD_NUMBER (DELAY, x, 86, 2);  // :SS


}


void CHR_MMSS (unsigned long int CHRONO, unsigned char x, unsigned char y, char size) // display CHRONO as MM:SS on x, y. size: 0->samll 1->big
{
 	justify=RIGHT;
	CNT_TO_HMS(&HOR, &MIN, &SEC, CHRONO);

if (size)
{


		LCD_COLORWINDOW (53+x,y+10,57+x,y+14,frground); //dots
		LCD_COLORWINDOW (53+x,y+24,57+x,y+28,frground); //dots


		font=nmbrXL;
		if (MIN<10)
		{
			LCD_NUMBER (MIN, 51+x, y, 1);  // MM:
			LCD_NUMBER(0,25+x,y,1); //print a 0 if min < 10
		}
		else LCD_NUMBER (MIN, 51+x, y, 2);  // :MM:

		if (SEC<10)
		{
			LCD_NUMBER (SEC, 113+x, y, 1);   // :SS
			LCD_NUMBER(0,87+x,y,1); //print a 0 if sec < 10
		}
		else LCD_NUMBER (SEC, 113+x, y, 2);  // :SS





}
else
{
	font=nmbrSS;
	if (MIN<10)
	{
		LCD_NUMBER (MIN, x+9, y, 1);  // MM:
		LCD_NUMBER(0,x,y,1); //print a 0 if hour < 10
	}
	else LCD_NUMBER (MIN, x+9, y, 2);  // HH:
		LCD_COLORWINDOW (x+12,y+2,x+13,y+3,frground); //dots
		LCD_COLORWINDOW (x+12,y+8,x+13,y+9,frground); //dots
	if (SEC<10)
	{
		LCD_NUMBER (SEC, x+33, y, 1);   // :SS
		LCD_NUMBER(0,x+24,y,1); //print a 0 if sec < 10
	}
	else LCD_NUMBER (SEC, x+33, y, 2);  // :SS
}

}


void CHR_HMMSS_V (unsigned long int CHRONO, unsigned char x) // display CHRONO vertically as H:MM:SS on colum x
{

	font=nmbrL;
	justify=RIGHT;

	CNT_TO_HMS(&HOR, &MIN, &SEC, CHRONO);

	if (HOR>9) HOR=0;


		LCD_NUMBER (HOR, x-17, 15, 1);  // H

		font=nmbrXL;

	if (MIN<10)
	{
		LCD_NUMBER (MIN, x, 44, 1);  // MM:
		LCD_NUMBER(0,x-26,44,1); //print a 0 if min < 10
	}
	else LCD_NUMBER (MIN, x, 44, 2);  // :MM:

	if (SEC<10)
	{
		LCD_NUMBER (SEC, x, 86, 1);   // :SS
		LCD_NUMBER(0,x-26,86,1); //print a 0 if sec < 10
	}
	else LCD_NUMBER (SEC, x, 86, 2);  // :SS


}


void CHR_HHMMSS (unsigned long int CHRONO, unsigned char y) // display CHRONO as HH:MM:SS on line y
{

	font=nmbrL;
	justify=RIGHT;

	CNT_TO_HMS(&HOR, &MIN, &SEC, CHRONO);

	if (HOR>99) HOR=0;

	if (HOR<10)
	{
		LCD_NUMBER (HOR, 36, y+12, 1);  // HH:
		LCD_NUMBER(0,18,y+12,1); //print a 0 if hour < 10
	}
	else LCD_NUMBER (HOR, 36, y+12, 2);  // HH:

	if (CHRONO_1/100 % 10) {LCD_COLORWINDOW (38,y+14,42,y+18,frground); //dots
	LCD_COLORWINDOW (101,y+14,105,y+18,frground); //dots
	LCD_COLORWINDOW (38,y+28,42,y+32,GREY1); //dots
	LCD_COLORWINDOW (101,y+28,105,y+32,GREY1); //dots
	} else {
		LCD_COLORWINDOW (38,y+14,42,y+18,GREY1); //dots
			LCD_COLORWINDOW (101,y+14,105,y+18,GREY1); //dots
			LCD_COLORWINDOW (38,y+28,42,y+32,frground); //dots
			LCD_COLORWINDOW (101,y+28,105,y+32,frground); //dots
	}

	font=nmbrXL;
	if (MIN<10)
	{
		LCD_NUMBER (MIN, 98, y, 1);  // MM:
		LCD_NUMBER(0,72,y,1); //print a 0 if min < 10
	}
	else LCD_NUMBER (MIN, 98, y, 2);  // :MM:

	if (SEC<10)
	{
		LCD_NUMBER (SEC, 160, y, 1);   // :SS
		LCD_NUMBER(0,134,y,1); //print a 0 if sec < 10
	}
	else LCD_NUMBER (SEC, 160, y, 2);  // :SS


}

void CHR_LAPS (void)  //print lap marks
{
	unsigned char y;

	font=nmbrSS;
	justify=RIGHT;

	for (i = 0; i < 5; ++i) // print laps marks if !=0
	{
		if (CHRON[i].LAP)  // not empty, so print
		{
			if (lapmode==DIFFERENTIAL)
			{
				if (CHRON[i].CHRON > CHRON[i+1].CHRON)   // in case of overlap will be negative
					CHRONO_AUX=CHRON[i].CHRON - CHRON[i+1].CHRON;
				else
					CHRONO_AUX =CHRON[i].CHRON;
			}
			else  CHRONO_AUX =CHRON[i].CHRON;

			CNT_TO_HMS(&HOR, &MIN, &SEC, CHRONO_AUX);
			y=44+i*17;
			LCD_NUMBER (CHRON[i].LAP,21, y,  2);
			LCD_CHAR ('/',22,y);  //print a - separator

			LCD_NUMBER (HOR,54, y,  2);
			if (HOR<10) LCD_NUMBER(0,45,y,1); //print a 0 if hour <  10

			LCD_NUMBER (MIN,78, y,  2);
			if (MIN<10) LCD_NUMBER(0,69,y,1); //print a 0 if min < 10

			LCD_NUMBER (SEC,102, y,  2);
			if (SEC<10) LCD_NUMBER(0,93,y,1); //print a 0 if min < 10


		}

	}
}

void CNT_TO_HMS (unsigned char *HOR, unsigned char *MIN, unsigned char *SEC, unsigned long int CHRONO) // convert counter CHRONO (50ms) to HHH:MM:SS
{
	*HOR = (CHRONO/3600000);
	*MIN = ((CHRONO - *HOR*3600000)/60000);
	*SEC = ((CHRONO - *HOR*3600000 - *MIN*60000)/1000);
}
