#include "main.h"
#include "LCD_ST7735.h"
#include "APP_BINGO.h"
#include "FONT_number_xg.h"
#include "FONT_number_lg.h"
#include "FONT_number_ss.h"
#include "FONT_alpha_lg.h"
#include "FONT_number_tny.h"
#include "FONT_alpha_tny.h"

#include "BMP_DISK64.h"
#include "BMP_DISK32.h"
#include <stdlib.h>

extern unsigned int TIMER_1;  // chronograph 1 (50ms~50days) used to general timing
extern unsigned int SYS_FLAGS;
extern unsigned int SOFT_FLAGS;
unsigned char state_BINGO;
unsigned char state_ROULETTE;
int i,j,k,chipp,released,slot,ball;
char w;
char TOKENS[92]; //bingo tokens

 static char RLT_SLOT[2][40]={{37,0,32,15,19,4,21,2,25,17,34,6,27,13,36,11,30,8,23,10,5,24,16,33,1,20,14,31,9,22,18,29,7,28,12,35,3,26},
		{38,0,28,9,26,30,11,7,20,32,17,5,22,34,15,3,24,36,13,1,0,27,10,25,29,12,8,19,31,18,6,21,33,16,4,23,35,14,2}};// the sequence number of the roulette in 2 versions

char ANY_KEY( unsigned int);
void GO_SLEEP(void);
void DISPLAY_BATT(void);
void LCD_NUMBER (int n, unsigned char x, unsigned char y, unsigned char field);
void APP_FUNCTION_MAIN();
void BUZZER (int pulse);
char GET_KEY ( unsigned int group);
void WAIT_NO_KEY (void);
void LCD_WORD (const char w[], unsigned char x, unsigned char y, unsigned char field);
void LCD_CHAR (unsigned char c, unsigned char x, unsigned char y);
void BG_REDRAW ();
void LCD_DISK(int xc, int yc, int radius, unsigned int color);
void LCD_BMP2 (const unsigned char *FIGURE, unsigned char X, unsigned char Y, unsigned char width, unsigned char height);
void BG_CHIP (int n, int x, int y);
void BG_TABLE();
void BG_INFO() ;
int BG_DRAW();
char SELECTMENU (const char ITEMS[6][11],unsigned int frg, unsigned int bkg);
void RL_REDRAW(void);
void RL_BALL (int x, int y, unsigned int color);
void RL_SLOTS (void);
void RL_SLOTBALL (int nball);
void RL_RUNBALL (int nball);
void TOILET (void);



