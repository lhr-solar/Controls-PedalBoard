#include "Pedals.h"
#include "Pots.h"
#include "StatusLEDs.h"
#include "stm32xx_hal.h"

StaticTask_t INIT_TASK_TCB;
StackType_t INIT_TASK_Stack_Array[INIT_TASK_STACK_SIZE];

int main() {

	xTaskCreateStatic(initAll_Task, "Initialization", INIT_TASK_STACK_SIZE,
					  NULL, INIT_TASK_PRIORITY, INIT_TASK_Stack_Array,
					  &INIT_TASK_TCB

	);

	vTaskStartScheduler();

	while (1) {
		printf("YOU are dumb asf\n\r");
	}

	return 0;
}

void initAll_Task(void *argument) {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	adc_GPIO_init();
	Status_LEDs_Init();

	//init all teh other tasks

	// CAN init
	vTaskDelete(NULL); //delete itself when done
}

extern void ADC_Task(void *argument);