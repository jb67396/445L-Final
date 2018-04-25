// LED.h
// Runs on TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4

// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// Feb 23, 2015

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


//------------LED_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is disabled.
// LEDs on PF3,2,1 are enabled
// Input: none
// Output: none
void LED_Init(void);

//------------LED_RightOn------------
// Turn on right side LED's
// Input: none
// Output: none
void LED_RightOn(void);

//------------LED_RightOff------------
// Turn off right side LED's
// Input: none
// Output: none
void LED_RightOff(void);

//------------LED_RightToggle------------
// Toggle right side LED's
// Input: none
// Output: none
void LED_RightToggle(void);

//------------LED_LeftToggle------------
// Toggle left side LED's
// Input: none
// Output: none
void LED_LeftToggle(void);

//------------LED_LeftOn------------
// Turn on left side LED's
// Input: none
// Output: none
void LED_LeftOn(void);

//------------LED_LeftOff------------
// Turn off left side LED's
// Input: none
// Output: none
void LED_LeftOff(void);