void BINGO (void)
{

	WAIT_NO_KEY (); //wait no key pressed



	SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off

	do {
		k=SELECTMENU(MBINGO,GREY2,BLACK);//first ask user for the bingo mode

		if (k==2)
			SOFT_FLAGS |= US_MODE; //75 token chips mode
		if (k==4)
			SOFT_FLAGS &= ~US_MODE; //90 token chips mode  (UK mode)
	} while (k==0);   //repeat till user selection

	for (i=0; i<92; i++) TOKENS[i]=0;  //put tokens inside cage
	TOKENS[DRAWN_CHIPS]=0; // tokens[DRAWN_CHIPS] is the number of chip tokens OUTside cage
	TOKENS[TOTAL_CHIPS]=((SOFT_FLAGS & US_MODE) ? USBINGO : UKBINGO); //TOKENKS[TOTAL_CHIPS]= TOTAL NUMBER OF CHIPS

	BG_REDRAW();
	state_BINGO=B_STOP; //initial state


	do {
		GO_SLEEP();	//wait something happens
		rand(); //xxx improve random generator function

		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

		if (SYS_FLAGS & BIT_KEY_M) // a function key is pressed or umpressed
		{
			SYS_FLAGS &= ~ BIT_KEY_M; //reset M KEY flag

			if (!ANY_KEY(BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M)) //if a key was released
			{
				if (state_BINGO==B_SHUFFLE)
				{
					TIMER_1=50;
					state_BINGO=B_RELEASE;
					released=10+rand()%10; //will shuffle futher after key realeased by a random time
				}
			}

			if ((GET_KEY (BIT_KEY_M))==5)  // play key pressed
			{
				switch (state_BINGO) {

					case B_STOP:
					case B_WAIT:
					case B_SHOW:
					case B_INFO:
						BG_TABLE();		//prepare to drawn a chip token
						state_BINGO = B_SHUFFLE;  //star to shuffle chips
						TIMER_1=20;  //this is shuffling speed

						break;


					case B_RELEASE:
						state_BINGO = B_SHUFFLE;  //star to shuffle chips again
						TIMER_1=100;

						break;

				}
			}
			else
			APP_FUNCTION_MAIN();
		}

		if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
			{

			SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag
			for (i=0; i<92; i++) TOKENS[i]=0;  //put tokens inside cage
			TOKENS[DRAWN_CHIPS]=0; // tokens[DRAWN_CHIPS] is the number of chip tokens OUTside cage
			TOKENS[TOTAL_CHIPS]=((SOFT_FLAGS && US_MODE) ? USBINGO : UKBINGO); //TOKENKS[TOTAL_CHIPS]= TOTAL NUMBER OF CHIPS
			BG_REDRAW();
			state_BINGO=B_STOP; //initial state

			}

		if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
		{
			SOFT_FLAGS &= ~BIT_REDRAW;
			BG_REDRAW();

		}



		if (SOFT_FLAGS & BIT_SET)  //Setting mode
			{
			SOFT_FLAGS &= ~BIT_SET;  //clear flag

			k=SELECTMENU(MBINGO,GREY2,BLACK);//enter mode selection menu

			if (k==2)
				SOFT_FLAGS |= US_MODE; //75 token chips mode
			else
			{
				if (k==4)
				SOFT_FLAGS &= ~US_MODE; //90 token chips mode  (UK mode)

			}

			if ((k==2)||(k==4)) //if mode was seleceted, restart game.
			{
			SOFT_FLAGS |= BIT_RESET; //clear all data after setting
			state_BINGO=B_STOP; //initial state
			TIMER_1=1; //force interruption to reset data
			}
			else
			BG_REDRAW();  //Else keep current game

			}


		if (SYS_FLAGS & (BIT_KEY_P1 | BIT_KEY_P2)) // check player key pressed
		{
			SYS_FLAGS &= ~ (BIT_KEY_P1 | BIT_KEY_P2); //reset player  KEY flag

			if (ANY_KEY (BIT_KEY_P1)) //yes! a player1 key  was pressed. Draw a token
			{
				BG_TABLE();		//prepare to drawn a chip token
				state_BINGO = B_SHUFFLE;  //star to shuffle chips
				TIMER_1=20;
			}
			else
			{
			if (ANY_KEY (BIT_KEY_P2)) //yes! a player2 key  was pressed. Show info
			{
				if (state_BINGO != B_RELEASE)  //if not finishing to draw a chip, then show info
				{
					TIMER_1=0;
				BG_INFO(); // show some Info
				}

			}
			else
			{
				if (!ANY_KEY(BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M)) //if a key was released
				{
					if (state_BINGO==B_SHUFFLE)
					{
							TIMER_1=50;
							state_BINGO=B_RELEASE;
							released=10+rand()%10; //will shuffle futher after key realeased by a random time

					}
				}
			}
			}

		}


		if (SYS_FLAGS & BIT_TIMER_1)  //process timeout
			{
			SYS_FLAGS ^= BIT_TIMER_1; //clear flag

			switch (state_BINGO) {
				case B_STOP:

					break;
				case B_WAIT:
					state_BINGO=B_WAIT;
					BG_REDRAW();

					break;
				case B_SHUFFLE:
				//	BG_CHIP(BG_DRAWN(),80,64);  //print any random chip inside cage
					if (TOKENS[DRAWN_CHIPS]<TOKENS[TOTAL_CHIPS])  //check is there is any chip inside cage
					{
						BG_CHIP(BG_DRAW(),80,64); //print a randon token chip
						TIMER_1=20;  //prepare for next draw
					}
					else {
						BG_INFO();
						TIMER_1=2000;
						state_BINGO=B_STOP;
					}

					break;
				case B_RELEASE:
					if (--released != 0)  //shuffle a little more or...
					{

							BG_CHIP(BG_DRAW(),80,64); //print a randon token chip
							TIMER_1=50;  //prepare for next draw

					}

					else  //finaly draw a token chip
					{
						k=BG_DRAW();  //drawn chip
						TOKENS[k]=++TOKENS[DRAWN_CHIPS]; //mark the chip token as drawn and increase the number of drawn chips.
						BG_CHIP(k,80,64); //print the drawn chip
						state_BINGO=B_WAIT;
						BUZZER (100);
						TIMER_1=4000; 	//Show drawn chip for 4 seconds
					}

					break;
				case B_SHOW:

					break;
				case B_INFO:

					break;

			}
			}





	} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

	TOILET ();



}

