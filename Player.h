// Player.h
// Lab 5
// This file defines the driving functions for DAC.c
// The C file should hold the music structs
// It should have the timer interrupt
// It should recieve the mode information to set what instrument is playing

#include <stdint.h>

void Play_Music(int code);

void StartStop_Button(void);

void Rewind(void);

void Change_Instrument(void);
