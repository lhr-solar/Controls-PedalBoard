#include "Pots.h"
#include "StatusLEDs.h"
#include "Debugging.h"

uint32_t adc_buffer[6];

extern ADC_HandleTypeDef hadc1;

const GPIO_Pin BRAKE_POT = {GPIOA, GPIO_PIN_2};
const GPIO_Pin ACCEL_POT = {GPIOA, GPIO_PIN_0};
const GPIO_Pin BRAKE_FL = {GPIOA, GPIO_PIN_4};
const GPIO_Pin BRAKE_FL_RED = {GPIOA, GPIO_PIN_5};
const GPIO_Pin ACCEL_POT_RED = {GPIOA, GPIO_PIN_6};
const GPIO_Pin BRAKE_POT_RED = {GPIOB, GPIO_PIN_0};

// void ADC1_Init(void) {
//     __HAL_RCC_ADC_CLK_ENABLE();
//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     __HAL_RCC_GPIOC_CLK_ENABLE();

//     GPIO_InitTypeDef g = {0};
//     g.Mode = GPIO_MODE_ANALOG;
//     g.Pull = GPIO_NOPULL;
//     g.Pin = BRAKE_POT.pin | ACCEL_POT.pin | BRAKE_FL.pin | BRAKE_FL_RED.pin | ACCEL_POT_RED.pin;
//     HAL_GPIO_Init(GPIOA, &g);
//     g.Pin = BRAKE_POT_RED.pin;
//     HAL_GPIO_Init(GPIOB, &g);

//     ADC_HandleTypeDef hadc1;
//     ADC_ChannelConfTypeDef sConfig = {0};
    
//     hadc1.Instance = ADC1;
//     hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
//     hadc1.Init.Resolution = ADC_RESOLUTION_12B;
//     hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//     hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
//     hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//     hadc1.Init.LowPowerAutoWait = DISABLE;
//     hadc1.Init.ContinuousConvMode = ENABLE;
//     hadc1.Init.NbrOfConversion = 6;
//     hadc1.Init.DiscontinuousConvMode = DISABLE;
//     hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//     hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//     hadc1.Init.DMAContinuousRequests = ENABLE;
//     hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
//     hadc1.Init.OversamplingMode = DISABLE;
//     HAL_ADC_Init(&hadc1);
//     //   if (HAL_ADC_Init(&hadc1) != HAL_OK) {
//     //     Error_Handler();
//     //   }
//     flashThem(500);
//     // Rank 1 → IN5 (PA0)
//     sConfig.Channel = ADC_CHANNEL_5;
//     sConfig.Rank = ADC_REGULAR_RANK_1;
//     sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     // Rank 2 → IN7 (PA6)
//     sConfig.Channel = ADC_CHANNEL_7;
//     sConfig.Rank = ADC_REGULAR_RANK_2;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     // Rank 3 → IN9 (PA4)
//     sConfig.Channel = ADC_CHANNEL_9;
//     sConfig.Rank = ADC_REGULAR_RANK_3;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     // Rank 4 → IN10 (PC0)
//     sConfig.Channel = ADC_CHANNEL_10;
//     sConfig.Rank = ADC_REGULAR_RANK_4;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     // Rank 5 → IN11 (PC1)
//     sConfig.Channel = ADC_CHANNEL_11;
//     sConfig.Rank = ADC_REGULAR_RANK_5;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     // Rank 6 → IN15 (PC5)
//     sConfig.Channel = ADC_CHANNEL_15;
//     sConfig.Rank = ADC_REGULAR_RANK_6;
//     HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//     DMA1_Init();
//     HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 6);
//     flashThem(500);

// }

// void DMA1_Init(void) {
//     /* DMA controller clock enable */
//     __HAL_RCC_DMA1_CLK_ENABLE();

//     /* DMA interrupt init */
//     /* DMA1_Channel1_IRQn interrupt configuration */
//     HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
//     HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

// }


// uint32_t getADCInput(ADCInput adc) {
//     uint32_t out;
//     flashThem(500);
//     out = adc_buffer[adc];

//     return out;
// }