int BG_DRAW()  //ramdonly choose a token chip inside cage
{
	if (chipp==0) chipp=rand()%(TOKENS[TOTAL_CHIPS]+1); //chipp is a pointer to any chip position inside cage. this routine initiates chipp
	if (TOKENS[TOTAL_CHIPS]!=TOKENS[DRAWN_CHIPS]) i=rand()%(TOKENS[TOTAL_CHIPS]-TOKENS[DRAWN_CHIPS])+1;  //random number of chips to fetch
	do {  //this loop counts number of chips to fetch
		do {	//this loop fetch for next chip inside cage
			if (++chipp>TOKENS[TOTAL_CHIPS]) chipp=1;  //return for first position after last position
		} while (TOKENS[chipp] !=0);
	} while (--i);
	// TOKENS[DRAWN_CHIPS]++; //add one more chip outside cage
	// TOKENS[chipp]=TOKENS[DRAWN_CHIPS]; //indicate that this chip is drawn and when was drawn
	return(chipp); //chipp is the drawn chip
}

void BG_INFO() //print some basic infos. Last 4 draw tokens and number of tokens inside/outside cage
{

	LCD_COLORWINDOW (0, 0, 159, 126, BLACK);  //clear screen
	LCD_COLORWINDOW (0, 60, 159, 61, GREY1);
	frground=YELLOW;
	bkground=BLACK;
	font=alpha_lg;
 	justify=LEFT;
	LCD_WORD ("LAST", 48,0,5);				//write title
	LCD_CHAR('>',0,31);   //print  -> to indicate sequence

	frground=GREEN2;
	LCD_WORD ("OUT?IN@", 24,68,7);
	frground=YELLOW;



	font=nmbrL;	//print last 3 draw token ships
	k=0;
	if (TOKENS[DRAWN_CHIPS]>2) j=TOKENS[DRAWN_CHIPS]-3; else j=0;  //print any token drawn on position > j

		for (i=1; i <TOKENS[TOTAL_CHIPS]+1; ++i) {  //loop  to find the last 3 chips
			if (TOKENS[i]>j)
			{
			LCD_NUMBER (i,(TOKENS[DRAWN_CHIPS]-TOKENS[i])*50+20,26,2);  //print last 3 chips
			if (++k == 3) i=100;  // brake the loop if already found the 3 chips
			}
	}

				bkground=BLACK;  //print number of chips inside and outside  cage
				frground=GREEN2;
			 	justify=CENTER;

				LCD_NUMBER (TOKENS[TOTAL_CHIPS]-TOKENS[DRAWN_CHIPS],85,94,2);
				LCD_NUMBER (TOKENS[DRAWN_CHIPS],30,94,2);

				WAIT_NO_KEY (); //keep this screen til key release
				BG_REDRAW ();

}

void BG_TABLE()  //prepare the table to draw another chip token

{
	LCD_COLORWINDOW (0,0,159,125,BLACK);  //draw the BINGO TABLE
	LCD_COLORWINDOW (0,0,10,125,SWOOD);
	LCD_COLORWINDOW (159,125,149,0,SWOOD);
	LCD_COLORWINDOW (0,0,159,10,SWOOD);
	LCD_COLORWINDOW (0,115,159,125,SWOOD);


}

