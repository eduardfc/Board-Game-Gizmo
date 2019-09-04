
#ifndef APP_FUNCTION_H_
#define APP_FUNCTION_H_


 	//states for DICE

#define SHAKING_DIE 1	//states for state_DICE
#define THROW_DIE 3
#define STOP_ROLL 5 // die just stopped rolling
#define END_DICE 111

//states for MUTE
#define KEYM_PRESSED 1
#define KEYM_RELEASED 3
#define END_MUTE 111

// states for MENU
#define WAIT_U 1
#define NEXT_KEY 3
#define SELECTED 5


#define ROLLSPEED 43 //speed of die rolling in ms
#define WAIT_STOP 2500 //wait time after die stops
#define HBUZZ 33 // click noise while holding die
#define RBUZZ 66 // click noise while released

#define MAXCOL 19  	// collums from 0~19 in writer() command
#define MAXLIN 8	// lines from 0~8 in writer() command

#define xxx 15 // xxx

#endif /* APP_FUNCTION_H_ */
