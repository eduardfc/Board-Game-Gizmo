#include "main.h"
#include "LCD_ST7735.h"
#include "APP_SCORE.h"
#include "FONT_number_xg.h"
#include "FONT_number_lg.h"
#include "FONT_number_ss.h"
#include "FONT_alpha_lg.h"
#include <stdio.h>



extern unsigned int TIMER_1;
extern unsigned int TIMER_2;
extern unsigned int SYS_FLAGS;
extern unsigned int SOFT_FLAGS;
extern const char *HELP;  //pointer to help text array
unsigned char state_general;
unsigned char state_P1;
unsigned char state_P2;
unsigned char eachplayer;
short aux;
short oldscore;
unsigned char field, x, y;

score_db PLAYERS [2] = {   //Array to store players score and etc.
		{0, COLORP1 , 0 , 0 , 0 , 0, 0, BIT_TIMER_1, &TIMER_1, BIT_KEY_P1,IDLE,0,LEFT}, //score,color,adding,mem1,mem2,mem3, timer, KEY, STATE
		{0, COLORP2 , 0 , 0 , 0 , 0, 0, BIT_TIMER_2, &TIMER_2, BIT_KEY_P2,IDLE,159,RIGHT}
};

const char HELP_SCORE[]={"/S     SCOREBOARD/L/BA Two players score board. Scores from -999 to 9999./L/SKeys:/L/BP1,P2: Add//subtract value to current score./L%/GPRESET:/B Initial score./L%/GRESET:/B Zero./L%/GSET: /BSet initial score./Z"};

void GO_SLEEP(void);
void DISPLAY_BATT(void);
void	updatescore (void);
char ANY_KEY( unsigned int);
char GET_KEY_VALUE (unsigned int);
void LCD_DICE (unsigned char d, unsigned char x, unsigned char y,unsigned int color,unsigned int pip);
void APP_FUNCTION_MAIN();
void WAIT_NO_KEY (void);
void LCD_WORD (const char w[], unsigned char x, unsigned char y, unsigned char field);
void TOILET (void);


