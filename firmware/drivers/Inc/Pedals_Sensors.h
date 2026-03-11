#pragma once

#include "Pedals.h"
#include "ADC_init.h"

extern ADC_ChannelConfTypeDef brakePot_buff;
extern ADC_ChannelConfTypeDef accelPot_buff;
extern ADC_ChannelConfTypeDef brakeFL_front_buff;
extern ADC_ChannelConfTypeDef brakeFL_back_buff;
extern ADC_ChannelConfTypeDef accelPotRed_buff;
extern ADC_ChannelConfTypeDef brakePotRed_buff;

typedef enum {
  ADC_INPUT_ACCEL_POT = 0,     // ACCEL_POT_READ
  ADC_INPUT_BRAKE_POT = 1,     // BRAKE_POT_READ
  ADC_INPUT_BRAKE_FL = 2,      // BRAKE_FL_READ
  ADC_INPUT_BRAKE_FL_RED = 3,  // BRAKE_FL_RED_READ
  ADC_INPUT_ACCEL_POT_RED = 4, // ACCEL_POT_RED_READ
  ADC_INPUT_BRAKE_POT_RED = 5, // BRAKE_POT_RED_READ

  ADC_INPUT_COUNT
} ADCInput;
