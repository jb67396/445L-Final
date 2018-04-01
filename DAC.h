// Player.h
// Lab 5
// This file defines the driving functions for DAC.c
// The C file should hold the music structs
// It should have the timer interrupt
// It should recieve the mode information to set what instrument is playing

#include <stdint.h>

void DAC_Init(uint16_t data);

void DAC_Out(uint16_t code);
