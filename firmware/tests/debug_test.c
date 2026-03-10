#include "Debugging.h"
#include "FreeRTOS.h"
#include "Pots.h"
#include "StatusLEDs.h"

#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define TEST_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t TEST_TASK_TCB;
StackType_t TEST_TASK_Stack_Array[TEST_TASK_STACK_SIZE];

void DebugTask(void *argument) {
	while (1) {
		set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(500));
		set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(500));

		printf("Mika Sucks Balls\n");
	}
}

int main(void) {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	Status_LEDs_Init();
	adc_GPIO_init();
	if (pedals_adc_init() != ADC_OK)
		Error_Handler();


	xTaskCreateStatic(DebugTask, "Debugging sucks", TEST_TASK_STACK_SIZE, NULL,
					  TEST_TASK_PRIORITY, TEST_TASK_Stack_Array, &TEST_TASK_TCB

	);
	vTaskStartScheduler();

	while (1) {
#ifdef USART1
		set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
		HAL_Delay(500);
#endif
#ifdef USART1
		set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
		HAL_Delay(500);
#endif

		// printf("Mika Sucks Balls\n");
		// HAL_Delay(500);
	}
}