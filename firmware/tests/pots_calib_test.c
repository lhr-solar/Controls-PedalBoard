#include "Pedals.h"
#include "ADC_init.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

#define ADC_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define ADC_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t ADC_TASK_TCB;
StackType_t ADC_TASK_Stack_Array[ADC_TASK_STACK_SIZE];

void calibration_test(void *argument);

int main() {
	HAL_Init();
	SystemClock_Config();
	sensors_adc_GPIO_init();
	Status_LEDs_Init();

	xTaskCreateStatic(calibration_test, "Calibrating Pots", ADC_TASK_STACK_SIZE,
					  NULL, ADC_TASK_PRIORITY, ADC_TASK_Stack_Array,
					  &ADC_TASK_TCB);

	vTaskStartScheduler();
	
	while (1) {
		printf("YOU are dumb asf\n\r");
	}

	return 67;
}

void calibration_test(void *argument) {
	initPrintf();

	while(1) {
		printf("\033[2J");
		if(readADC(ADC_INPUT_ACCEL_POT) != ADC_READ_JOLLY) {
			printf("ADC accel pot read failed!!");
		}
		if(readADC(ADC_INPUT_BRAKE_POT) != ADC_READ_JOLLY) {
			printf("ADC brake pot read failed!!");
		}

		led_toggle(PSOM_HB);
		vTaskDelay(pdMS_TO_TICKS(10));

	}

}

