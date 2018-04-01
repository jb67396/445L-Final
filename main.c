//***********************  main.c  ***********************
// Program written by:
// - Steven Prickett  steven.prickett@gmail.com
//
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
//*********************************************************
/* Modified by Jonathan Valvano
 Sept 14, 2016
 Out of the box: to make this work you must
 Step 1) Set parameters of your AP in lines 59-60 of esp8266.c
 Step 2) Change line 39 with directions in lines 40-42
 Step 3) Run a terminal emulator like Putty or TExasDisplay at
         115200 bits/sec, 8 bit, 1 stop, no flow control
 Step 4) Set line 50 to match baud rate of your ESP8266 (9600 or 115200)
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "SysTick.h"
#include "DAC.h"
#include "Timer0A.h"
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "LED.h"
#include "PWM.h"
#include "main.h"

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


volatile int processBuff = 0;
char* instructionBuffer;


/* Buttons */
int PP_Button = 0;

/* Music */
#define C_2 19111   // 65.406 Hz			
#define DF_1 18039   // 69.296 Hz			
#define D_1 17026   // 73.416 Hz			
#define EF_1 16071   // 77.782 Hz			
#define E_1 15169   // 82.407 Hz			
#define F_1 14317   // 87.307 Hz			
#define GF_1 13514   // 92.499 Hz			
#define G_1 12755   // 97.999 Hz			
#define AF_1 12039   // 103.826 Hz			
#define A_1 11364   // 110.000 Hz			
#define BF_1 10726   // 116.541 Hz			
#define B_1 10124   // 123.471 Hz			
#define C_1 9556   // 130.813 Hz			
#define DF0 9019   // 138.591 Hz			
#define D0 8513   // 146.832 Hz			
#define EF0 8035   // 155.563 Hz			
#define E0 7584   // 164.814 Hz			
#define F0 7159   // 174.614 Hz			
#define GF0 6757   // 184.997 Hz			
#define G0 6378   // 195.998 Hz			
#define AF0 6020   // 207.652 Hz			
#define A0 5682   // 220.000 Hz			
#define BF0 5363   // 233.082 Hz			
#define B0 5062   // 246.942 Hz			
#define C0 4778   // 261.626 Hz			
#define DF 4510   // 277.183 Hz			
#define D 4257   // 293.665 Hz			
#define EF 4018   // 311.127 Hz			
#define E 3792   // 329.628 Hz			
#define F 3579   // 349.228 Hz			
#define GF 3378   // 369.994 Hz			
#define G 3189   // 391.995 Hz			
#define AF 3010   // 415.305 Hz			
#define A 2841   // 440.000 Hz			
#define BF 2681   // 466.164 Hz			
#define B 2531   // 493.883 Hz			
#define C 2389   // 523.251 Hz			
#define DF1 2255   // 554.365 Hz			
#define D1 2128   // 587.330 Hz			
#define EF1 2009   // 622.254 Hz			
#define E1 1896   // 659.255 Hz			
#define F1 1790   // 698.456 Hz			
#define GF1 1689   // 739.989 Hz			
#define G1 1594   // 783.991 Hz			
#define AF1 1505   // 830.609 Hz			
#define A1 1420   // 880.000 Hz			
#define BF1 1341   // 932.328 Hz			
#define B1 1265   // 987.767 Hz			
#define C1 1194   // 1046.502 Hz			
#define DF2 1127   // 1108.731 Hz			
#define D2 1064   // 1174.659 Hz			
#define EF2 1004   // 1244.508 Hz			
#define E2 948   // 1318.510 Hz			
#define F2 895   // 1396.913 Hz			
#define GF2 845   // 1479.978 Hz			
#define G2 797   // 1567.982 Hz			
#define AF2 752   // 1661.219 Hz			
#define A2 710   // 1760.000 Hz			
#define BF2 670   // 1864.655 Hz			
#define B2 633   // 1975.533 Hz			
#define C2 597   // 2093.005 Hz			
				

unsigned short notes[] =   {D, D, E, F, A, G, D1, D1, 0, D1, D1, D1, C, A, A, 0, D, D, E, F, 0, A, G, D, D, C0, D, 0, D, D, D, DF, DF, D, 0, 0, 0, 0, 0, 0, 0, D1, D1, D1, C, A, G, D, D, 0, D1, D1, D1, C, A, A, 0, D1, D1, D1, C, A, G, D, D, 0, D, D, DF, DF, D, 0};
unsigned short lengths[] = {1, 5, 1, 5, 1, 5, 1, 3, 2, 1, 5, 1, 5, 1, 6, 5, 1, 5, 1, 3, 2, 1, 1, 3, 1, 1, 3, 2, 1, 5, 1, 5, 1, 6, 6, 6, 6, 6, 6, 6, 5, 1, 5, 1, 5, 1, 1, 5, 3, 2, 1, 5, 1, 5, 1, 6, 5, 1, 5, 1, 5, 1, 5, 1, 4, 2, 5, 1, 5, 1, 6, 6};	
	
