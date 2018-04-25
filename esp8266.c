// esp8266.c
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
/* 
  Author: Steven Prickett (steven.prickett@gmail.com)
 
  THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
  OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
  VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
  OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

*/
// http://www.xess.com/blog/esp8266-reflash/

// NOTE: ESP8266 resources below:
// General info and AT commands: http://nurdspace.nl/ESP8266
// General info and AT commands: http://www.electrodragon.com/w/Wi07c
// Community forum: http://www.esp8266.com/
// Offical forum: http://bbs.espressif.com/
// example http://zeflo.com/2014/esp8266-weather-display/

/* Modified by Jonathan Valvano
 September 14, 2016
 Hardware connections
 Vcc is a separate regulated 3.3V supply with at least 215mA
 /------------------------------\
 |              chip      1   8 |
 | Ant                    2   7 |
 | enna       processor   3   6 |
 |                        4   5 |
 \------------------------------/ 
ESP8266    TM4C123 
  1 URxD    PB1   UART out of TM4C123, 115200 baud
  2 GPIO0         +3.3V for normal operation (ground to flash)
  3 GPIO2         +3.3V
  4 GND     Gnd   GND (70mA)
  5 UTxD    PB0   UART out of ESP8266, 115200 baud
  6 Ch_PD         chip select, 10k resistor to 3.3V
  7 Reset   PB5   TM4C123 can issue output low to cause hardware reset
  8 Vcc           regulated 3.3V supply with at least 70mA
 */

/*
===========================================================
==========          CONSTANTS                    ==========
===========================================================
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "esp8266.h"
#include "UART.h"
#include "main.h"
// Access point parameters
#define SSID_NAME  "ValvanoAP"
#define PASSKEY    "12345678"
//#define SEC_TYPE   ESP8266_ENCRYPT_MODE_WPA2_PSK

#define BUFFER_SIZE 15 //20 //1024
#define MAXTRY 10
// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
// prototypes for helper functions
void DelayMs(uint32_t delay);
void ESP8266ProcessInput(const char* buffer);
void ESP8266HandleInputCommands(const char* input);
void ESP8266ParseSettingPointers(char* timePtr, char* shotsPtr, char* distPtr);
void ESP8266InitUART(void);
void ESP8266_PrintChar(char iput);
void ESP8266EnableRXInterrupt(void);
void ESP8266DisableRXInterrupt(void);
void ESP8266SendCommand(const char* inputString);
void ESP8266FIFOtoBuffer(void);


/*
=============================================================
==========            GLOBAL VARIABLES             ==========
=============================================================
*/

uint32_t RXBufferIndex = 0;
uint32_t LastReturnIndex = 0;
uint32_t CurrentReturnIndex = 0;
char RXBuffer[BUFFER_SIZE];
char TXBuffer[BUFFER_SIZE];
#define SERVER_RESPONSE_SIZE 1024
char ServerResponseBuffer[SERVER_RESPONSE_SIZE]; // characters after +IPD,
uint32_t ServerResponseIndex = 0;

volatile bool ESP8266_ProcessBuffer = false;
volatile bool ESP8266_EchoResponse = false;
volatile bool ESP8266_ResponseComplete = false;    
volatile bool ESP8266_APEnabled = false;
volatile bool ESP8266_ClientEnabled = false;
volatile bool ESP8266_ServerEnabled = false;
volatile bool ESP8266_InputProcessingEnabled = false;
volatile bool ESP8266_PageRequested = false;

/*
=======================================================================
==========              search FUNCTIONS                     ==========
=======================================================================
*/
char SearchString[32];
volatile bool SearchLooking = false;
volatile bool SearchFound = false;
volatile uint32_t SearchIndex = 0;
char lc(char letter){
  if((letter>='A')&&(letter<='Z')) letter |= 0x20;
  return letter;
}
//-------------------SearchStart -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none
void SearchStart(char *pt){
  strcpy(SearchString,pt);
  SearchIndex = 0;
  SearchFound = false;
  SearchLooking = true;
}
//-------------------SearchCheck -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none
void SearchCheck(char letter){
  if(SearchLooking){
    if(SearchString[SearchIndex] == lc(letter)){ // match letter?
      SearchIndex++;
      if(SearchString[SearchIndex] == 0){ // match string?
        SearchFound = true;
        SearchLooking = false;  
      }        
    }else{
      SearchIndex = 0; // start over
    }
  }
}

char ServerResponseSearchString[16]="+ipd,";
volatile uint32_t ServerResponseSearchFinished = false;
volatile uint32_t ServerResponseSearchIndex = 0;
volatile uint32_t ServerResponseSearchLooking = 0;

