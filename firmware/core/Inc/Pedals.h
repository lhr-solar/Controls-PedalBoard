#pragma once

#include "ADC.h"
#include "Debugging.h"
#include "stm32xx_hal.h"
#include "pinDefs.h"
#include "config.h"

/* Task used for initializing all other tasks on Pedals Board */
#define INIT_TASK_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 
#define INIT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
extern StaticTask_t INIT_TASK_TCB;
extern StackType_t INIT_TASK_Stack_Array[INIT_TASK_STACK_SIZE];

/*
*
* 0 - disable printfs
* 1 - enable printfs
*
*/
#define ENABLE_DEBUG 0

/* --------------------------------------------------
	Pedals Status
-------------------------------------------------- */

typedef enum Pedals_Status_t {
	// Faults
	BRAKE_POT_OK,
	BRAKE_POT_FAIL,
	BRAKE_POT_RED_OK,
	BRAKE_POT_RED_FAIL,
	ACCEL_POT_OK,
	ACCEL_POT_FAIL,
	ACCEL_POT_RED_OK,
	ACCEL_POT_RED_FAIL,
	FL_BRAKE_POT_FRONT_OK,
	FL_BRAKE_POT_FRONT_FAIL,
	FL_BRAKE_POT_BACK_OK,
	FL_BRAKE_POT_BACK_FAIL,


	// General Status
	PEDALS_ADC_INIT_FAIL,
	PEDALS_CAN_START_FAIL,
	PEDALS_CAN_INIT_FAIL,
	PEDALS_CAN_SEND_FAIL,
	PEDALS_CAN_STOP_FAIL,
	PEDALS_OK

} Pedals_Status_t;

/* -------------------------------------------------- */


/* --------------------------------------------------
	Pedals ADC Channels
-------------------------------------------------- */

typedef enum Pedals_ADC_Channel{
    BRAKE_POT_BUFF_CHANNEL      = ADC_CHANNEL_7,  // PA2
    ACCEL_POT_BUFF_CHANNEL      = ADC_CHANNEL_5,  // PA0
    BRAKE_FL_FRONT_BUFF_CHANNEL = ADC_CHANNEL_9,  // PA4
    BRAKE_FL_BACK_BUFF_CHANNEL  = ADC_CHANNEL_10, // PA5
    ACCEL_POT_RED_BUFF_CHANNEL  = ADC_CHANNEL_11, // PA6
    BRAKE_POT_RED_BUFF_CHANNEL  = ADC_CHANNEL_15, // PB0
} Pedals_ADC_Channel;

/* -------------------------------------------------- */

/**
 * @brief Initializes peripherals and all other critial tasks.
 *
 * This function initializes all the the GPIO, ADC, and UART (printfs) peripherals
 * and other cirital RTOS tasks that run on the main thread
 *
 * @attention - this task deletes itself after called
 */
void initAll_Task(void *argument);

/**
 * @brief Function used to catch errors in init code
 *
 * Flashes all software LEDs on board when called
 * 
 * @attention - requires board to be reset to go back to
 * 				its normal function
 * 
 */
void Error_Handler(void);