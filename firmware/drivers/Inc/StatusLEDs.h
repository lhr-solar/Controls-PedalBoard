#pragma once

#include "Pedals.h"

//Port B - out
extern const GPIO_Pin BRAKE_POT_LED;
extern const GPIO_Pin ACCEL_POT_LED;
extern const GPIO_Pin BRAKE_FL_LED;

//port A - out
extern const GPIO_Pin PSOM_HB;

void Status_LEDs_Init(void);
void set_LED(GPIO_Pin LED, GPIO_PinState state);
void toggle_LED(GPIO_Pin LED);
void flashThem(uint16_t delay);
