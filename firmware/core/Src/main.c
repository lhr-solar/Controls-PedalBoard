#include "InitTask.h"
#include "stm32xx_hal.h"

StaticTask_t InitTaskTCB;
StackType_t  InitTaskStackArray[INIT_TASK_STACK_SIZE];

int main(void) {
    // Creates the initialization task. This task will initialize hardware,
    // spin up the other RTOS tasks, and then delete itself.
    xTaskCreateStatic(
		Task_Init, 
		"Initialization", 
		INIT_TASK_STACK_SIZE, 
		NULL, 
		INIT_TASK_PRIORITY, 
		InitTaskStackArray, 
		&InitTaskTCB
	);

    vTaskStartScheduler();

    while (1) {
	}

    return 0;
}