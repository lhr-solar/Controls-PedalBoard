#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#define ADC1_QUEUE_LENGTH 10
#define ADC_ITEM_SIZE sizeof(uint32_t)

extern uint8_t adc1_brakePot_queue[];
extern StaticQueue_t adc1_brakePot_queue_buffer;
extern QueueHandle_t adc1_brakePot_RecvQ;

extern uint8_t adc1_accelPot_queue[];
extern StaticQueue_t adc1_accelPot_queue_buffer;
extern QueueHandle_t adc1_accelPot_RecvQ;

extern uint8_t adc1_brakeFL_queue[];
extern StaticQueue_t adc1_brakeFL_queueBuffer;
extern QueueHandle_t adc1_brakeFL_RecvQ;

extern uint8_t adc1_brakeFLRed_queue[];
extern StaticQueue_t adc1_brakeFLRed_queueBuffer;
extern QueueHandle_t adc1_brakeFLRed_RecvQ;

extern uint8_t adc1_accelPotRed_queue[];
extern StaticQueue_t adc1_accelPotRed_queue_buffer;
extern QueueHandle_t adc1_accelPotRed_RecvQ;

extern uint8_t adc1_brakePotRed_queue[];
extern StaticQueue_t adc1_brakePotRed_queue_buffer;
extern QueueHandle_t adc1_brakePotRed_RecvQ;

extern const GPIO_Pin BRAKE_POT;
extern const GPIO_Pin ACCEL_POT;
extern const GPIO_Pin BRAKE_FL_FRONT;
extern const GPIO_Pin BRAKE_FL_BACK;
extern const GPIO_Pin ACCEL_POT_RED;
extern const GPIO_Pin BRAKE_POT_RED;

extern uint16_t adcPercentPotsLUT[4096];


extern ADC_HandleTypeDef *hadc1;

void adc_GPIO_init();                                            //Initializes GPIO properties for ADC pins
adc_status_t adc_start_read(ADC_ChannelConfTypeDef *adcPin);     //reads a specific adcPin for data
adc_status_t pedals_adc_init();                                  //initializes all ADC channels on pedals
void Error_Handler(void);

void readAll_ADCs_task(void *argument);                          //task for reading all ADCs at once