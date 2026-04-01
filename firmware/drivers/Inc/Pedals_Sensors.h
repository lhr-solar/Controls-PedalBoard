#pragma once

#include "FreeRTOS.h"
#include "Pedals.h"
#include "queue.h"

/* ADC Config Macros */
#define PEDALS_ADC1_PRIO 5
#define ADC1_QUEUE_LENGTH 10
#define ADC_ITEM_SIZE sizeof(uint32_t)

/* ADC Sampling Macros for config and user implementations */
#define PEDALS_READ_ADC_SAMPLING_TIME ADC_SAMPLETIME_2CYCLES_5 // for config
#define PEDALS_RECEIVE_ADC_SAMPLING_MS 10						  // for user implementation

extern const uint16_t adcPercentPotsLUT[4096];
extern ADC_HandleTypeDef *hadc1;


/* 
	ADC_Input_t is used to differentiate between the 6 different
	ADC inputs in the adc_receive(...) function
*/
typedef enum {
	ADC_INPUT_ACCEL_POT = 0,	  // ACCEL_POT_READ
	ADC_INPUT_BRAKE_POT = 1,	  // BRAKE_POT_READ
	ADC_INPUT_BRAKE_FL_FRONT = 2, // BRAKE_FL_READ
	ADC_INPUT_BRAKE_FL_BACK = 3,  // BRAKE_FL_REDUNDANT_READ
	ADC_INPUT_ACCEL_POT_REDUNDANT = 4,  // ACCEL_POT_REDUNDANT_READ
	ADC_INPUT_BRAKE_POT_REDUNDANT = 5,  // BRAKE_POT_REDUNDANT_READ

	ADC_INPUT_COUNT
} ADC_Input_t;

/**
 * @brief Initializes GPIO pins for ADC Config
 *
 * From CubeMX
 *
 */
void adc_GPIO_init();

/**
 * @brief Reads from a single ADC channel
 *
 * Configs ADC channel queue and puts the ADC value read onto the queue
 *
 * @param channel       Uses Pedals_ADC_Channel enum to differentiate between which
 * 						channel to read
 *
 * @return adc_status_t Returns ADC_OK for a successful read, and other outputs like
 * 						ADC_CHANNEL_CONFIG_FAIL, etc. for specific errors
 * 
 *
 */
adc_status_t adc_start_read(Pedals_ADC_Channel channel); // reads a specific adcPin for data

/**
 * @brief Initializes all ADC channels on Pedals Board
 *
 * Initializes all queues and buffers used to fetch and read ADC
 * values for each of the 6 adc inputs
 *
 * @return adc_status_t Returns ADC_OK for a successful read, and other outputs like
 * 						ADC_CHANNEL_CONFIG_FAIL, etc. for specific errors
 */
adc_status_t pedals_adc_init();

/**
 * @brief Reads specific ADC values from its queue
 *
 * This function reads the ADC values pushed to the queue by adc_start_read().
 * The user can input a specific type of ADC (BrakePot, AccelPot, etc.) to
 * read from a specific queue.
 *
 * @param	adc_input	Input to choose what ADC queue to read (6 options)
 * @param	val         Pointer to val thats populated with the ADC value read from
 * 						the queue
 * 
 * @return Pedals_Status_t  return PEDALS_OK if all is jolly, else get to debugging boi
 * 
 * 
 */
Pedals_Status_t adc_receive(ADC_Input_t adc_input, uint32_t *val);