void BG_REDRAW ()
{
	unsigned char x,y;
	LCD_COLORWINDOW (0, 0, 159, 126, BLACK);
	LCD_COLORWINDOW (0, 0, 159, 125, GREY1);

	if (SOFT_FLAGS & US_MODE)  //display is different if US or UK mode
	{
		bkground=GREY1;		//print BINGO vertically
		font=alpha_lg;
		justify=LEFT;
		frground=BLUE0;
		LCD_CHAR('B',0,3);
		frground=RED;
		LCD_CHAR('I',0,28);
		frground=WHITE;
		LCD_CHAR('N',0,53);
		frground=GREEN2;
		LCD_CHAR('G',0,78);
		frground=YELLOW;
		LCD_CHAR('O',0,103);

		font=numbrtny;
		justify=LEFT;


		for (i=1;i < USBINGO+1;i++ )		// print all the numbers
		{
			if (TOKENS[i]==0) 	//choose the color of the number
			{
				frground=BLACK; // GREY if the token chip is inside the cage
				bkground=GREY1;

			}
			else
			{
				if (i<16) { frground=WHITE; bkground=BLUE0; } //or respective group color if outside cage
				else
					if (i<31) {frground=WHITE; bkground=RED; }
					else
						if (i<46) {frground=BLACK; bkground=WHITE; }
							else
								if (i<61) {frground=BLACK; bkground=GREEN2; }
									else if (i<76) {frground=BLACK; bkground=YELLOW; }

		//		if (TOKENS[DRAWN_CHIPS]-TOKENS[i] < 3)  //if chip is one of the last three chips drawn, make it REVERSE
		//		{
		//			frground=WHITE; bkground=BLACK;
		//		}

			}
			j=(i-1)/15; //to calculate position, first find the group (B,I,N,G or O)
			k=(i-15*j)/9; // calculate the line inside the group (0 or 1)
			y=j*25+(12*k); //find y
			x=(2+(i-15*j)*18)-k*144; //find x

			LCD_NUMBER (i,x,y,2);
		}

		for (i=0;i<5;i++)		//draw horizontal lines between groups

		LCD_LINE(0,23+i*25,159,23+i*25,GREY2);

		for (i=0;i<5;i++)		//draw horizontal lines

		LCD_LINE(17,10+i*25,159,10+i*25,GREY2);

		for (i=0;i<8;i++)		//draw vertical lines

		LCD_LINE(17+i*18,0,17+i*18,123,GREY2);



	}

	else  //uk mode with 90 chips
	{
		font=numbrtny;
		justify=LEFT;

		for (i=1;i < UKBINGO+1;i++ )		// print all the numbers
		{
			if (TOKENS[i]==0) 	//choose the color of the number
			{
				frground=BLACK; // GREY if the token chip is inside the cage
				bkground=GREY1;

			}
			else
			{
				if (TOKENS[DRAWN_CHIPS]-TOKENS[i] < 3)  //if chip is one of the las three chips drawn, make it REVERSE
				{
					frground=BLACK; bkground=YELLOW;
				}
				else
				{
					frground=BLACK; bkground=WHITE;
				}
			}

			j=(i-1)/10;  // ?  the line
			y=(j*14);

			j=(i-1)%10; // ? the colun
			x=j*16;

			LCD_NUMBER (i,x,y,2);
		}

		for (i=0; i<9;i++) //draw horizontal lines
			LCD_LINE(0,11+i*14,158,11+i*14,GREY2);

		for (i=0; i<10;i++) //draw horizontal lines
			LCD_LINE(14+i*16,0,14+i*16,123,GREY2);

	}



}

