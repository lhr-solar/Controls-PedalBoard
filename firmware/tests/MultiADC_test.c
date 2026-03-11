#include "ADC_init.h"
#include "Pedals.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

#define ADC_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define ADC_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t ADC_TASK_TCB;
StackType_t ADC_TASK_Stack_Array[ADC_TASK_STACK_SIZE];

void multiADC_task(void *argument);

int main() {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	adc_GPIO_init();
	Status_LEDs_Init();

	xTaskCreateStatic(multiADC_task, "Multi ADC testing", ADC_TASK_STACK_SIZE,
					  NULL, ADC_TASK_PRIORITY, ADC_TASK_Stack_Array,
					  &ADC_TASK_TCB);

	vTaskStartScheduler();

	while (1) {
		printf("YOU are dumb asf\n\r");
	}

	return 0;
}

void multiADC_task(void *argument) {
	if (pedals_adc_init() != ADC_OK)
		Error_Handler();
	while (1) {

		for (uint32_t i = 0; i < 50; i++) {
			printf("\033[2J");
		}

		if (adc_start_read(&brakePot_buff) != ADC_OK)
			printf("Brake ADC read failed\n\r");
		if (adc_start_read(&accelPot_buff) != ADC_OK)
			printf("Accel ADC read failed\n\r");
		if (adc_start_read(&brakeFL_buff) != ADC_OK)
			printf("Brake FL ADC read failed\n\r");
		if (adc_start_read(&brakeFLRed_buff) != ADC_OK)
			printf("Brake FL Redundant ADC read failed\n\r");
		if (adc_start_read(&accelPotRed_buff) != ADC_OK)
			printf("Accel Pot Redundant ADC read failed\n\r");
		if (adc_start_read(&brakePotRed_buff) != ADC_OK)
			printf("Brake Pot Redundant ADC read failed\n\r");

		uint32_t brakePot_buff_val = 0;
		uint32_t accelPot_buff_val = 0;
		uint32_t brakeFL_buff_val = 0;
		uint32_t brakeFLRed_buff_val = 0;
		uint32_t accelPotRed_buff_val = 0;
		uint32_t brakePotRed_buff_val = 0;

		/* --------------------------------------------------
			POTS Queue Receives
		  -------------------------------------------------- */
		if (xQueueReceive(adc1_brakePot_RecvQ, &brakePot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake Pot: %lu  |  Brake (%%)): %u%%\n\r",
				   brakePot_buff_val, adcPercentPotsLUT[brakePot_buff_val]);
		} else {
			printf("Failed to receive BrakePot ADC value from queue\n\r");
		}

		if (xQueueReceive(adc1_accelPot_RecvQ, &accelPot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Accel Pot: %lu  |  Accel (%%)): %u%%\n\r",
				   accelPot_buff_val, adcPercentPotsLUT[accelPot_buff_val]);
		} else {
			printf("Failed to receive AccelPot ADC value from queue\n\r");
		}

		/* --------------------------------------------------
			POTS Redundant Queue Receives
		  -------------------------------------------------- */

		if (xQueueReceive(adc1_brakePotRed_RecvQ, &brakePotRed_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf(
				"Brake Pot Redundant: %lu  |  Brake Redundant (%%)): %u%%\n\r",
				brakePotRed_buff_val, adcPercentPotsLUT[brakePotRed_buff_val]);
		} else {
			printf("Failed to receive BrakePot Redundant ADC value from "
				   "queue\n\r");
		}

		if (xQueueReceive(adc1_accelPotRed_RecvQ, &accelPotRed_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf(
				"Accel Pot Redundant: %lu  |  Accel Redundant (%%)): %u%%\n\r",
				accelPotRed_buff_val, adcPercentPotsLUT[accelPotRed_buff_val]);
		} else {
			printf("Failed to receive AccelPot Redundant ADC value from "
				   "queue\n\r");
		}

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL
		   -------------------------------------------------- */
		if (xQueueReceive(adc1_brakeFL_RecvQ, &brakeFL_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake FL: %lu  |  Brake FL (%%)): %u%%\n\r",
				   brakeFL_buff_val, adcPercentPotsLUT[brakeFL_buff_val]);
		} else {
			printf("Failed to receive BrakeFL ADC value from queue\n\r");
		}

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL Redundant
		  -------------------------------------------------- */
		if (xQueueReceive(adc1_brakeFLRed_RecvQ, &brakeFLRed_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake FL Redundant: %lu  |  Brake FL Redundant (%%)): "
				   "%u%%\n\r",
				   brakeFLRed_buff_val, adcPercentPotsLUT[brakeFLRed_buff_val]);
		} else {
			printf(
				"Failed to receive BrakeFL Redundant ADC value from queue\n\r");
		}

		/* --------------------------------------------------
			Input Status LEDs toggle
		  -------------------------------------------------- */
		if (adcPercentPotsLUT[brakePot_buff_val] < 70)
			set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
		else
			set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
		if (adcPercentPotsLUT[accelPot_buff_val] > 20)
			set_LED(ACCEL_POT_LED, GPIO_PIN_SET);
		else
			set_LED(ACCEL_POT_LED, GPIO_PIN_RESET);
		if (adcPercentPotsLUT[brakeFL_buff_val] > 50)
			set_LED(BRAKE_FL_LED, GPIO_PIN_SET);
		else
			set_LED(BRAKE_FL_LED, GPIO_PIN_RESET);

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}