//-------------------ServerResponseSearchStart -------------------
// - start looking for server response string in received data stream
// Inputs: none
// Outputs: none
void ServerResponseSearchStart(void){
  strcpy(ServerResponseSearchString,"+ipd,");
  ServerResponseSearchIndex = 0;
  ServerResponseSearchLooking = 1; // means look for "+IPD"
  ServerResponseSearchFinished = 0;
  ServerResponseIndex = 0;
}

//-------------------ServerResponseSearchCheck -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none


//Commented out


void ServerResponseSearchCheck(char letter){
  if(ServerResponseSearchLooking==1){
    if(ServerResponseSearchString[ServerResponseSearchIndex] == lc(letter)){ // match letter?
      ServerResponseSearchIndex++;
      if(ServerResponseSearchString[ServerResponseSearchIndex] == 0){ // match string?
        ServerResponseSearchLooking = 2; 
        strcpy(ServerResponseSearchString,"\r\nok\r\n"); 
        ServerResponseSearchIndex = 0;        
      }        
    }else{
      ServerResponseSearchIndex = 0; // start over
    }
  }else if(ServerResponseSearchLooking==2){
    if(ServerResponseIndex < SERVER_RESPONSE_SIZE){
      ServerResponseBuffer[ServerResponseIndex] = lc(letter); // copy data from "+IPD," to "OK"
      ServerResponseIndex++;
    }
    if(ServerResponseSearchString[ServerResponseSearchIndex] == lc(letter)){ // match letter?
      ServerResponseSearchIndex++;
      if(ServerResponseSearchString[ServerResponseSearchIndex] == 0){   // match OK string?
        ServerResponseSearchFinished = 1;
        ServerResponseSearchLooking = 0;    
      }        
    }else{
      ServerResponseSearchIndex = 0; // start over
    }
  }
}
/*
=======================================================================
==========         UART1 and private FUNCTIONS               ==========
=======================================================================
*/

//------------------- ESP8266_InitUART-------------------
// intializes uart and gpio needed to communicate with esp8266
// Configure UART1 for serial full duplex operation
// Inputs: baud rate (e.g., 115200 or 9600)
//         echo to UART0?
// Outputs: none
void ESP8266_InitUART(uint32_t baud, int echo){ volatile int delay;
  ESP8266_EchoResponse = echo;
  SYSCTL_RCGCUART_R |= 0x02; // Enable UART1
  while((SYSCTL_PRUART_R&0x02)==0){};
  SYSCTL_RCGCGPIO_R |= 0x02; // Enable PORT B clock gating
  while((SYSCTL_PRGPIO_R&0x02)==0){}; 
  GPIO_PORTB_AFSEL_R |= 0x03;
  GPIO_PORTB_DIR_R |= 0x20; // PB5 output to reset
  GPIO_PORTB_PCTL_R =(GPIO_PORTB_PCTL_R&0xFF0FFF00)|0x00000011;
  GPIO_PORTB_DEN_R   |= 0x23; //23 
  GPIO_PORTB_DATA_R |= 0x20; // reset high
  UART1_CTL_R &= ~UART_CTL_UARTEN;                  // Clear UART1 enable bit during config
  UART1_IBRD_R = 5000000/baud;   
  UART1_FBRD_R = ((64*(5000000%baud))+baud/2)/baud;      
   // UART1_IBRD_R = 43;       // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
   // UART1_FBRD_R = 26;       // FBRD = round(0.4028 * 64 ) = 26

  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);  // 8 bit word length, 1 stop, no parity, FIFOs enabled
  UART1_IFLS_R &= ~0x3F;                            // Clear TX and RX interrupt FIFO level fields
  UART1_IFLS_R += UART_IFLS_RX1_8 ;                 // RX FIFO interrupt threshold >= 1/8th full
  UART1_IM_R  |= UART_IM_RXIM | UART_IM_RTIM;       // Enable interupt on RX and RX transmission end
  UART1_CTL_R |= UART_CTL_UARTEN;                   // Set UART1 enable bit    
}
// -----------UART1_Handler-----------
// called on one receiver data input followed by timeout
// or     on going from 1 to 2 data input characters
void UART1_Handler(void){
	//long sr = StartCritical();
  if(UART1_RIS_R & UART_RIS_RXRIS){   // rx fifo >= 1/8 full
    UART1_ICR_R = UART_ICR_RXIC;      // acknowledge interrupt
    ESP8266FIFOtoBuffer();
  }
  if(UART1_RIS_R & UART_RIS_RTRIS){   // receiver timed out
    UART1_ICR_R = UART_ICR_RTIC;      // acknowledge receiver time
    ESP8266FIFOtoBuffer();
  }
	//EndCritical(sr);
}
//--------ESP8266_EnableRXInterrupt--------
// - enables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_EnableRXInterrupt(void){
  NVIC_EN0_R = 1<<6; // interrupt 6
}