void BG_CHIP (int n, int x, int y)
// print a 64x64 bingo token chip @ x,y (center) with number n.
//chip color = white, number=black, bkground=black
{
	font=alpha_lg;
	justify=CENTER;

if (SOFT_FLAGS & US_MODE)  //US_mode
{

	if (n<16) { frground=BLUE; bkground=BLACK; w='B';} //define color and leter according to number
	else
		if (n<31) {frground=RED; bkground=BLACK; w='I';}
		else
			if (n<46) {frground=WHITE; bkground=BLACK;w='N'; }
				else
					if (n<61) {frground=GREEN2; bkground=BLACK;w='G'; }
						else if (n<76) {frground=YELLOW; bkground=BLACK; w='O';}


	LCD_BMP2 (BMP_DISK_64, x-32, y-32, 64, 64);  //unsing a raster image for the chip (instead of a circle procedure) to make it faster

	bkground=frground;
	frground=BLACK;


	LCD_CHAR(w,x-8,y-30);
	font=nmbrL;
	LCD_NUMBER (n,x-18,y-13,2);
}
	else  //UK mode
	{
		frground=GREY2; bkground=BLACK; // the chip color

		LCD_BMP2 (BMP_DISK_64, x-32, y-32, 64, 64);

		bkground=frground; // lettering color
		frground=BLACK;
		font=nmbrL;
		LCD_NUMBER (n,x-18,y-13,2);
	}
BUZZER (55);



}