void SCORE(void) {



SC_REDRAW();

WAIT_NO_KEY (); //wait no key pressed
SOFT_FLAGS |= BIT_KREPEAT; // set player key repeat on

HELP=HELP_SCORE; //set help file

do {
	 GO_SLEEP();	//wait something happens


	if (SYS_FLAGS & BIT_BATT) DISPLAY_BATT();  //process batt

	if (SYS_FLAGS & BIT_KEY_M) APP_FUNCTION_MAIN(); //function key pressed


	if (SOFT_FLAGS & (BIT_RESET|BIT_PRESET))  //reset all data
	{
		for (int i = 0;  i < 2; ++ i) {

		if (SOFT_FLAGS & BIT_RESET) {PLAYERS [i].SCORE=0; PLAYERS [i].FIRST=0;}//clear score
		else PLAYERS [i].SCORE=PLAYERS [i].FIRST;       // use the first value as preset
		PLAYERS [i].MEM1=0;
		PLAYERS [i].MEM2=0;
		PLAYERS [i].MEM3=0;
		PLAYERS [i].ADDING=0;

	}

		SOFT_FLAGS &= ~(BIT_RESET|BIT_PRESET); //clear flag

	}
	if (SOFT_FLAGS & BIT_REDRAW)  //need to redraw screen
	{
		SOFT_FLAGS &= ~BIT_REDRAW;
		SC_REDRAW();
	}

	if (SOFT_FLAGS & BIT_SET)  //Setting initial score
	{
		SOFT_FLAGS &= ~BIT_SET;
		PLAYERS [0].FIRST=0; //reseting FIRST data will make it store first added value
		PLAYERS [1].FIRST=0;

		frground=WHITE;
		bkground=BLACK;
		font=alpha_lg;
	 	justify=LEFT;
	 	LCD_COLORWINDOW (0,1,79,34,BLACK);
	 	LCD_COLORWINDOW(81,1,159,34,BLACK);
		LCD_WORD("SET",0,5,4);
	 	justify=RIGHT;
		LCD_WORD("SET",96,5,4);

	}

	for (eachplayer=0; eachplayer <2; ++eachplayer)
	{
		if (SYS_FLAGS &  PLAYERS[eachplayer].KEY)  //Player Key status changed?
		{
			SYS_FLAGS &= ~PLAYERS[eachplayer].KEY;  //clear flag //xxx melhor? SYS_FLAGS ^= PLAYERS[eachplayer].KEY
			switch (PLAYERS[eachplayer].STATE) {  //actin according to state
				case IDLE:   // Player just pressed a Key
				case KEYRELEASED:

					if (ANY_KEY(PLAYERS[eachplayer].KEY))  //check if a key is pressed
					{
						PLAYERS [eachplayer].STATE= KEYPRESSED;  //change state
						updatescore ();  // update score and add value and display
						//xxxy *PLAYERS [eachplayer].TIMERADRESS=TIMEtoREPEAT; //star timer to check repeat
					}
					break;

				case KEYPRESSED:  //state is keypressed and key is just released
				{
					PLAYERS [eachplayer].STATE= KEYRELEASED;
					*PLAYERS [eachplayer].TIMERADRESS=TIMEtoGOtoIDLE;
					break;
				}
			}
		}

		if (SYS_FLAGS & PLAYERS[eachplayer].TIMERMASK)  //a timer overflow occured?
		{
			SYS_FLAGS ^= PLAYERS [eachplayer].TIMERMASK; //clear flag
			switch (PLAYERS [eachplayer].STATE) {  //action according to state
			case KEYPRESSED:  //player is holding keypressed
		//	{
		//		if (ANY_KEY (PLAYERS[eachplayer].KEY))  //make it sure key is really pressed
		//		{
		//			updatescore ();    //update score and add automatically
		//			*PLAYERS [eachplayer].TIMERADRESS=TIMEREPEATSPEED; //prepare for next repeat
		//		}
		//		else   // there is no key pressed
		//		{
		//			PLAYERS [eachplayer].STATE= KEYRELEASED;
		//			*PLAYERS [eachplayer].TIMERADRESS=TIMEtoGOtoIDLE;
		//		}
		//	}
			break;
			case KEYRELEASED:  //a key was pressed and now is released and TIMEtoGOtoIDLE ends
			{
				PLAYERS [eachplayer].STATE=IDLE;
				bkground=BLACK;
				font=nmbrSS;
				justify=PLAYERS [eachplayer].JUSTIFY;
				PLAYERS [eachplayer].MEM3=PLAYERS [eachplayer].MEM2; //update MEM3
				if (PLAYERS[eachplayer].MEM3) frground=GREY1; //print if <> 0
				else frground=BLACK;
				LCD_NUMBER (PLAYERS[eachplayer].MEM3,PLAYERS [eachplayer].X,YMEM3,4);

				PLAYERS [eachplayer].MEM2=PLAYERS [eachplayer].MEM1; //update MEM2
				if (PLAYERS[eachplayer].MEM2) frground=GREY1; //print if <> 0
				else frground=BLACK;
				LCD_NUMBER (PLAYERS[eachplayer].MEM2,PLAYERS [eachplayer].X,YMEM2,4);

				PLAYERS [eachplayer].MEM1=PLAYERS [eachplayer].ADDING; //update MEM1
				if (PLAYERS[eachplayer].MEM1) frground=GREY1; //print if <> 0
				else frground=BLACK;
				LCD_NUMBER (PLAYERS[eachplayer].MEM1,PLAYERS [eachplayer].X,YMEM1,4);

				if (!PLAYERS[eachplayer].FIRST) PLAYERS[eachplayer].FIRST=PLAYERS [eachplayer].SCORE; //store firs  value for preset

				PLAYERS [eachplayer].ADDING=0; //clear ADDING
				frground=BLACK; //erase adding field
				font=nmbrL;
				LCD_NUMBER (0,PLAYERS [eachplayer].X,YADD,4);
			}
			}

		}
	}


} while (!(SOFT_FLAGS & BIT_RETURN));  // exit from app and return to MENU

TOILET ();

}

