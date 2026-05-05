#include "Pedals.h"
#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"
#include "stm32xx_hal.h"

StaticTask_t Init_Task_TCB;
StackType_t Init_Task_Stack_Array[INIT_TASK_STACK_SIZE];

StaticSemaphore_t pedals_sample_ready_sem_storage;
SemaphoreHandle_t pedals_sample_ready_sem;


int main() {

	xTaskCreateStatic(Task_InitAll, 
					"Initialization", 
					INIT_TASK_STACK_SIZE,
					NULL, 
					INIT_TASK_PRIORITY, 
					Init_Task_Stack_Array,
					&Init_Task_TCB

	);

	vTaskStartScheduler();

	while (1) {
	}

	return 0;
}

void Task_InitAll(void *argument) {
	(void)argument;

	HAL_Init();
	SystemClock_Config();
	initPrintf();
	sensors_adc_GPIO_init();
	Status_LEDs_Init();

	if (sensors_adc_init() != ADC_OK)
		Pedals_Error_Handler();

	if (pedals_CAN_init() != PEDALS_OK)
		Pedals_Error_Handler();

	pedals_sample_ready_sem = xSemaphoreCreateBinaryStatic(&pedals_sample_ready_sem_storage);
	if (pedals_sample_ready_sem == NULL)
		Pedals_Error_Handler();

	xTaskCreateStatic(Task_Pedals, "Pedals", PEDALS_TASK_STACK_SIZE, NULL,
					  PEDALS_TASK_PRIORITY, Task_Pedals_Stack_Array, &Task_Pedals_TCB);
	xTaskCreateStatic(Task_BrakePressure, "BrakePressure", BRAKE_PRESSURE_TASK_STACK_SIZE,
					  NULL, BRAKE_PRESSURE_TASK_PRIORITY, Task_BrakePressure_Stack_Array,
					  &Task_BrakePressure_TCB);
	xTaskCreateStatic(Task_Heartbeat, "Heartbeat", HEARTBEAT_TASK_STACK_SIZE, NULL,
					  HEARTBEAT_TASK_PRIORITY, Task_Heartbeat_Stack_Array,
					  &Task_Heartbeat_TCB);

	vTaskDelete(NULL);
}