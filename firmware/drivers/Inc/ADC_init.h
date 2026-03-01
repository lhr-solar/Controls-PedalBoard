#include "ADC.h"

#define ADC1_QUEUE_LENGTH   10
#define ADC_ITEM_SIZE       sizeof( uint32_t )

extern ADC_HandleTypeDef* hadc1;


ADC_InitTypeDef adc_init_1;
void Error_Handler();
adc_status_t adc1_init();
adc_status_t dash_adc_init();
adc_status_t adc_start_read();