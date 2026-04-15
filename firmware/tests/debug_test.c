#include "Debugging.h"
#include "FreeRTOS.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define TEST_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t Test_Task_TCB;
StackType_t Test_Task_Stack_Array[TEST_TASK_STACK_SIZE];

void DebugTask(void *argument) {
	while (1) {
		printf("\033[2J");
		led_toggle(PSOM_HB_PORT, PSOM_HB_PIN);


		printf("Mika Sucks Balls\n\r");
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

int main(void) {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	Status_LEDs_Init();
	sensors_adc_GPIO_init();
	if (sensors_adc_init() != ADC_OK)
		Error_Handler();


	xTaskCreateStatic(DebugTask, 
					 "Debugging sucks", 
					 TEST_TASK_STACK_SIZE, 
					 NULL,
					 TEST_TASK_PRIORITY, 
					 Test_Task_Stack_Array, 
					 &Test_Task_TCB

	);
	
	vTaskStartScheduler();

	while (1) {
		// printf("Mika Sucks Balls\n");
		// HAL_Delay(500);
	}
}