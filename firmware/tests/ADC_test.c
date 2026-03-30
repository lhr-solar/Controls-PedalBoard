#include "FreeRTOS.h"
#include "Pedals.h"
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
	
	if (pedals_adc_init_test() != ADC_OK)
		Error_Handler_test();

	while (1) {
		for (uint16_t i = 0; i < 100; i++) {
			printf("\033[2J");
		}

		if (adc_start_read_test(&brakePot_buff) != ADC_OK)
			printf("ADC read failed\n\r");

		uint16_t brakePot = 0;
		if (xQueueReceive(adc1RecvQ, &brakePot, portMAX_DELAY) == pdTRUE) {
			printf("ADC value: %d\n\r", brakePot);
		} else
			printf("Failed to receive ADC value from queue\n\r");

		set_LED(PSOM_HB, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(500));
		set_LED(PSOM_HB, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

adc_status_t adc_start_read_test(ADC_ChannelConfTypeDef *adcPin) {
	return adc_read(hadc1, adcPin, adc1RecvQ);
}

adc_status_t pedals_adc_init_test() {
	ADC_InitTypeDef adc_init_1 = {0};

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

	adc1RecvQ = xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1Queue,
								   &adc1QueueBuffer);
	if (adc1RecvQ == NULL) {
		printf("Failed to create ADC queue\n\r");
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

void adc_GPIO_init_test() {
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
			printf("ravi");
			Error_Handler();
		}

		/* ADC1 clock enable */
		__HAL_RCC_ADC_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
		PA2     ------> ADC1_IN7
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(ADC1_IRQn);

		printf("GPIO-ADC initialized successfully\n\r");
	}
}

void Error_Handler_test(void) {
	printf("Error Handler: ADC initialization failed\n\r");
	while (1) {
	}
}