void	updatescore ()  //update score according to key and print score and add
{
	aux=GET_KEY_VALUE (PLAYERS[eachplayer].KEY); //aux = value to add
	if (aux) {
	PLAYERS[eachplayer].ADDING += aux;  //update adding and score of player
	if (PLAYERS[eachplayer].ADDING>9999) PLAYERS[eachplayer].ADDING=9999;
	if (PLAYERS[eachplayer].ADDING<-999) PLAYERS[eachplayer].ADDING=-999;
	oldscore = PLAYERS [eachplayer].SCORE;
	PLAYERS[eachplayer].SCORE += aux;
	if ((PLAYERS[eachplayer].SCORE>9999)||(PLAYERS[eachplayer].SCORE<-999)) //limit max & min score
		PLAYERS[eachplayer].SCORE=oldscore;


				//prepare for print according to eachplayer
	bkground=BLACK;
	justify=PLAYERS[eachplayer].JUSTIFY;

	font=nmbrL;   // let�s print added value
	if (PLAYERS[eachplayer].ADDING) frground=WHITE;	 // print if <> 0
	else frground=BLACK; // will not show ADD if =0 but clean field  xxx talvez melhor imprimir. usar n�o imprimir s� quando retornar para IDLE
	LCD_NUMBER (PLAYERS[eachplayer].ADDING,PLAYERS[eachplayer].X,YADD,4);

	//let�s print SCORE
	frground=PLAYERS[eachplayer].COLOR;

	font=nmbrXL;
	field=3;
	if ((PLAYERS [eachplayer].SCORE>999) || (PLAYERS [eachplayer].SCORE< -99)) //choose font that fit display
	{
		if ((oldscore<1000) && (oldscore>-100))   //must clean field 'cause changed from XL to L
		{
			frground=BLACK;
			LCD_NUMBER (0,PLAYERS[eachplayer].X, YSCORE,3);   //clean field
			frground=PLAYERS[eachplayer].COLOR;
		}
		font=nmbrL;
		field=4;
	}

	LCD_NUMBER (PLAYERS[eachplayer].SCORE, PLAYERS[eachplayer].X, YSCORE,field );
}
}

void	SC_REDRAW(void) {	//Clear and redraw score screen


LCD_COLORWINDOW (0, 0, 159, 126, BLACK);

LCD_LINE (0,0,159,0,GREY1);
LCD_LINE (80,0,80,126,GREY1);






for (int iredraw = 0;  iredraw < 2; ++ iredraw) {
	font=nmbrXL;
	field=3;
	if (PLAYERS [iredraw].SCORE>999 || PLAYERS [iredraw].SCORE< -99){
		font=nmbrL;
		field=4;
	}
	frground=PLAYERS [iredraw].COLOR;
	bkground=BLACK;
	y=35;   //5
	if (iredraw) {x=159;
	justify=RIGHT;
	} else {
		x=0;				// xxx refazer tudo considerando que x,justify est�o em PLAYERS[]
		justify=LEFT;
	}
	LCD_NUMBER(PLAYERS[iredraw].SCORE,x,y,field);

	font=nmbrL;								//pode ser retirado pois n�o tem adi��o apos interrup��o
	y=3;	// 51								//xxxx faltou acertar field
	frground=WHITE;							//
	if (PLAYERS[iredraw].ADDING)
	LCD_NUMBER (PLAYERS[iredraw].ADDING,x,y,4);	//

	font=nmbrSS;
	y=84;
	frground=GREY1;
	if (PLAYERS[iredraw].MEM1)
	LCD_NUMBER (PLAYERS[iredraw].MEM1,x,y,4);
	y=98;
	if (PLAYERS[iredraw].MEM2)
	LCD_NUMBER (PLAYERS[iredraw].MEM2,x,y,4);
	y=112;
	if (PLAYERS[iredraw].MEM3)
	LCD_NUMBER (PLAYERS[iredraw].MEM3,x,y,4);



}
}