//--------ESP8266_DisableRXInterrupt--------
// - disables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_DisableRXInterrupt(void){
  NVIC_DIS0_R = 1<<6; // interrupt 6    
}

//--------ESP8266_PrintChar--------
// prints a character to the esp8226 via uart
// Inputs: character to transmit
// Outputs: none
// busy-wait synchronization
void ESP8266_PrintChar(char input){
  while((UART1_FR_R&UART_FR_TXFF) != 0) {};
  UART1_DR_R = input;
//  UART_OutChar(input); // echo debugging
}
//----------ESP8266FIFOtoBuffer----------
// - copies uart fifo to RXBuffer, using a circular MACQ to store the last data
// - NOTE: would probably be better to use a software defined FIFO here
// - LastReturnIndex is index to previous \n
// Inputs: none
// Outputs:none
void ESP8266FIFOtoBuffer(void){
  char letter;
  while((UART1_FR_R&UART_FR_RXFE) == 0){
    letter = UART1_DR_R;        // retrieve char from FIFO
    /*if(ESP8266_EchoResponse){
      UART_OutCharNonBlock(letter); // echo
    }*/
    if(RXBufferIndex >= BUFFER_SIZE){
      RXBufferIndex = 0; // store last BUFFER_SIZE received
    }
    RXBuffer[RXBufferIndex] = letter; // put char into buffer
    RXBufferIndex++; // increment buffer index 
    SearchCheck(letter);               // check for end of command
    ServerResponseSearchCheck(letter); // check for server response
    if(letter == '\n'){
      LastReturnIndex = CurrentReturnIndex;
      CurrentReturnIndex = RXBufferIndex;
    }
  }
	/*Rotate string until uniform
	while(RXBuffer[0] != 'b' || RXBuffer[1] != '1' || RXBuffer[3] != 'b' || RXBuffer[4] != '2' || RXBuffer[6] != 'b' || RXBuffer[7] != '3'){
		char *temp=malloc(1);
    memcpy(temp, RXBuffer, 1);
    memmove(RXBuffer, RXBuffer+1, 18);
    memcpy(RXBuffer +18, temp, 1);
		free(temp);
	}*/
	setBuff(1, RXBuffer);  
}

//---------ESP8266SendCommand-----
// - sends a string to the esp8266 module
// uses busy-wait
// however, hardware has 16 character hardware FIFO
// Inputs: string to send (null-terminated)
// Outputs: none
void ESP8266SendCommand(const char* inputString){
  int index = 0;
  while(inputString[index] != 0){
    ESP8266_PrintChar(inputString[index++]);
  }
}

// DelayMs
//  - busy wait n milliseconds
// Input: time to wait in msec
// Outputs: none
void DelayMs(uint32_t n){
  volatile uint32_t time;
  while(n){
    time = 6665;  // 1msec, tuned at 80 MHz
    while(time){
      time--;
    }
    n--;
  }
}
void DelayMsSearching(uint32_t n){
  volatile uint32_t time;
  while(n){
    time = 4065;  // 1msec, tuned at 80 MHz
    while(time){
      time--;
      if(SearchFound) return;
    }
    n--;
  }
}

/*
=======================================================================
==========          ESP8266 PUBLIC FUNCTIONS                 ==========
=======================================================================
*/
//-------------------ESP8266_Init --------------
// initializes the module as a client
// Inputs: baud rate: tested with 9600 and 115200
// Outputs: none
void ESP8266_Init(uint32_t baud){
  ESP8266_InitUART(baud,true); // baud rate, no echo to UART0
  ESP8266_EnableRXInterrupt();
  /*SearchLooking = false;
  SearchFound = false;
  ServerResponseSearchLooking = 0; // not looking for "+IPD"
  ServerResponseSearchFinished = 0;*/
  EnableInterrupts();
// step 1: AT+RST reset module
  /*printf("ESP8266 Initialization:\n\r");
  //ESP8266_EchoResponse = true; // debugging
  if(ESP8266_Reset()==0){ 
    printf("Reset failure, could not reset\n\r"); while(1){};
  }*/
	ESP8266SendCommand("AT+UART_CUR=115200,8,1,0,0\r\n");
//  UART_InChar();

	ESP8266_InitUART(115200,true);
}

//----------ESP8266_Reset------------
// resets the esp8266 module
// input:  none
// output: 1 if success, 0 if fail
int ESP8266_Reset(){int try=MAXTRY;
  SearchStart("ready");
  while(try){
    GPIO_PORTB_DATA_R &= ~0x20; // reset low
    DelayMs(10);
    GPIO_PORTB_DATA_R |= 0x20; // reset high
    ESP8266SendCommand("AT+RST\r\n");
    DelayMsSearching(500);
    if(SearchFound) return 1; // success
    try--;
  }
  return 0; // fail
}
