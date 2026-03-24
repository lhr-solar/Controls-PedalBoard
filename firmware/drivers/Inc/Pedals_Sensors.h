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
  ADC_INPUT_BRAKE_FL_FRONT = 2,      // BRAKE_FL_READ
  ADC_INPUT_BRAKE_FL_BACK = 3,  // BRAKE_FL_RED_READ
  ADC_INPUT_ACCEL_POT_RED = 4, // ACCEL_POT_RED_READ
  ADC_INPUT_BRAKE_POT_RED = 5, // BRAKE_POT_RED_READ

  ADC_INPUT_COUNT
} ADCInput;

PedalsStatus readADC(ADCInput adc);
void readAll_ADCs_task(void *argument);
adc_status_t adc_start_read(ADC_ChannelConfTypeDef *adcPin);
adc_status_t pedals_adc_init();
void adc_GPIO_init();
void ADC_Error_Handler(void);



