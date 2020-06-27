#ifndef FUNC_KBB
#define FUNC_KBB

#define stab 100
#define STABILITY for (int ii=0;ii<stab;++ii );

#define KEYTONE 150   //time of buzzer  for feedback of key state change
// #define BATT_CYCLE 1025 //read batt voltage every 1.025 seconds. Or every 61.5 cycles of a 60Hz noise.
#define BATT_CYCLE 1025  // xxx ou usa 0 para parar batt e facilitar diagnostico

// battery voltage from 2.4V to 4.2V
// ADC readings from 0 to 4095 (0V to 3300mV)
// 1 bit ADC -> 805.86uV
// ADC battery voltage reading range -> 2.978 (2.400mV) to 4.095(3.300mv)
// ADC battery 0% -> 2978
// ADC battery 25% -> 3257   batt_status=0
// ADC battery 50% -> 3536   batt_status=1
// ADC battery 75% -> 3815   batt_status=2
// ADC battery 100% -> 4095   batt_status=3
// ADC THRESHOLD -> 186

// battery voltage from 2.2V to 4.2V
// ADC readings from 0 to 4095 (0V to 3300mV)
// 1 bit ADC -> 805.86uV
// ADC battery voltage reading range -> 2.978 (2.400mV) to 4.095(3.300mv)
// USING A RESISTOR DIVIDER BY 5  TO PROTECT INPUT PORT
// THRESHOLD VOLTAGES FOR EACH BATTERY LEVEL STATUS ARE: 3903mV(100%) 3800mV(75%) 3695mV(50%) 3445mV(25%)
// USING  HYSTERESIS   TRANSACTION FACTOR BETWEEN EACH LEVEL
// EXAMPLE: IF IN LEVEL 75% (--) or 3800mV:
// threshold to transiction to level 50% (-)  is 3695mV
// threshold to transiction to level 100% (---) is 3903mv


#define V_Batt_100 781//ADC reading for batt 100%  (3903 if /1  or  781 if /5)
#define V_Batt_75 760//ADC reading for batt 75% (3800 if /1  or  760 if /5)
#define V_Batt_50 739//ADC reading for batt 50% (3695 if /1  or  739 if /5)
#define V_Batt_25 689//ADC reading for batt 25% (3445 if /1  or  689 if /5)

//char The_Key; // the key that just awaked the cpu. 0 means no key pressed or key released
//int The_Group; // and the group. May be P1, P2 or M

unsigned short ADC_RAW[2];
#endif

