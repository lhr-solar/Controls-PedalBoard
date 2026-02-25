#include "StatusLEDs.h"
#include "Debugging.h"
#include "stm32l4xx_hal.h"


#define PRINT_TASK_PRIORITY                  (tskIDLE_PRIORITY+1)
#define PRINT_TASK_STACK_SIZE                configMINIMAL_STACK_SIZE
StaticTask_t PRINT_TASK_TCB;
StackType_t PRINT_TASK_Stack_Array[PRINT_TASK_STACK_SIZE];

extern ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

uint32_t adcBuffer[1];

void Error_Handler(void);
void MX_GPIO_ADC_Init(void);
void MX_ADC1_Init(void);
void MX_DMA_Init(void);
void Debug_ADC_Task(void *argument);
void initADC();
void DMA1_Channel1_IRQHandler(void);


int main() {
    HAL_Init();
    GPIO_Init();
    initADC();
    initPrintf();

    xTaskCreateStatic(
        Debug_ADC_Task,
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


void Debug_ADC_Task(void *argument) {
    
    /* ---------- START ---------- */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 1) != HAL_OK)
        Error_Handler();
    
    flashThem(500);

    while(1) {
        printf("ADC: %ld\n", adcBuffer[0]);

        set_LED(PSOM_HB, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(500));
        set_LED(PSOM_HB, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Error_Handler(void) {
    while(1) flashThem(100);
}

void MX_GPIO_ADC_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();   // or correct port

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2;        // <-- ADC1_IN7 pin
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void MX_ADC1_Init(void) {
    __HAL_RCC_ADC_CLK_ENABLE();

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
        Error_Handler();

    /* ---------- CALIBRATION ---------- */

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
        Error_Handler();
    /* ---------- CHANNEL ---------- */

    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
        Error_Handler();

    /* ---------- DMA ---------- */

    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Request = DMA_REQUEST_0;  // ✔ correct for L431
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    flashThem(500);
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        Error_Handler();
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    flashThem(500);
}

void MX_DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

void initADC() {
    MX_GPIO_ADC_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
}

void DMA1_Channel1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_adc1);
}





