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
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "LED.h"
#include "main.h"

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


volatile int processBuff = 0;
char* instructionBuffer;



char Fetch[] = "GET /data/2.5/weather?q=Austin%20Texas&APPID=1234567890abcdef1234567890abcdef HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n";
// 1) go to http://openweathermap.org/appid#use 
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 1234567890abcdef1234567890abcdef with your APPID
void setBuff(int value, char* buffer){
	processBuff = value;
	instructionBuffer = buffer;
}

void setRobotStatus(){
		// This is just for debugging
		// Will have globals set based off inputs
		// Display will be triggered off global flag updates
		processBuff = 0;
		ST7735_SetCursor(0,0);
		// Button 1 is pressed
		if(instructionBuffer[2] == '1'){
			ST7735_OutString("Button 1: 1");
		}
		else{
			ST7735_OutString("Button 1: 0");
		}
		ST7735_SetCursor(0,1);
		// Button 2 is pressed
		if(instructionBuffer[4] == '1'){
			ST7735_OutString("Button 2: 1");
		}
		else{
			ST7735_OutString("Button 2: 0");
		}
		ST7735_SetCursor(0,2);
		// Button 3 is pressed
		if(instructionBuffer[6] == '1'){
			ST7735_OutString("Button 3: 1");
		}
		else{
			ST7735_OutString("Button 3: 0");
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
		ST7735_OutChar(instructionBuffer[11]);
		ST7735_OutChar(instructionBuffer[12]);
		ST7735_OutChar(instructionBuffer[13]);
		ST7735_OutChar(instructionBuffer[14]);
		//ST7735_OutChar(instructionBuffer[18]);
		ST7735_SetCursor(0,5);
}



int main3(void){  
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init();  
  OutputUART_Init();       // UART0 only used for debugging
  printf("\n\r-----------\n\rSystem starting...\n\r");
  ESP8266_Init(9600);      // connect to access point, set up as client
  Output_Init();					 // ST7735 Init
	//
	//
	// TODO: set the receive to be on a timer of 30 Hz
	//  - decipher commands and set globals based on input
	//	- display globals on LCD
	//
	
	ESP8266_GetVersionNumber();
  while(1){
    ESP8266_GetStatus();
    if(ESP8266_MakeTCPConnection("api.openweathermap.org")){ // open socket in server
      LED_GreenOn();
      ESP8266_SendTCP(Fetch);
    }
    ESP8266_CloseTCPConnection();
    while(Board_Input()==0){// wait for touch
    }; 
    LED_GreenOff();
    LED_RedToggle();
  }
}
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
	ESP8266_EnableRXInterrupt();
  Output_Init();					 // ST7735 Init
	EnableInterrupts();
	//
	//
	// TODO: decipher commands and set globals based on input
	//	- display globals on LCD
	//  - handle motor controls
	//  - handle sound
	
	//ESP8266_GetVersionNumber();
  while(1){
    if(processBuff == 1){
			setRobotStatus();
		}
  }
}
// transparent mode for testing
void ESP8266SendCommand(char *);
int main2(void){  char data;
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init();  
  Output_Init();       // UART0 as a terminal
  printf("\n\r-----------\n\rSystem starting at 9600 baud...\n\r");
//  ESP8266_Init(38400);
  ESP8266_InitUART(38400,true);
  ESP8266_EnableRXInterrupt();
  EnableInterrupts();
  ESP8266SendCommand("AT+RST\r\n");
  data = UART_InChar();
//  ESP8266SendCommand("AT+UART=115200,8,1,0,3\r\n");
//  data = UART_InChar();
//  ESP8266_InitUART(115200,true);
//  data = UART_InChar();
  
  while(1){
// echo data back and forth
    data = UART_InCharNonBlock();
    if(data){
      ESP8266_PrintChar(data);
    }
  }
}



