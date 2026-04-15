#pragma once

#include "FreeRTOS.h"
#include "Pedals.h"
#include "queue.h"
#include "CarCAN_can_msgs.h"

/* ADC Config Macros */
#define PEDALS_ADC1_PRIO 5
#define ADC1_QUEUE_LENGTH 10
#define ADC_ITEM_SIZE sizeof(uint32_t)

/* ADC Sampling Macros for config and user implementations */
#define PEDALS_READ_ADC_SAMPLING_TIME  ADC_SAMPLETIME_2CYCLES_5 // for config
#define PEDALS_RECEIVE_ADC_SAMPLING_MS 10						  // for user implementation

extern const uint8_t adcPercentBrakeLUT[4096];
extern const uint8_t adcPercentAccelLUT[4096];
extern const uint8_t adcPercentBrakePressure1LUT[4096];
extern const uint8_t adcPercentBrakePressure2LUT[4096];

extern uint32_t raw_vals[6];

extern ADC_HandleTypeDef *hadc1;

/* 
	Sensors_ADC_Input_t is used to differentiate between the 6 different
	ADC inputs in the adc_receive(...) function
*/
typedef enum {
	ADC_INPUT_ACCEL_POT,	  // ACCEL_POT_READ
	ADC_INPUT_BRAKE_POT,	  // BRAKE_POT_READ
	ADC_INPUT_BRAKE_FL_FRONT, // BRAKE_FL_READ
	ADC_INPUT_BRAKE_FL_BACK,  // BRAKE_FL_REDUNDANT_READ
	ADC_INPUT_ACCEL_POT_REDUNDANT,  // ACCEL_POT_REDUNDANT_READ
	ADC_INPUT_BRAKE_POT_REDUNDANT,  // BRAKE_POT_REDUNDANT_READ

	ADC_INPUT_COUNT
} Sensors_ADC_Input_t;

/**
 * @brief Initializes GPIO pins for ADC Config
 *
 * From CubeMX
 *
 */
void sensors_adc_GPIO_init();

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
adc_status_t sensors_adc_start_read(Pedals_ADC_Channel channel);

/**
 * @brief Initializes all ADC channels on Pedals Board
 *
 * Initializes all queues and buffers used to fetch and read ADC
 * values for each of the 6 adc inputs
 *
 * @return adc_status_t Returns ADC_OK for a successful read, and other outputs like
 * 						ADC_CHANNEL_CONFIG_FAIL, etc. for specific errors
 */
adc_status_t sensors_adc_init();

/**
 * @brief Reads specific ADC values from its queue
 *
 * This function reads the ADC values pushed to the queue by sensors_adc_start_read().
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
Pedals_Status_t sensors_adc_receive(Sensors_ADC_Input_t adc_input, uint32_t *val);

void readAll_ADCs(); // continuously reads all ADC channels and updates global structs

pedal_brake_rawv_t read_brake_raw_voltage();
pedal_accel_rawv_t read_accel_raw_voltage();
brake_pressure_1_t read_brakeFL_1_raw_voltage();
brake_pressure_2_t read_brakeFL_2_raw_voltage();

pedal_status_t read_main_positions_and_faults();

void ADC_Error_Handler(void);

