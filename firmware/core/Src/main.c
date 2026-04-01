#include "Pedals.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"
#include "stm32xx_hal.h"

StaticTask_t Init_Task_TCB;
StackType_t Init_Task_Stack_Array[INIT_TASK_STACK_SIZE];

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
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	adc_GPIO_init();
	Status_LEDs_Init();

	// init all teh other tasks

	// CAN init
	vTaskDelete(NULL); // delete itself when done
}