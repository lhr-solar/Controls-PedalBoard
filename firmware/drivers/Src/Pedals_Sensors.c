#include "Pedals_Sensors.h"
#include "ADC_init.h"
#include "Pedals.h"
#include "StatusLEDs.h"

/* --------------------------------------------------
	  ADC Queue Declarations and GPIO Pin Configurations
   -------------------------------------------------- */

uint8_t adc1_brakePot_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_brakePot_queueBuffer;
QueueHandle_t adc1_brakePot_RecvQ;

uint8_t adc1_accelPot_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_accelPot_queue_buffer;
QueueHandle_t adc1_accelPot_RecvQ;

uint8_t adc1_brakeFL_front_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_brakeFL_front_queueBuffer;
QueueHandle_t adc1_brakeFL_front_RecvQ;

uint8_t adc1_brakeFL_back_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_brakeFL_back_queueBuffer;
QueueHandle_t adc1_brakeFL_back_RecvQ;

uint8_t adc1_accelPotRed_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_accelPotRed_queue_buffer;
QueueHandle_t adc1_accelPotRed_RecvQ;

uint8_t adc1_brakePotRed_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
StaticQueue_t adc1_brakePotRed_queue_buffer;
QueueHandle_t adc1_brakePotRed_RecvQ;

ADC_ChannelConfTypeDef brakePot_buff = {.Channel = ADC_CHANNEL_7, // PA2
										.Rank = ADC_REGULAR_RANK_1,
										.SamplingTime =
											ADC_SAMPLETIME_2CYCLES_5,
										.SingleDiff = ADC_SINGLE_ENDED,
										.OffsetNumber = ADC_OFFSET_NONE,
										.Offset = 0};

ADC_ChannelConfTypeDef accelPot_buff = {.Channel = ADC_CHANNEL_5, // PA0
										.Rank = ADC_REGULAR_RANK_1,
										.SamplingTime =
											ADC_SAMPLETIME_2CYCLES_5,
										.SingleDiff = ADC_SINGLE_ENDED,
										.OffsetNumber = ADC_OFFSET_NONE,
										.Offset = 0};

ADC_ChannelConfTypeDef brakeFL_front_buff = {.Channel = ADC_CHANNEL_9, // PA4
											 .Rank = ADC_REGULAR_RANK_1,
											 .SamplingTime =
												 ADC_SAMPLETIME_2CYCLES_5,
											 .SingleDiff = ADC_SINGLE_ENDED,
											 .OffsetNumber = ADC_OFFSET_NONE,
											 .Offset = 0};

ADC_ChannelConfTypeDef brakeFL_back_buff = {.Channel = ADC_CHANNEL_10, // PA5
											.Rank = ADC_REGULAR_RANK_1,
											.SamplingTime =
												ADC_SAMPLETIME_2CYCLES_5,
											.SingleDiff = ADC_SINGLE_ENDED,
											.OffsetNumber = ADC_OFFSET_NONE,
											.Offset = 0};

ADC_ChannelConfTypeDef accelPotRed_buff = {.Channel = ADC_CHANNEL_11, // PA6
										   .Rank = ADC_REGULAR_RANK_1,
										   .SamplingTime =
											   ADC_SAMPLETIME_2CYCLES_5,
										   .SingleDiff = ADC_SINGLE_ENDED,
										   .OffsetNumber = ADC_OFFSET_NONE,
										   .Offset = 0};

ADC_ChannelConfTypeDef brakePotRed_buff = {.Channel = ADC_CHANNEL_15, // PB0
										   .Rank = ADC_REGULAR_RANK_1,
										   .SamplingTime =
											   ADC_SAMPLETIME_2CYCLES_5,
										   .SingleDiff = ADC_SINGLE_ENDED,
										   .OffsetNumber = ADC_OFFSET_NONE,
										   .Offset = 0};

