#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "ADC.h"
#include "CarCAN_can_msgs.h"
#include "pinDefs.h"
#include "Debugging.h"


// ADC Config Macros
#define PEDALS_ADC1_PRIO            5
#define ADC1_QUEUE_LENGTH           10
#define ADC_ITEM_SIZE               sizeof(uint32_t)

#define PEDALS_READ_ADC_SAMPLING_TIME   ADC_SAMPLETIME_2CYCLES_5
#define PEDALS_RECEIVE_ADC_SAMPLING_MS  10

#define ENABLE_DEBUG        0
#define ERROR_HANDLER_DELAY pdMS_TO_TICKS(500)

extern uint32_t raw_vals[6];

extern ADC_HandleTypeDef *hadc1;

typedef enum {
    BRAKE_POT_OK,
    BRAKE_POT_FAIL,
    BRAKE_POT_REDUNDANT_OK,
    BRAKE_POT_REDUNDANT_FAIL,
    ACCEL_POT_OK,
    ACCEL_POT_FAIL,
    ACCEL_POT_REDUNDANT_OK,
    ACCEL_POT_REDUNDANT_FAIL,
    FL_BRAKE_POT_FRONT_OK,
    FL_BRAKE_POT_FRONT_FAIL,
    FL_BRAKE_POT_BACK_OK,
    FL_BRAKE_POT_BACK_FAIL,
    PEDALS_ADC_INIT_FAIL,
    PEDALS_CAN_START_FAIL,
    PEDALS_CAN_INIT_FAIL,
    PEDALS_CAN_SEND_FAIL,
    PEDALS_CAN_STOP_FAIL,
    PEDALS_OK
} PedalsStatus_t;

typedef enum {
    BRAKE_POT_BUFF_CHANNEL           = ADC_CHANNEL_7,
    ACCEL_POT_BUFF_CHANNEL           = ADC_CHANNEL_5,
    /* IN9/IN10 ↔ PA4/PA5 (STM32L431); maps to brake_pressure_1 / brake_pressure_2 */
    BRAKE_FL_FRONT_BUFF_CHANNEL      = ADC_CHANNEL_9,
    BRAKE_FL_BACK_BUFF_CHANNEL       = ADC_CHANNEL_10,
    ACCEL_POT_REDUNDANT_BUFF_CHANNEL = ADC_CHANNEL_11,
    BRAKE_POT_REDUNDANT_BUFF_CHANNEL = ADC_CHANNEL_15
} PedalsADCChannel_e;

typedef enum {
    ADC_INPUT_ACCEL_POT,
    ADC_INPUT_BRAKE_POT,
    ADC_INPUT_BRAKE_FL_FRONT,
    ADC_INPUT_BRAKE_FL_BACK,
    ADC_INPUT_ACCEL_POT_REDUNDANT,
    ADC_INPUT_BRAKE_POT_REDUNDANT,
    ADC_INPUT_COUNT
} SensorsADCInput_t;

void sensors_adc_GPIO_init(void);
adc_status_t sensors_adc_init(void);
adc_status_t sensors_adc_start_read(PedalsADCChannel_e channel);
PedalsStatus_t sensors_adc_receive(SensorsADCInput_t adc_input, uint32_t *val);

void readAllADCs(void);
pedal_brake_adc_t read_pedal_brake_adc(void);
pedal_accel_adc_t read_pedal_accel_adc(void);
brake_pressure_1_t read_brake_FL_1_raw_voltage(void);
brake_pressure_2_t read_brake_FL_2_raw_voltage(void);
pedal_status_t read_main_positions_and_faults(void);

void ADC_Error_Handler(void);
