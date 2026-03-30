#include "FreeRTOS.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"
#include "queue.h"

#define ADC1_QUEUE_LENGTH 10
#define ADC_ITEM_SIZE sizeof(uint32_t)

extern ADC_HandleTypeDef *hadc1;

#define PRINT_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define PRINT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t PRINT_TASK_TCB;
StackType_t PRINT_TASK_Stack_Array[PRINT_TASK_STACK_SIZE];

uint8_t adc1Queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1QueueBuffer;
QueueHandle_t adc1RecvQ;

void adc_GPIO_init_test();
adc_status_t adc_start_read_test(ADC_ChannelConfTypeDef *adcPin);
adc_status_t pedals_adc_init_test();
void Error_Handler_test(void);

void ADC_Task(void *argument);

int main() {

	xTaskCreateStatic(ADC_Task, "ADC testing", PRINT_TASK_STACK_SIZE, NULL,
					  PRINT_TASK_PRIORITY, PRINT_TASK_Stack_Array,
					  &PRINT_TASK_TCB

	);

	vTaskStartScheduler();

	return 0;
}

void ADC_Task(void *argument) {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	sensors_adc_GPIO_init();
	Status_LEDs_Init();
	
	if (pedals_adc_init() != ADC_OK)
		Error_Handler_test();

	while (1) {
		if(ENABLE_DEBUG) printf("\033[2J"); //clear printf screen

		if (adc_start_read(BRAKE_POT_BUFF_CHANNEL) != ADC_OK)
			printf("ADC read failed\n\r");

		uint32_t brakePot = 0;
		adc_receive(ADC_INPUT_BRAKE_POT, &brakePot);


		toggle_LED(PSOM_HB_PORT, PSOM_HB_PIN);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


void Error_Handler_test(void) {
	printf("Error Handler: ADC initialization failed\n\r");
	while (1) {
	}
}