PedalsStatus readADC(ADCInput adc) {
	if (pedals_adc_init() != ADC_OK)
		Error_Handler();
	uint32_t brakePot_buff_val = 0;
	uint32_t accelPot_buff_val = 0;
	switch (adc) {
	case ADC_INPUT_BRAKE_POT:
		if (adc_start_read(&brakePot_buff) != ADC_OK) {
			printf("ADC read failed\n\r");
			return ADC_READ_FAIL;
		}
		if (xQueueReceive(adc1_brakePot_RecvQ, &brakePot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {

			printf("Brake (%%): %u%% | Brake Pot: %lu\n\r",
				   adcPercentBrakeLUT[brakePot_buff_val], brakePot_buff_val);
		} else {
			printf("Failed to receive AccelPot ADC value from queue\n\r");
			return ADC_READ_FAIL;
		}
		break;
	case ADC_INPUT_ACCEL_POT:
		if (adc_start_read(&accelPot_buff) != ADC_OK) {
			printf("ADC read failed\n\r");
			return ADC_READ_FAIL;
		}
		if (xQueueReceive(adc1_accelPot_RecvQ, &accelPot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Accel (%%): %u%% | Accel Pot: %lu\n\r",
				   adcPercentAccelLUT[accelPot_buff_val], accelPot_buff_val);
		} else {
			printf("Failed to receive AccelPot ADC value from queue\n\r");
			return ADC_READ_FAIL;
		}
		break;
	case ADC_INPUT_BRAKE_FL_FRONT:
		// TODO
		break;
	case ADC_INPUT_BRAKE_FL_BACK:
		// TODO
		break;
	case ADC_INPUT_ACCEL_POT_RED:
		// TODO
		break;
	case ADC_INPUT_BRAKE_POT_RED:
		// TODO
		break;
	default:
		return ADC_READ_FAIL;
	}

	return ADC_READ_JOLLY;
}

void readAll_ADCs_task(void *argument) {
	if (pedals_adc_init() != ADC_OK)
		Error_Handler();
	while (1) {
		printf("\033[2J");

		if (adc_start_read(&brakePot_buff) != ADC_OK)
			printf("ADC read failed\n\r");
		if (adc_start_read(&accelPot_buff) != ADC_OK)
			printf("ADC read failed\n\r");

		uint32_t brakePot_buff_val = 0;
		uint32_t accelPot_buff_val = 0;
		uint32_t brakeFL_front_buff_val = 0;
		uint32_t brakeFL_back_buff_val = 0;
		uint32_t accelPotRed_buff_val = 0;
		uint32_t brakePotRed_buff_val = 0;

		/* --------------------------------------------------
			POTS Queue Receives
		  -------------------------------------------------- */
		if (xQueueReceive(adc1_brakePot_RecvQ, &brakePot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake Pot: %lu  |  Brake (%%)): %u%%\n\r",
				   brakePot_buff_val, adcPercentBrakeLUT[brakePot_buff_val]);
		} else {
			printf("Failed to receive BrakePot ADC value from queue\n\r");
		}

		if (xQueueReceive(adc1_accelPot_RecvQ, &accelPot_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Accel Pot: %lu  |  Accel (%%)): %u%%\n\r",
				   accelPot_buff_val, adcPercentAccelLUT[accelPot_buff_val]);
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
				brakePotRed_buff_val, adcPercentBrakeLUT[brakePotRed_buff_val]);
		} else {
			printf("Failed to receive BrakePot Redundant ADC value from "
				   "queue\n\r");
		}

		if (xQueueReceive(adc1_accelPotRed_RecvQ, &accelPotRed_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf(
				"Accel Pot Redundant: %lu  |  Accel Redundant (%%)): %u%%\n\r",
				accelPotRed_buff_val, adcPercentAccelLUT[accelPotRed_buff_val]);
		} else {
			printf("Failed to receive AccelPot Redundant ADC value from "
				   "queue\n\r");
		}

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL1
		   -------------------------------------------------- */
		if (xQueueReceive(adc1_brakeFL_front_RecvQ, &brakeFL_front_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake FL: %lu  |  Brake FL (%%)): %u%%\n\r",
				   brakeFL_front_buff_val,
				   adcPercentAccelLUT[brakeFL_front_buff_val]);
		} else {
			printf("Failed to receive BrakeFL ADC value from queue\n\r");
		}

		/* --------------------------------------------------
			CALIBRATE A NEW LUT FOR BRAKE_FL2
		  -------------------------------------------------- */
		if (xQueueReceive(adc1_brakeFL_back_RecvQ, &brakeFL_back_buff_val,
						  pdMS_TO_TICKS(10)) == pdPASS) {
			printf("Brake FL Redundant: %lu  |  Brake FL Redundant (%%)): "
				   "%u%%\n\r",
				   brakeFL_back_buff_val,
				   adcPercentAccelLUT[brakeFL_back_buff_val]);
		} else {
			printf(
				"Failed to receive BrakeFL Redundant ADC value from queue\n\r");
		}

		/* --------------------------------------------------
			Input Status LEDs toggle (processing data into array)
		  -------------------------------------------------- */
		if (adcPercentBrakeLUT[brakePot_buff_val] > 50)
			set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
		else
			set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
		if (adcPercentAccelLUT[accelPot_buff_val] > 50)
			set_LED(ACCEL_POT_LED, GPIO_PIN_SET);
		else
			set_LED(ACCEL_POT_LED, GPIO_PIN_RESET);
		if (adcPercentAccelLUT[brakeFL_front_buff_val] > 50)
			set_LED(BRAKE_FL_LED, GPIO_PIN_SET);
		else
			set_LED(BRAKE_FL_LED, GPIO_PIN_RESET);
	}
}

