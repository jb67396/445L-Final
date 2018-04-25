// LED.c
// Runs on TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4
// Output to LEDs
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// Feb 23, 2015
#include <stdint.h>
#include "tm4c123gh6pm.h"

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Section 4.2    Program 4.2

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Example 2.3, Program 2.9, Figure 2.36

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register


//------------LED_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is disabled.
// LEDs on PF3,2,1 are enabled
// Input: none
// Output: none
void LED_Init(void){       
  SYSCTL_RCGCGPIO_R |= 0x20;     // 1) activate Port F
  while((SYSCTL_PRGPIO_R & 0x20)!=0x20){}; // wait to finish activating     
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;// 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_CR_R = 0x0F;        // 2b) enable commit for PF4-PF0     
  GPIO_PORTF_AMSEL_R &= ~0x0F;   // 3) disable analog functionality on PF4-PF0     
  GPIO_PORTF_PCTL_R = 0x00000000;// 4) configure PF0-PF4 as GPIO
  GPIO_PORTF_DIR_R = 0x0F;       // 5) make PF3-0 output                        
  GPIO_PORTF_AFSEL_R = 0;        // 6) disable alt funct
  GPIO_PORTF_PUR_R = 0x00;       // enable weak pull-up on none
  GPIO_PORTF_DEN_R = 0x0F;       // 7) enable digital I/O on PF0-PF4
}

//------------LED_RightOn------------
// Turn on right side LED's
// Input: none
// Output: none
void LED_RightOn(void){
  GPIO_PORTF_DATA_R = 0x0A;
}
//------------LED_RightOff------------
// Turn off right side LED's
// Input: none
// Output: none
void LED_RightOff(void){
  GPIO_PORTF_DATA_R = 0x00;
}
//------------LED_RightToggle------------
// Toggle right side LED's
// Input: none
// Output: none
void LED_RightToggle(void){
  GPIO_PORTF_DATA_R ^= 0x0A;
}

//------------LED_LeftToggle------------
// Toggle left side LED's
// Input: none
// Output: none
void LED_LeftToggle(void){
  GPIO_PORTF_DATA_R ^= 0x05;
}
//------------LED_LeftOn------------
// Turn on left side LED's
// Input: none
// Output: none
void LED_LeftOn(void){
  GPIO_PORTF_DATA_R = 0x05;
}
//------------LED_LeftOff------------
// Turn off left side LED's
// Input: none
// Output: none
void LED_LeftOff(void){
  GPIO_PORTF_DATA_R = 0x00;
}


