// PWM.h
// Runs on TM4C123 (not LM4F120)
// Use PWM0A/PB6 and PWM0B/PB7 to generate pulse-width modulated outputs
// Use PWM1A/PB4 and PWM1B/PB5 to generate pulse-width modulated outputs
// Use PWM3A/PD3 to generate pulse-width modulated output

// Daniel Valvano
// February 2, 2017
// ***************************************************
// ************remove R9 and R10**********************
// ***************************************************
// PB7 A+  PWM 100 Hz, right motor, PWM positive logic
// PB6 A-  regular GPIO, right motor, 0 means forward
// PB5 B+  PWM 100 Hz, left motor, PWM negative logic
// PB4 B-  regular GPIO, left motor, 1 means forward
// PD0 is servo A, 20ms period, pulse time 0.5 to 2.5ms
// PD1 was servo B, but no software written for PD1

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2017
   Program 8.4, Section 8.3

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB7 (0 or 1)
// Output on PB6/M0PWM0
// GPIO on   PB7 controls direction
void Right_Init(uint16_t period, uint16_t duty, int direction);

// change duty cycle of PB6
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB7(0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Right_Duty(uint16_t duty, int direction);


// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB6 (0 or 1)
// Output on PB7/M0PWM0B
// GPIO on   PB6 controls direction
void Right_InitB(uint16_t period, uint16_t duty, int direction);

// change duty cycle of PB7
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB6(0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Right_DutyB(uint16_t duty, int direction);

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// Output on PB4/M0PWM1A
// GPIO on   PB5 controls direction
void Left_Init(uint16_t period, uint16_t duty, int direction);

// change duty cycle of PB4
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Left_Duty(uint16_t duty, int direction);


// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB4 (0 or 1)
// Output on PB5/M0PWM1B
// GPIO on   PB4 controls direction
void Left_InitB(uint16_t period, uint16_t duty, int direction);

// change duty cycle of PB5
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB4 (0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Left_DutyB(uint16_t duty, int direction);

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
// Output on PD0/M0PWM3A is PWM6
void Servo_Init(uint16_t period, uint16_t duty);

// change duty cycle of PD0
// Inputs: period was set in call to Init
//         duty is in 800 ns units
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Servo_Duty(uint16_t duty);
