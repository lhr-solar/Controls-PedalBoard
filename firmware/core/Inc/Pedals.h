#ifndef PEDALS_H
#define PEDALS_H

#include "ADC.h"
#include "Debugging.h"
#include "stm32xx_hal.h"

#define INIT_TASK_PRIORITY (5)
#define INIT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
extern StaticTask_t INIT_TASK_TCB;
extern StackType_t INIT_TASK_Stack_Array[INIT_TASK_STACK_SIZE];

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} GPIO_Pin;


/* --------------------------------------------------
	GPIO/ADC PinDefs
-------------------------------------------------- */


/* -------- ADC --------*/

extern const GPIO_Pin BRAKE_POT;
extern const GPIO_Pin ACCEL_POT;
extern const GPIO_Pin BRAKE_FL_FRONT;
extern const GPIO_Pin BRAKE_FL_BACK;
extern const GPIO_Pin ACCEL_POT_RED;
extern const GPIO_Pin BRAKE_POT_RED;


/* -------- Status LEDS --------*/

//Port B - out
extern const GPIO_Pin BRAKE_POT_LED;
extern const GPIO_Pin ACCEL_POT_LED;
extern const GPIO_Pin BRAKE_FL_LED;

//port A - out
extern const GPIO_Pin PSOM_HB;

/* -------- CAN -------- */
extern const GPIO_Pin PEDALS_CAN_RX;
extern const GPIO_Pin PEDALS_CAN_TX;




/* --------------------------------------------------
	Pedals Status
-------------------------------------------------- */

typedef enum PedalsStatus {
	// Faults
	BRAKE_POT_OK = 1,
	BRAKE_POT_FAIL = 0,
	ACCEL_POT_OK = 1,
	ACCEL_POT_FAIL = 0,
	FL_BRAKE_POT_OK = 1,
	FL_BRAKE_POT_FAIL = 0,

	ADC_READ_FAIL,
	ADC_READ_JOLLY,

	// General Status
	PEDALS_CAN_START_FAIL,
	PEDALS_CAN_INIT_FAIL,
	PEDALS_CAN_SEND_FAIL,
	PEDALS_CAN_STOP_FAIL,
	PEDALS_OK

} PedalsStatus;

/* -------------------------------------------------- */

void initAll_Task(void *argument);

void SystemClock_Config(void);

void HeartBeat();

#endif