typedef struct Music Music;
struct Music {
	unsigned short* note;
	unsigned short* duration;
};
	
Music song = { .note = notes, .duration = lengths};
	
const unsigned short Bassoon64[64]={
1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003};

const unsigned short Oboe64[64]={
1024, 1024, 1014, 1008, 1022, 1065, 1093, 1006, 858, 711, 612, 596, 672, 806, 952, 1074, 1154, 1191, 
1202, 1216, 1236, 1255, 1272, 1302, 1318, 1299, 1238, 1140, 1022, 910, 827, 779, 758, 757, 782, 856, 
972, 1088, 1177, 1226, 1232, 1203, 1157, 1110, 1067, 1028, 993, 958, 929, 905, 892, 900, 940, 1022, 
1125, 1157, 1087, 965, 836, 783, 816, 895, 971, 1017};

const unsigned short Trumpet64[64]={
  987, 1049, 1090, 1110, 1134, 1160, 1139, 1092, 1070, 1042, 1035, 1029, 1008, 1066, 1150, 1170, 1087, 915, 679, 372, 151, 
  558, 1014, 1245, 1260, 1145, 1063, 984, 934, 960, 1027, 1077, 1081, 1074, 1064, 1042, 1010, 974, 968, 974, 994, 1039, 
  1094, 1129, 1125, 1092, 1056, 1056, 1082, 1059, 1046, 1058, 1061, 1045, 1034, 1050, 1094, 1112, 1092, 1063, 1053, 1065, 1052, 992};
	
/* This array is for notes, starting with offset -33 from middle A to +27 from middle A	(A at 0 offset and 440 Hz) */
/* This is the reload value of the SysTick Timer */
const unsigned short SineUpdateDelay[61] = {11945,11274,10641,10044,9480,8948,8446,7972,7525,7102,6704,6327,5972,5637,5321,
	5022,4740,4474,4223,3986,3762,3551,3352,3164,2986,2819,2660,2511,2370,2237,2112,1993,1881,1776,1676,1582,1493,1409,1330,1256,
	1185,1119,1056,997,941,888,838,791,747,705,665,628,593,559,528,498,470,444,419,395,373};

const float envelope[11] = {0.95, 1.0, 0.65, 0.5, 0.475, 0.45, 0.425, 0.4125, 0.4, 0.39, 0.385};
//const float envelope[10] = {1.0, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09};
	
const unsigned short* outArray = Trumpet64;

#define SONG_TEMPO 24150944 //3773585
int idx = 0, env = 0, envI = 0;
float factor = 0.6;
int duration = 0;
volatile int playing = 0;
volatile int curNote = 0;

void SysTick_Handler(void){
  //DAC_Out(Trumpet64[idx&0x3F] * 10 / factor);
	//LED_BlueToggle();
	DAC_Out(outArray[idx&0x3F] * factor);
	if(env > duration){
		factor = envelope[envI++];
		env = 0;
	}
	idx++;
	env++;
}
void songMaker(void){
	//LED_BlueToggle();
	if(PP_Button){
		// Handle what goes into Systick timer and dac_out
		NVIC_ST_RELOAD_R = song.note[curNote];
		TIMER0_TAILR_R = song.duration[curNote] * SONG_TEMPO;
		duration = song.duration[curNote] * SONG_TEMPO / 12;
		duration /= song.note[curNote];
		if(curNote < 71)
			curNote++;
		else
			curNote = 0;													
		idx = 0;
		env = 0;
		envI = 0;
	}
}
void setBuff(int value, char* buffer){
	processBuff = value;
	instructionBuffer = buffer;
}

volatile int rPower = 0;
volatile int lPower = 0;
volatile int powerSet = 0;
volatile int powerAngle = 0;

