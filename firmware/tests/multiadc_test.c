#include "Pedals.h"
#include "StatusLEDs.h"
#include "readAllADCs.h"

#define ADC_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define ADC_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t ADC_Task_TCB;
StackType_t ADC_Task_Stack_Array[ADC_TASK_STACK_SIZE];

int main() {
	HAL_Init();
	SystemClock_Config();
	initPrintf();
	sensors_adc_GPIO_init();
	Status_LEDs_Init();
 
	xTaskCreateStatic(readAll_ADCs_task, 
					  "Multi ADC testing", 
					  ADC_TASK_STACK_SIZE,
					  NULL, ADC_TASK_PRIORITY, 
					  ADC_Task_Stack_Array,
					  &ADC_Task_TCB
	);

	vTaskStartScheduler();

	while (1) {}

	return 0;
}



void readAll_ADCs_task(void *argument) {
	if (pedals_adc_init() != ADC_OK) {
		if (ENABLE_DEBUG) printf("ADC_Init Failed");
		return;
	}
	while (1) {
		if (ENABLE_DEBUG) printf("\033[2J");

		if (adc_start_read(BRAKE_POT_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Brake Pot ADC read failed\n\r");
		if (adc_start_read(ACCEL_POT_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Accel Pot ADC read failed\n\r");
		if (adc_start_read(BRAKE_FL_FRONT_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Brake FL Front ADC read failed\n\r");
		if (adc_start_read(BRAKE_FL_BACK_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Brake FL Back ADC read failed\n\r");
		if (adc_start_read(ACCEL_POT_REDUNDANT_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Accel Pot Redundant ADC read failed\n\r");
		if (adc_start_read(BRAKE_POT_REDUNDANT_BUFF_CHANNEL) != ADC_OK)
			if (ENABLE_DEBUG) printf("Brake Pot Redundant ADC read failed\n\r");

		uint32_t brakePot_buff_val = 0;
		uint32_t accelPot_buff_val = 0;
		uint32_t brakeFL_front_buff_val = 0;
		uint32_t brakeFL_back_buff_val = 0;
		uint32_t accelPotRed_buff_val = 0;
		uint32_t brakePotRed_buff_val = 0;

		/* --------------------------------------------------
			POTS Queue Receives
		  -------------------------------------------------- */
		adc_receive(ADC_INPUT_BRAKE_POT, &brakePot_buff_val);
		if (ENABLE_DEBUG) printf("Brake Pot: %lu  |  LUT (%%): %u%%\n\r", brakePot_buff_val, adcPercentPotsLUT[brakePot_buff_val]);
		adc_receive(ADC_INPUT_ACCEL_POT, &accelPot_buff_val);
		if (ENABLE_DEBUG) printf("Accel Pot: %lu  |  LUT (%%): %u%%\n\r", accelPot_buff_val, adcPercentPotsLUT[accelPot_buff_val]);

		/* --------------------------------------------------
			POTS Redundant Queue Receives
		  -------------------------------------------------- */
		adc_receive(ADC_INPUT_BRAKE_POT_REDUNDANT, &brakePotRed_buff_val);
		if (ENABLE_DEBUG) printf("Brake Pot Redundant: %lu  |  LUT (%%): %u%%\n\r", brakePotRed_buff_val, adcPercentPotsLUT[brakePotRed_buff_val]);
		adc_receive(ADC_INPUT_ACCEL_POT_REDUNDANT, &accelPotRed_buff_val);
		if (ENABLE_DEBUG) printf("Accel Pot Redundant: %lu  |  LUT (%%): %u%%\n\r", accelPotRed_buff_val, adcPercentPotsLUT[accelPotRed_buff_val]);

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL_Front
		   -------------------------------------------------- */
		adc_receive(ADC_INPUT_BRAKE_FL_FRONT, &brakeFL_front_buff_val);
		//if (ENABLE_DEBUG) printf("Brake FL Front: %lu  |  LUT (%%): %u%%\n\r", brakeFL_front_buff_val, adcPercentPotsLUT[brakeFL_front_buff_val]);

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL_Back
		  -------------------------------------------------- */
		adc_receive(ADC_INPUT_BRAKE_FL_BACK, &brakeFL_back_buff_val);
		//if (ENABLE_DEBUG) printf("Brake FL Back: %lu  |  LUT (%%): %u%%\n\r", brakeFL_back_buff_val, adcPercentPotsLUT[brakeFL_back_buff_val]);



		/* --------------------------------------------------
			Input Status LEDs toggle (processing data into array)
		  -------------------------------------------------- */
		set_LED(BRAKE_POT_LED_PORT, BRAKE_POT_LED_PIN, adcPercentPotsLUT[brakePot_buff_val]      > 50 ? GPIO_PIN_SET : GPIO_PIN_RESET);
		set_LED(ACCEL_POT_LED_PORT, ACCEL_POT_LED_PIN, adcPercentPotsLUT[accelPot_buff_val]      > 50 ? GPIO_PIN_SET : GPIO_PIN_RESET);
		set_LED(BRAKE_FL_LED_PORT,  BRAKE_FL_LED_PIN,  adcPercentPotsLUT[brakeFL_front_buff_val] > 50 ? GPIO_PIN_SET : GPIO_PIN_RESET);

	}
}