void ROULETTE (void)
{
	WAIT_NO_KEY (); //wait no key pressed
	slot=rand()%36+1; //initialy choose a random roulette slot to show
	ball=rand()%4; //initially choose a random position for the ball (0~3)
	SOFT_FLAGS |= US_MODE; // US mode or 0 and 00 slots. Otherwise only 0 slot
	SOFT_FLAGS &= ~BIT_KREPEAT; // set player key repeat off

	do {
		k=SELECTMENU(MROULLETE,GREY2,BLACK);//first ask user for the roulette mode -> UK 0~36   US 0,00~36

		if (k==2)
			SOFT_FLAGS |= US_MODE; //US mode 0,00~36
		if (k==4)
			SOFT_FLAGS &= ~US_MODE; //UK mode 0~36
	} while (k==0);   //repeat till user selection


	RL_REDRAW();
	state_ROULETTE=RL_STOP; //initial state


	do {
		GO_SLEEP();	//wait something happens

		rand(); //improve random generator function



		if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

		if (SYS_FLAGS & (BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M)) // a function or a player key is pressed or umpressed
		{
			SYS_FLAGS &= ~ (BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M); //reset M and player KEY flag

			if (!ANY_KEY(BIT_KEY_P1 | BIT_KEY_P2 | BIT_KEY_M)) //if a key was released
			{
				if (state_ROULETTE==RL_ROLLING1)
				{
					TIMER_1=RL_SPEED1 ;
					state_ROULETTE=RL_ROLLING2;   // RLT-> Fast  Ball-> Fast
					released=10+rand()%10; //will roll futher after key realeased by a random time
				}
			}

			if (((GET_KEY (BIT_KEY_M))==5)  || ANY_KEY((BIT_KEY_P1 | BIT_KEY_P2)))  // play or player key pressed
			{
				switch (state_ROULETTE) {

					case RL_STOP:
					case RL_END:
						state_ROULETTE=RL_STOP;  //start from begining
						RL_REDRAW();		//Redraw and ...
						state_ROULETTE = RL_ROLLING1;  //... star roulling roullete but not ball
						TIMER_1=RL_SPEED1 ;  //this is rolling  speed

						break;


				}
			}
			else
			APP_FUNCTION_MAIN();  //check for other function key pressed
		}

		if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
			{
			SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag
			slot=rand()%36+1;
			state_ROULETTE=RL_STOP; //..just redraw and set state to initial
			}

		if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
		{
			SOFT_FLAGS &= ~BIT_REDRAW;
			RL_REDRAW();      //xxx avaliar necessidade de desenhar a bola

		}



		if (SOFT_FLAGS & BIT_SET)  //Setting mode
			{
			SOFT_FLAGS &= ~BIT_SET;  //clear flag

				k=SELECTMENU(MROULLETE,GREY2,BLACK);//first ask user for the roulette mode -> UK 0~36   US 0,00~36

				if (k==2)
					SOFT_FLAGS |= US_MODE; //US mode 0,00~36
				else
				{
				if (k==4)
					SOFT_FLAGS &= ~US_MODE; //UK mode 0~36
				}


			if ((k==2)||(k==4)) //if mode was seleceted, restart game.
			{
			SOFT_FLAGS |= BIT_RESET; //clear all data after setting
			state_ROULETTE=RL_STOP; //initial state
			slot=rand()%36+1;
			TIMER_1=1; //force interruption to reset data

			}
			RL_REDRAW();  //Else keep current game

			}



		if (SYS_FLAGS & BIT_TIMER_1)  //process timeout
			{
			SYS_FLAGS ^= BIT_TIMER_1; //clear flag

			switch (state_ROULETTE) {

				case RL_ROLLING1:	// a key is pressed -> RLT-> Fast  Ball-> zero


					if (SOFT_FLAGS & US_MODE)  //ramdonly choose a slot depending on mode
					slot=rand()%RLT_SLOT[1][0];
					else
					slot=rand()%RLT_SLOT[0][0];

					RL_SLOTS(); //draw 4 slots number

					TIMER_1=RL_SPEED1 ;  //this is rolling  speed


					break;

				case RL_ROLLING2:		// NO key is pressed -> RLT-> Fast  Ball-> Fast


					if (SOFT_FLAGS & US_MODE)  //ramdonly choose a slot depending on mode
					slot=rand()%RLT_SLOT[1][0];
					else
					slot=rand()%RLT_SLOT[0][0];

					RL_SLOTS(); //draw 4 slots number

					RL_RUNBALL(ball+1);  //run ball
					if (ball>10) ball=-1;


					TIMER_1=RL_SPEED1 ;  //this is rolling  speed


					if(--released == 0) //will roll in this mode for a while..
					{
						state_ROULETTE=RL_ROLLING3; //then advance to next state
						released=10+rand()%10; //will roll futher after key realeased by a random time


					}

				break;

				case RL_ROLLING3:		// RLT-> Medium  Ball-> Fast

					if ((released%2)==0)  //divide speed by 2
					{
					if (SOFT_FLAGS & US_MODE)  //ramdonly choose a slot depending on mode
					slot=rand()%RLT_SLOT[1][0];
					else
					slot=rand()%RLT_SLOT[0][0];

					RL_SLOTS(); //draw 4 slots number
					}

					RL_RUNBALL(ball+1);  //run ball
					if (ball>10) ball=-1;


					TIMER_1=RL_SPEED1 ;  //this is rolling  speed


					if(--released == 0) //will roll in this mode for a while..
					{
						state_ROULETTE=RL_ROLLING4; //then advance to next state
						released=20+rand()%10; //will roll futher after key realeased by a random time


					}

					break;

				case RL_ROLLING4:		// RLT-> Slow  Ball-> Medium

					if ((released%3)==0)  //divide speed by 3
					{

					if (SOFT_FLAGS & US_MODE)  //increse roulette 1 by one depending on mode
						{
						if (++slot > RLT_SLOT[1][0]) slot =1;    //increse roulette 1 by one
						}
					else
						if (++slot > RLT_SLOT[0][0]) slot =1;    //increse roulette 1 by one

					RL_SLOTS(); //draw 4 slots number
					}

					if ((released%2)==0)  //divide speed by 2
					{
						RL_RUNBALL(ball+1);  //run ball
						if (ball>10) ball=-1;
					}


					TIMER_1=RL_SPEED1 ;  //this is rolling  speed


					if(--released == 0) //will roll in this mode for a while..
					{
						state_ROULETTE=RL_ROLLING5; //then advance to next state
						released=10+rand()%10; //will roll futher after key realeased by a random time


					}

					break;

				case RL_ROLLING5:		// RLT-> STOP  Ball-> SLOW


					if ((released%3)==0)  //divide speed by 3
					{
						RL_RUNBALL(ball+1);  //run ball
						if (ball>10) ball=-1;
					}


					TIMER_1=RL_SPEED1 ;  //this is rolling  speed


					if(--released == 0) //will roll in this mode for a while..
					{
						state_ROULETTE=RL_ROLLING6; //then advance to next state
						released=rand()%4; //now released is the random slot to rest the ball
						TIMER_1=RL_SPEED2;   //speed of stopping ball

					}

					break;

				case RL_ROLLING6:		// RLT-> STOP  Ball-> STOPPING

					if (released != ball)  //if ball did not reach the final slot, move ball
						{
						RL_RUNBALL(ball+1);  //run ball
						if (ball>10) ball=-1;
						TIMER_1=RL_SPEED2;  //this is rolling  speed
						}
					else	//ball stops
					{
						state_ROULETTE=RL_END;
						RL_RUNBALL (111); //just to erase ball from rail
						RL_SLOTBALL (released); //and drop ball to the slot
						TIMER_1=0;
					}

					break;


			}
			}





	} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

	TOILET ();



}

