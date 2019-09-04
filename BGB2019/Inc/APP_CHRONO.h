
 #ifndef __APPCHRONO_H
 #define __APPCHRONO_H
 


 	//states for chronograph

#define C_STOP 0		// chronometer stopped  dahhh
#define C_RUN 1			// chronometer RUNNING  :)
#define C_UPDATE 3		// editing start value for regressive counter and hourglass
#define C_BUPDATE 5		// editing bonus value for chess
#define C_DUPDATE 7		// editing delay value for chess
#define C_HUPDATE 9		// editing HOUR value for chess
#define C_MUPDATE 11	// editing MINUTE value for chess or mode for hourglass
#define C_SUPDATE 13	// editing SECONDS value for chess
#define C_P1RUN	15		// Player 1 chron running
#define C_P2RUN	17		// Player 2 chron running
#define C_P1DELAY 19	// Player 1 delay counting down
#define C_P2DELAY 21	// Player 1 delay counting down
#define C_P1PAUSE 23		// chronometer paused, used in hourglass. To make it different from stop (time =0).
#define C_P2PAUSE 25		// chronometer paused, used in hourglass. To make it different from stop (time =0).
#define C_PAUSE 27		// chronometer paused, used in hourglass. To make it different from stop (time =0).


#define ABSOLUTE 0 		//CHRONO LAP MODE is absolute values
#define DIFFERENTIAL 1 		//CHRONO LAP MODE is differential values

#define DFTIMEMAIN 300000	//default main time value for chess 5m
#define DFTIMEDELAY 3	//default delay time value for chess 3s

#define MAXSANDLEVEL 45 // initial sand level of hourglass

#define CHESS_MODE 0X0010	//same as BIT_AUX flag in SOFT_FLAGS, used in hourglass to indicate hourglass CHESS MODE

#define BURST1 0X0004 //same as BIT_AUX1 flag in SOFT_FLAGS, used in CHESS to indicate player 1 bursted
#define BURST2 0X0008 //same as BIT_AUX2 flag in SOFT_FLAGS, used in CHESS to indicate player 2 bursted




typedef struct  {		//struct used in chronograph app
	unsigned long int CHRON; //laps marks
	unsigned char LAP;

} laps_db;

typedef struct  {	//struct used for chess

	unsigned int PLAY_COLOR; //this player  color
	unsigned long int *CHRONO;  //discrete run chrono counter
	unsigned long int *SCHRONO; //step run chrono run (multiple of chrono alarm)
	unsigned long int *CHRONO_ALARM; //Chrono will set alarm every CHRONO_ALARM (0=off)
	unsigned long int TIME_MAIN;  //regressive actual value
	unsigned long int TIME_MAIN_SET;  //regressive start value
	unsigned char TIME_DELAY;  //delay value (0~99seconds)
	unsigned int F_CHRONO_RUN;  //mask for start/stop chrono
	short F_ALARM; //Cyclic alarm mask for Chrono
	unsigned int F_KEY;  //mask for player KEY
	unsigned char  BONUS; //bonus value to add to time main after each turn (from 0 to 99sec)
	unsigned char X; // display collum to write
	char JUSTIFY; // If left or right
} chess_db;

 
 #endif