void setRobotStatus(){
		// This is just for debugging
		// Will have globals set based off inputs
		// Display will be triggered off global flag updates
		processBuff = 0;
		ST7735_SetCursor(0,0);
		// Button 1 is pressed
		if(instructionBuffer[2] == '1'){
			ST7735_OutChar('1');
			if(PP_Button == 0){
				PP_Button = 1;
				TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
				NVIC_ST_CTRL_R |= 0x02;
				songMaker();
			}
		}
		else{
			ST7735_OutChar('0');
			if(PP_Button == 1){
				PP_Button = 0;
				TIMER0_CTL_R = 0x00000000;    // 10) disable TIMER0A
				NVIC_ST_CTRL_R &= ~0x02;
				songMaker();
			}
		}
		ST7735_SetCursor(0,1);
		// Button 2 is pressed
		if(instructionBuffer[4] == '1'){
			ST7735_OutChar('1');
		}
		else{
			ST7735_OutChar('0');
		}
		ST7735_SetCursor(0,2);
		// Button 3 is pressed
		if(instructionBuffer[6] == '1'){
			ST7735_OutChar('1');
		}
		else{
			ST7735_OutChar('0');
		}
		ST7735_SetCursor(0,3);
		//Output x coord
		ST7735_OutChar(instructionBuffer[7]);
		ST7735_OutChar(instructionBuffer[8]);
		ST7735_OutChar(instructionBuffer[9]);
		ST7735_OutChar(instructionBuffer[10]);
		//ST7735_OutChar(instructionBuffer[13]);
		ST7735_SetCursor(0,4);
		//Output y coord
		//ST7735_OutChar(instructionBuffer[11]);
		ST7735_OutChar(instructionBuffer[12]);
		ST7735_OutChar(instructionBuffer[13]);
		ST7735_OutChar(instructionBuffer[14]);
		//ST7735_OutChar(instructionBuffer[18]);
		ST7735_SetCursor(0,5);
		
		powerSet = ((instructionBuffer[7] - 0x30) * 100) + (instructionBuffer[9] - 0x30) * 10 + (instructionBuffer[10] - 0x30);
		powerAngle = ((instructionBuffer[12] - 0x30) * 100) + (instructionBuffer[13] - 0x30) * 10 + (instructionBuffer[14] - 0x30);
		
		switch(powerAngle / 45){
			case 0:
				lPower = powerSet;
				rPower = (powerSet * (45 - powerAngle))/45; 			// 100 -> 0
				break;
			case 1:
				lPower = powerSet;
				rPower = -1 * (powerSet * (powerAngle - 45))/45; 	// 0 -> -100
				break;
			case 2:
				lPower = (powerSet * (135 - powerAngle))/45; 			// 100 -> 0 
				rPower = -1 * powerSet;
				break;
			case 3:
				lPower = -1 * (powerSet * (powerAngle - 135))/45; // 0 -> -100
				rPower = -1 * powerSet;
				break;
			case 4:
				lPower = -1 * powerSet;
				rPower = -1 * (powerSet * (225 - powerAngle))/45;	// -100 -> 0 													
				break;
			case 5: 
				lPower = -1 * powerSet;
				rPower = (powerSet * (powerAngle - 225))/45;			// 0 -> 100 													
				break;
			case 6:
				lPower = -1 * (powerSet * (315 - powerAngle))/45;	// -100 -> 0
				rPower = powerSet; 																
				break;
			case 7:
				lPower = (powerSet * (powerAngle - 315))/45;			// 0 -> 100
				rPower = powerSet; 																
				break;
			default:
				break;
		}
		//Going backwards
		if(lPower < 0){
			Left_DutyB(lPower * -120, 1);
		}
		else{
			Left_DutyB(lPower * 120, 0);
		}
		// Going backwards
		if(rPower < 0){
			Right_DutyB(rPower * -120, 0);
		}
		else{
			Right_DutyB(rPower * 120, 1);
		}
}

/* 
 * PWM Control
 * 
 *          (0,1) (1,1)	(1,0)
 *							 \	|	 /
 *								\	|	/
 *			(-1,1)______|_______   (1,-1)
 *								 /|\
 *								/	| \
 *							 /	|  \
 *				(-1,0) (-1,-1)  (0,-1)
 *
 * Pause when going + to - for motor to work effectively
 *
*/

// SW2 cycles through 12 positions
// mode=0 1875,...,1275
// mode=1 1870,...,2375
uint32_t Steering;     // 625 to 3125
uint32_t SteeringMode; // 0 for increase, 1 for decrease
#define POWERMIN 400
#define POWERMAX 12400
#define POWERDELTA 2000
uint32_t Power;


/*
 * Test main for the robot run sequence 
 * 
 *
 */
int main(void){  
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init();  
  OutputUART_Init();       // UART0 only used for debugging
//  printf("\n\r-----------\n\rSystem starting...\n\r");
  ESP8266_InitUART(9600,false);      // connect to access point, set up as receiver
	//ESP8266_Init(9600);
	ESP8266_EnableRXInterrupt();
  Output_Init();					 // ST7735 Init
	// For music & Sound 
	DAC_Init(0x1000); 	// initialize with command: Vout = Vref --- see video for details
  SysTick_Init();  		// initiate for frequency control
	Timer0A_Init(&songMaker, 24150944); //3773585);										// initialize timer0a for note length
	//
	
	// For PWM Motor control 
	Power = POWERMAX/4;
  Left_InitB(12500, Power,0);          // initialize PWM0, 100 Hz
  Right_InitB(12500, 12500-Power,1);   // initialize PWM0, 100 Hz 
	//
	EnableInterrupts();
	//
	//
	// TODO: decipher commands and set globals based on input
	//  - PD6 is standby for h-bridge
	// 	- LED for car headlights/taillights
	//  	Possible ideas:
	//		- Button for switching what is displayed on LCD
	//  	- Bumper possibility 
	//  	- Switch songs/sounds button
	// 		- Automation with sensor 
	// 		- Keyboard sending message
	// 		- Interpret typed language
	
	//ESP8266_GetVersionNumber();
  while(1){
    if(processBuff == 1){
			setRobotStatus();
		}
		/* Still need to work on button control
		if (PP_Button == 1){
			//LED_RedOn();
			songMaker();
		}*/
  }
}



