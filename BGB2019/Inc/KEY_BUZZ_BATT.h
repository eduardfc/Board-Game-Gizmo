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

#define V_Batt_100 3300//ADC reading for batt 100%
#define V_Batt_75 3074//ADC reading for batt 75%
#define V_Batt_50 2849//ADC reading for batt 50%
#define V_Batt_25 2624//ADC reading for batt 25%
#define V_Batt_THR 150 // threshold

//char The_Key; // the key that just awaked the cpu. 0 means no key pressed or key released
//int The_Group; // and the group. May be P1, P2 or M

unsigned short ADC_RAW[2];
#endif

