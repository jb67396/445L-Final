// DAC.c

// SSI1Clk (SCLK, pin 4) connected to PD0
// SSI1Fss (!CS, pin 2) connected to PD1
// SSI1Tx (DIN, pin 3) connected to PD3
// see Figure 7.19 for complete schematic

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "DAC.h"

//********DAC_Init*****************
// Initialize Max5353 12-bit DAC
// inputs:  initial voltage output (0 to 4095)
// outputs: none
// assumes: system clock rate less than 20 MHz
void DAC_Init(uint16_t data){
  SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI1
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// ready?
  GPIO_PORTD_AFSEL_R |= 0x0B;     // enable alt funct on PD0-1-3
  GPIO_PORTD_DEN_R |= 0x0B;       // configure PD0-1-3 as SSI
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00002022;
  GPIO_PORTD_AMSEL_R = 0;         // disable analog functionality on PD
  SSI1_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI1_CPSR_R = 0x0A;             // 8 MHz SSIClk 
  SSI1_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 1, SPO = 0 Freescale  // look up
  SSI1_CR0_R |= 0x8F;             // DSS = 16-bit data
  SSI1_DR_R = data;               // load 'data' into transmit FIFO
  SSI1_CR1_R |= 0x00000002;       // enable SSI

}



//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
void DAC_Out(uint16_t code){   
  while((SSI1_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI1_DR_R = code; }                // data out, no reply
  


