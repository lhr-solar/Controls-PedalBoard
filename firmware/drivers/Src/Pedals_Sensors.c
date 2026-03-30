#include "Pedals_Sensors.h"
#include "Pedals.h"
#include "StatusLEDs.h"

/* --------------------------------------------------
	  ADC Queue Declarations and GPIO Pin Configurations
   -------------------------------------------------- */

static uint8_t adc1_brakePot_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_brakePot_queueBuffer;
static QueueHandle_t adc1_brakePot_RecvQ;

static uint8_t adc1_accelPot_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_accelPot_queue_buffer;
static QueueHandle_t adc1_accelPot_RecvQ;

static uint8_t adc1_brakeFL_front_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_brakeFL_front_queueBuffer;
static QueueHandle_t adc1_brakeFL_front_RecvQ;

static uint8_t adc1_brakeFL_back_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_brakeFL_back_queueBuffer;
static QueueHandle_t adc1_brakeFL_back_RecvQ;

static uint8_t adc1_accelPotRed_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_accelPotRed_queue_buffer;
static QueueHandle_t adc1_accelPotRed_RecvQ;

static uint8_t adc1_brakePotRed_queue[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1_brakePotRed_queue_buffer;
static QueueHandle_t adc1_brakePotRed_RecvQ;

adc_status_t sensors_adc_start_read(Pedals_ADC_Channel channel) {
	QueueHandle_t targetQueue;

	switch (channel) {
	case BRAKE_POT_BUFF_CHANNEL:
		targetQueue = adc1_brakePot_RecvQ;
		break;
	case ACCEL_POT_BUFF_CHANNEL:
		targetQueue = adc1_accelPot_RecvQ;
		break;
	case BRAKE_FL_FRONT_BUFF_CHANNEL:
		targetQueue = adc1_brakeFL_front_RecvQ;
		break;
	case BRAKE_FL_BACK_BUFF_CHANNEL:
		targetQueue = adc1_brakeFL_back_RecvQ;
		break;
	case ACCEL_POT_RED_BUFF_CHANNEL:
		targetQueue = adc1_accelPotRed_RecvQ;
		break;
	case BRAKE_POT_RED_BUFF_CHANNEL:
		targetQueue = adc1_brakePotRed_RecvQ;
		break;
	default:
		return ADC_CHANNEL_CONFIG_FAIL;
	}

	if (targetQueue == NULL) return ADC_INIT_FAIL;

	ADC_ChannelConfTypeDef cfg = {.Channel = channel,
								  .SamplingTime = PEDALS_ADC_SAMPLING_TIME,
								  .SingleDiff = ADC_SINGLE_ENDED,
								  .OffsetNumber = ADC_OFFSET_NONE,
								  .Offset = 0};

	return adc_read(hadc1, &cfg, targetQueue);
}

adc_status_t sensors_adc_init() {
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
		if (ENABLE_DEBUG) printf("Failed to create BrakePot ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_accelPot_RecvQ =
		xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE,
						   adc1_accelPot_queue, &adc1_accelPot_queue_buffer);
	if (adc1_accelPot_RecvQ == NULL) {
		if (ENABLE_DEBUG) printf("Failed to create AccelPot ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakeFL_front_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakeFL_front_queue,
		&adc1_brakeFL_front_queueBuffer);
	if (adc1_brakeFL_front_RecvQ == NULL) {
		if (ENABLE_DEBUG) printf("Failed to create BrakeFL ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakeFL_back_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakeFL_back_queue,
		&adc1_brakeFL_back_queueBuffer);
	if (adc1_brakeFL_back_RecvQ == NULL) {
		if (ENABLE_DEBUG) printf("Failed to create BrakeFLRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_accelPotRed_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_accelPotRed_queue,
		&adc1_accelPotRed_queue_buffer);
	if (adc1_accelPotRed_RecvQ == NULL) {
		if (ENABLE_DEBUG) printf("Failed to create AccelPotRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	adc1_brakePotRed_RecvQ = xQueueCreateStatic(
		ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_brakePotRed_queue,
		&adc1_brakePotRed_queue_buffer);
	if (adc1_brakePotRed_RecvQ == NULL) {
		if (ENABLE_DEBUG) printf("Failed to create BrakePotRed ADC queue\n\r");
		return ADC_INIT_FAIL;
	}

	volatile adc_status_t s = adc_init(&adc_init_1, hadc1);
	if (s != ADC_OK) {
		if (ENABLE_DEBUG) printf("Failed to initialize ADC1\n\r");
		return ADC_INIT_FAIL;
	}
	return ADC_OK;
}

/* --------------------------------------------------
	  ADC GPIO init
   -------------------------------------------------- */

void sensors_adc_GPIO_init() {
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
			if (ENABLE_DEBUG) printf("ravi is dum\n\r");
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
		GPIO_InitStruct.Pin = BRAKE_POT_PIN | ACCEL_POT_PIN |
							  BRAKE_FL_FRONT_PIN | BRAKE_FL_BACK_PIN |
							  ACCEL_POT_RED_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BRAKE_POT_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = BRAKE_POT_RED_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BRAKE_POT_RED_PORT, &GPIO_InitStruct);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_IRQn, PEDALS_ADC1_PRIO, 0);
		HAL_NVIC_EnableIRQ(ADC1_IRQn);
	}
}

static QueueHandle_t sensors_adc_get_queue(Sensors_ADC_Input_t adc_input, const char **name,
								   Pedals_Status_t *success, Pedals_Status_t *fail) {
	switch (adc_input) {
	case ADC_INPUT_BRAKE_POT:
		*name = "Brake Pot";
		*success = BRAKE_POT_OK;
		*fail = BRAKE_POT_FAIL;
		return adc1_brakePot_RecvQ;
	case ADC_INPUT_ACCEL_POT:
		*name = "Accel Pot";
		*success = ACCEL_POT_OK;
		*fail = ACCEL_POT_FAIL;
		return adc1_accelPot_RecvQ;
	case ADC_INPUT_BRAKE_FL_FRONT:
		*name = "Brake FL";
		*success = FL_BRAKE_POT_FRONT_OK;
		*fail = FL_BRAKE_POT_FRONT_FAIL;
		return adc1_brakeFL_front_RecvQ;
	case ADC_INPUT_BRAKE_FL_BACK:
		*name = "Brake FL Redundant";
		*success = FL_BRAKE_POT_BACK_OK;
		*fail = FL_BRAKE_POT_BACK_FAIL;
		return adc1_brakeFL_back_RecvQ;
	case ADC_INPUT_ACCEL_POT_RED:
		*name = "Accel Pot Redundant";
		*success = ACCEL_POT_RED_OK;
		*fail = ACCEL_POT_RED_FAIL;
		return adc1_accelPotRed_RecvQ;
	case ADC_INPUT_BRAKE_POT_RED:
		*name = "Brake Pot Redundant";
		*success = BRAKE_POT_RED_OK;
		*fail = BRAKE_POT_RED_FAIL;
		return adc1_brakePotRed_RecvQ;
	default:
		*name = "Unknown";
		*success = PEDALS_OK;
		*fail = PEDALS_OK;
		return NULL;
	}
}

Pedals_Status_t sensors_adc_receive(Sensors_ADC_Input_t adc_input, uint32_t *val) {
	const char *name;
	Pedals_Status_t success, fail;
	QueueHandle_t tempRecvQ =
		sensors_adc_get_queue(adc_input, &name, &success, &fail);
	if (tempRecvQ == NULL) return fail;

	if (xQueueReceive(tempRecvQ, val, pdMS_TO_TICKS(PEDALS_ADC_SAMPLING_MS)) == pdPASS) {
		return success;
	} else {
		if(ENABLE_DEBUG) printf("Failed to receive %s ADC value from queue\n\r", name);
		return fail;
	}
}

void Error_Handler(void) {
	if (ENABLE_DEBUG) printf("Error Handler: ADC initialization failed\n\r");
	while (1) {
		flashThem();
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}