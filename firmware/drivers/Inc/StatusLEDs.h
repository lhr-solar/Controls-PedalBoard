#pragma once

#include "Pedals.h"


void Status_LEDs_Init(void);                      // init all LED GPIOs
void set_LED(GPIO_Pin LED, GPIO_PinState state);  // sets LED state to GPIO_PIN_SET (ON) or GPIO_PIN_RESET (OFF)
void toggle_LED(GPIO_Pin LED); 					  // nots the current state of LED (ON -> OFF or OFF -> ON)
void flashThem(uint16_t delay); 				  // Flashes all LEDs :)
