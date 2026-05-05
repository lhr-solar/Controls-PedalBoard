#pragma once

#include "ADC.h"
#include "Debugging.h"
#include "stm32xx_hal.h"
#include "pinDefs.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Task used for initializing all other tasks on Pedals Board */
#define INIT_TASK_PRIORITY tskIDLE_PRIORITY + 5
/* Init runs HAL/CAN/ADC setup + create tasks — needs headroom above minimal */
#define INIT_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 4u)
extern StaticTask_t Init_Task_TCB;
extern StackType_t Init_Task_Stack_Array[INIT_TASK_STACK_SIZE];

#define HEARTBEAT_TASK_PRIORITY (tskIDLE_PRIORITY + 4)
#define PEDALS_TASK_PRIORITY		  (tskIDLE_PRIORITY + 3)
#define BRAKE_PRESSURE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define HEARTBEAT_TASK_STACK_SIZE	configMINIMAL_STACK_SIZE
#define PEDALS_TASK_STACK_SIZE		  (configMINIMAL_STACK_SIZE * 4u)
#define BRAKE_PRESSURE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 4u)

extern StaticTask_t Task_Heartbeat_TCB;
extern StackType_t Task_Heartbeat_Stack_Array[HEARTBEAT_TASK_STACK_SIZE];
extern StaticTask_t Task_Pedals_TCB;
extern StackType_t Task_Pedals_Stack_Array[PEDALS_TASK_STACK_SIZE];
extern StaticTask_t Task_BrakePressure_TCB;
extern StackType_t Task_BrakePressure_Stack_Array[BRAKE_PRESSURE_TASK_STACK_SIZE];

extern SemaphoreHandle_t pedals_sample_ready_sem;

void Task_Heartbeat(void *argument);
void Task_Pedals(void *argument);
void Task_BrakePressure(void *argument);

/*
*
* 0 - disable printfs
* 1 - enable printfs
*
*/
#define ENABLE_DEBUG		 	1

#define ERROR_HANDLER_DELAY		pdMS_TO_TICKS(250)

/* --------------------------------------------------
	Pedals Status
-------------------------------------------------- */

typedef enum Pedals_Status_t {
	// Faults
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


	// General Status
	PEDALS_ADC_INIT_FAIL,
	PEDALS_CAN_START_FAIL,
	PEDALS_CAN_INIT_FAIL,
	PEDALS_CAN_SEND_FAIL,
	PEDALS_CAN_STOP_FAIL,
	PEDALS_OK

} Pedals_Status_t;

/* -------------------------------------------------- */


/**
 * @brief Maps ADC channels to their physical pins and sensor names.
 *
 * | Channel        | Pin  | Name                |
 * |----------------|------|---------------------|
 * | ADC_CHANNEL_7  | PA2  | Brake Pot           |
 * | ADC_CHANNEL_5  | PA0  | Accel Pot           |
 * | ADC_CHANNEL_9  | PA4  | Brake FL Front      |
 * | ADC_CHANNEL_10 | PA5  | Brake FL Back       |
 * | ADC_CHANNEL_11 | PA6  | Accel Pot Redundant |
 * | ADC_CHANNEL_15 | PB0  | Brake Pot Redundant |
 */
typedef enum {
    BRAKE_POT_BUFF_CHANNEL      = ADC_CHANNEL_7,  // PA2
    ACCEL_POT_BUFF_CHANNEL      = ADC_CHANNEL_5,  // PA0
    BRAKE_FL_FRONT_BUFF_CHANNEL = ADC_CHANNEL_9,  // PA4
    BRAKE_FL_BACK_BUFF_CHANNEL  = ADC_CHANNEL_10, // PA5
    ACCEL_POT_REDUNDANT_BUFF_CHANNEL  = ADC_CHANNEL_11, // PA6
    BRAKE_POT_REDUNDANT_BUFF_CHANNEL  = ADC_CHANNEL_15, // PB0
} Pedals_ADC_Channel;

/* -------------------------------------------------- */

/**
 * @brief Configs System Clock
 *
 * From CubeMX
 *
 */
void SystemClock_Config(void);

/**
 * @brief Initializes peripherals and all other critial tasks.
 *
 * This function initializes all the the GPIO, ADC, and UART (printfs) peripherals
 * and other cirital RTOS tasks that run on the main thread
 *
 * @attention - this task deletes itself after called
 */
void Task_InitAll(void *argument);

/**
 * @brief Function used to catch errors in init code
 *
 * Flashes all software LEDs on board when called
 * 
 * @attention - requires board to be reset to go back to
 * 				its normal function
 * 
 */
void Pedals_Error_Handler(void);