#include "FreeRTOS.h"
#include "StatusLEDs.h"
#include "Debugging.h"
#include "stm32xx_hal.h"
#include "ADC_init.h"
#include "queue.h"

#define PRINT_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define PRINT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t PRINT_TASK_TCB;
StackType_t PRINT_TASK_Stack_Array[PRINT_TASK_STACK_SIZE];

static uint8_t adc1_static_storage[ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adc1QueueBuffer;
static QueueHandle_t adc1RecvQ;

ADC_ChannelConfTypeDef brakePot_buff = {
    .Channel = ADC_CHANNEL_7, // PA2
    .Rank = ADC_REGULAR_RANK_1,
    .SamplingTime = ADC_SAMPLETIME_2CYCLES_5,
    .SingleDiff = ADC_SINGLE_ENDED,
    .OffsetNumber = ADC_OFFSET_NONE,
    .Offset = 0};

void ADC_Task(void *argument);
void adc_GPIO_init();
adc_status_t adc_start_read();
adc_status_t dash_adc_init();

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();
    initPrintf();
    adc_GPIO_init();
    GPIO_Init();

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

void ADC_Task(void *argument)
{
    if (dash_adc_init() != ADC_OK)
        Error_Handler();

    while (1)
    {
        for(uint16_t i = 0; i < 100; i++) {
            printf("  \n\r");
        }

        if (adc_start_read() != ADC_OK)
        {
            printf("ADC read failed\n\r");
        }

        uint16_t adcVal = 0;
        if (xQueueReceive(adc1RecvQ, &adcVal, portMAX_DELAY) == pdTRUE)
        {
            printf("ADC value: %d\n\r", adcVal);
        }
        else
            printf("Failed to receive ADC value from queue\n\r");

        set_LED(PSOM_HB, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(500));
        set_LED(PSOM_HB, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

adc_status_t adc_start_read()
{
    return adc_read(hadc1, &brakePot_buff, adc1RecvQ);
}

adc_status_t dash_adc_init()
{
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

    adc1RecvQ = xQueueCreateStatic(ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE, adc1_static_storage, &adc1QueueBuffer);
    if (adc1RecvQ == NULL)
    {
        printf("Failed to create ADC queue\n\r");
        return ADC_INIT_FAIL;
    }

    volatile adc_status_t s = adc_init(&adc_init_1, hadc1);
    s += 0;
    if (s != ADC_OK)
    {
        printf("Failed to initialize ADC1\n\r");
        return ADC_INIT_FAIL;
    }
    return ADC_OK;
}

void adc_GPIO_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if (hadc1->Instance == ADC1)
    {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
        PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
        PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
        PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
        PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
        PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
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
        HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);

        printf("GPIO-ADC initialized successfully\n\r");
    }
}

void Error_Handler(void)
{
    __disable_irq();
    printf("Error Handler: ADC initialization failed\n\r");
    while (1)
    {
    }
}