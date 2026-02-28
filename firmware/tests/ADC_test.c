#include "StatusLEDs.h"
#include "Debugging.h"
#include "stm32l4xx_hal.h"
#include "ADC_init.h"


#define PRINT_TASK_PRIORITY                  (tskIDLE_PRIORITY+1)
#define PRINT_TASK_STACK_SIZE                configMINIMAL_STACK_SIZE
StaticTask_t PRINT_TASK_TCB;
StackType_t PRINT_TASK_Stack_Array[PRINT_TASK_STACK_SIZE];

static uint8_t adc1_static_storage[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1QueueBuffer;
static QueueHandle_t adc1RecvQ;


void ADC_Task(void *argument);

int main() {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    initPrintf();
    if(dash_adc_init() != ADC_OK) Error_Handler();

    xTaskCreateStatic(
        ADC_Task,
        "ADC testing",
        PRINT_TASK_STACK_SIZE,
        NULL,
        PRINT_TASK_PRIORITY,
        PRINT_TASK_Stack_Array,
        &PRINT_TASK_TCB

    );
    vTaskStartScheduler();    

    return 0;
}


void ADC_Task(void *argument) {

    while(1) {
        printf("\n\r--> Task Started\n\r");
        
        printf("bruh1\n\r");
        if(readADC_All() != ADC_OK) {
            printf("ADC read failed\n\r");
            continue;
        }
        
        printf("bruh2\n\r");

        uint16_t adcVal = 0;
        if(xQueueReceive(adc1RecvQ, &adcVal, portMAX_DELAY) == pdTRUE) {
            printf("ADC value: %d\n\r", adcVal);
        } else printf("Failed to receive ADC value from queue\n\r");


        set_LED(PSOM_HB, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(500));
        set_LED(PSOM_HB, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


adc_status_t readADC_All() {
    return adc_read(ADC_CHANNEL_7, ADC_SAMPLETIME_2CYCLE_5, hadc1, adc1RecvQ);
}

adc_status_t dash_adc_init() {
    printf("Creating ADC Queue...\n\r");
    adc1RecvQ = xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_static_storage, &adc1QueueBuffer);

    if(adc1RecvQ == NULL) return ADC_INIT_FAIL;

    printf("Queue created successfully\n\r");
    return adc1_init();
}

adc_status_t adc1_init() {
    printf("Initializing ADC1...\n\r");
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the peripherals clocks*/
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) Error_Handler();

    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    printf("Peripheral clocks initialized successfully\n\r");

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    printf("GPIO-ADC initialized successfully\n\r");

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1->Instance = ADC1;
    hadc1->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
    hadc1->Init.Resolution = ADC_RESOLUTION_12B;
    hadc1->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1->Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1->Init.LowPowerAutoWait = DISABLE;
    hadc1->Init.ContinuousConvMode = DISABLE;
    hadc1->Init.NbrOfConversion = 1;
    hadc1->Init.DiscontinuousConvMode = DISABLE;
    hadc1->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1->Init.DMAContinuousRequests = DISABLE;
    hadc1->Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1->Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(hadc1) != HAL_OK) Error_Handler();

    printf("ADC1 initialized successfully\n\r");

    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK) Error_Handler();

    printf("ADC1 channel configured successfully\n\r");

    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

    printf("ADC1 interrupt configured successfully\n\r");
    printf("ADC initialization complete\n\r");
    return ADC_OK;

}

void Error_Handler(void) {
    __disable_irq();
    printf("Error Handler: ADC initialization failed\n\r");
    while (1) {}
}