adc_status_t adc_start_read(ADC_ChannelConfTypeDef *adcPin) {
	// Determine which queue to use based on the ADC channel
	QueueHandle_t targetQueue = adc1_brakePot_RecvQ; // default

	if (adcPin == &accelPot_buff) {
		targetQueue = adc1_accelPot_RecvQ;
	} else if (adcPin == &brakeFL_front_buff) {
		targetQueue = adc1_brakeFL_front_RecvQ;
	} else if (adcPin == &brakeFL_back_buff) {
		targetQueue = adc1_brakeFL_back_RecvQ;
	} else if (adcPin == &accelPotRed_buff) {
		targetQueue = adc1_accelPotRed_RecvQ;
	} else if (adcPin == &brakePotRed_buff) {
		targetQueue = adc1_brakePotRed_RecvQ;
	}
	// brakePot_buff uses default adc1_brakePot_RecvQ

	return adc_read(hadc1, adcPin, targetQueue);
}

adc_status_t pedals_adc_init() {
	ADC_InitTypeDef adc_init_1 = {0};

	/* --------------------------------------------------
		ADC Init
	 -------------------------------------------------- */
	adc_init_1.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc_init_1.Resolution = ADC_RESOLUTION_12B;
	adc_init_1.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_init_1.ScanConvMode = ADC_SCAN_DISABLE;
	adc_init_1.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc_init_1.LowPowerAutoWait = DISABLE;
	adc_init_1.ContinuousConvMode = DISABLE;
	adc_init_1.NbrOfConversion = 1;
	adc_init_1.DiscontinuousConvMode = DISABLE;
	adc_init_1.ExternalTrigConv = ADC_SOFTWARE_START;
	adc_init_1.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc_init_1.DMAContinuousRequests = DISABLE;
	adc_init_1.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_init_1.OversamplingMode = DISABLE;

	/* --------------------------------------------------
		ADC Queue Init
	 -------------------------------------------------- */
	adc1_brakePot_RecvQ =
		xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE,
						   adc1_brakePot_queue, &adc1_brakePot_queueBuffer);
	if (adc1_brakePot_RecvQ == NULL) {
		printf("Failed to create BrakePot ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_accelPot_RecvQ =
		xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE,
						   adc1_accelPot_queue, &adc1_accelPot_queue_buffer);
	if (adc1_accelPot_RecvQ == NULL) {
		printf("Failed to create AccelPot ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakeFL_front_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakeFL_front_queue,
		&adc1_brakeFL_front_queueBuffer);
	if (adc1_brakeFL_front_RecvQ == NULL) {
		printf("Failed to create BrakeFL ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakeFL_back_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakeFL_back_queue,
		&adc1_brakeFL_back_queueBuffer);
	if (adc1_brakeFL_back_RecvQ == NULL) {
		printf("Failed to create BrakeFLRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_accelPotRed_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_accelPotRed_queue,
		&adc1_accelPotRed_queue_buffer);
	if (adc1_accelPotRed_RecvQ == NULL) {
		printf("Failed to create AccelPotRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakePotRed_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakePotRed_queue,
		&adc1_brakePotRed_queue_buffer);
	if (adc1_brakePotRed_RecvQ == NULL) {
		printf("Failed to create BrakePotRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	volatile adc_status_t s = adc_init(&adc_init_1, hadc1);
	s += 0;
	if (s != ADC_OK) {
		printf("Failed to initialize ADC1\n\r");
		return ADC_INIT_FAIL;
	}
	return ADC_OK;
}

/* --------------------------------------------------
	  ADC GPIO init
   -------------------------------------------------- */

void adc_GPIO_init() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if (hadc1->Instance == ADC1) {

		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
		PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
		PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
		PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
		PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
		PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
		PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
		PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
			printf("ravi is dum\n\r");
			Error_Handler();
		}

		/* ADC1 clock enable */
		__HAL_RCC_ADC_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
		PA2     ------> ADC1_IN7
		PA0     ------> ADC1_IN5
		PA4     ------> ADC1_IN9
		PA5     ------> ADC1_IN10
		PA6     ------> ADC1_IN11
		*/
		GPIO_InitStruct.Pin = BRAKE_POT.pin || ACCEL_POT.pin ||
							  BRAKE_FL_FRONT.pin || BRAKE_FL_BACK.pin ||
							  ACCEL_POT_RED.pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BRAKE_POT.port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = BRAKE_POT_RED.pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BRAKE_POT_RED.port, &GPIO_InitStruct);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(ADC1_IRQn);
	}
}

void Error_Handler(void) {
	__disable_irq();
	printf("Error Handler: ADC initialization failed\n\r");
	while (1) {
	}
}