void RL_REDRAW(void)
{
	LCD_COLORWINDOW (0, 0, 159, 126, BLACK);
	LCD_LINE(0,0,152,0,GOLD);  //draw base of roulette
	LCD_COLORWINDOW (0, 1, 152, 34, GREY0);
	LCD_LINE(0,35,152,35,GOLD);
	LCD_COLORWINDOW (0, 36, 152, 59, SWOOD);
	LCD_LINE(0,60,152,60,GOLD);
	LCD_COLORWINDOW (0, 91, 152, 124, BLUE0);
	LCD_LINE(0,90,152,90,GOLD);
	LCD_LINE(0,125,152,125,GOLD);
	LCD_LINE(0,60,0,125,GOLD);
	LCD_LINE(38,60,38,125,GOLD);
	LCD_LINE(76,60,76,125,GOLD);
	LCD_LINE(114,60,114,125,GOLD);
	LCD_LINE(152,60,152,125,GOLD);
	RL_SLOTS ();


	RL_SLOTS ();

	if (state_ROULETTE != RL_STOP)
	RL_SLOTBALL (ball);

}

void RL_SLOTS (void)  //draw 4 slots starting from slots variable
{
	BUZZER(55);
	frground=WHITE;
	font=nmbrL;
	justify=CENTER;

	i=slot;   //fetch for the next 4 slots number to print
	k=0; //current slot on screen
	chipp=(SOFT_FLAGS & US_MODE) != 0; // chipp=1 if US mode,  0 otherwise. chipp is used as aux variable here
	do {
		j=RLT_SLOT[chipp][i];  //j= is the number in slot to print
		if ((j>0 && j<11) || (j>18 && j<29)){ 	//number ranges from 1 to 10 and 19 to 28
			if (j%2) bkground=RED;				//odd numbers are red and even are black
			else bkground=BLACK;
		}
		else									//ranges from 11 to 18 and 29 to 36
		{
				if (j%2) {						//odd numbers are black and even are red

					bkground=BLACK;
				}
				else
					{
					if (j==0 || j==100)			//0 and 00 green
					bkground=GREEN1;
					else bkground=RED;
					}

		}
		LCD_COLORWINDOW(1+k*38,61,37+k*38,89,bkground);  //paint current slot with coreesponding color

		if (j==0 && i== 20)  //check if it is 00 number
		{
			LCD_NUMBER(0,1+k*38,62,1);
			LCD_NUMBER(0,19+k*38,62,1);
		} else
		LCD_NUMBER(j,1+k*38,62,2);

		if (++i > RLT_SLOT[chipp][0]) i=1;
	} while (++k < 4);


}

void RL_SLOTBALL (int nball)  //print the ball to the new   rest ball slots and erase last ball position
{
	BUZZER(155);

	if (ball<4 && ball>-1) LCD_COLORWINDOW (ball*38+1, 91, ball*38+37, 124, BLUE0);  //erase last ball position
	ball=nball; //update ball variable
	bkground=BLUE0;
	RL_BALL (ball*38+19,108,GREY2);
}

void RL_RUNBALL (int nball)  //print the ball to the new   running ball track and erase last ball position
{

	if (ball<4 && ball>-1) LCD_COLORWINDOW (ball*38+1, 2, ball*38+37, 34, GREY0);  //erase last ball position
	ball=nball; //update ball variable
	bkground=GREY0;
	 if (ball<4 && ball>-1) RL_BALL (ball*38+19,18,GREY2);
}

void RL_BALL (int x, int y, unsigned int color)
// print a 32x32 roulette ball  @ x,y (center)

{
	frground=color;

	LCD_BMP2 (BMP_DISK_32, x-16, y-16, 32, 32);  //unsing a raster image for the ball (instead of a circle procedure) to make it faster

}

