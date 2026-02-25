#include "pinMode.h"

extern uint32_t adc_buffer[];

extern const GPIO_Pin BRAKE_POT;
extern const GPIO_Pin ACCEL_POT;
extern const GPIO_Pin BRAKE_FL;
extern const GPIO_Pin BRAKE_FL_RED;
extern const GPIO_Pin ACCEL_POT_RED;
extern const GPIO_Pin BRAKE_POT_RED;

typedef enum {
    ADC_INPUT_ACCEL_POT        = 0,   // ACCEL_POT_READ
    ADC_INPUT_BRAKE_POT        = 1,   // BRAKE_POT_READ
    ADC_INPUT_BRAKE_FL         = 2,   // BRAKE_FL_READ
    ADC_INPUT_BRAKE_FL_RED     = 3,   // BRAKE_FL_RED_READ
    ADC_INPUT_ACCEL_POT_RED    = 4,   // ACCEL_POT_RED_READ
    ADC_INPUT_BRAKE_POT_RED    = 5,   // BRAKE_POT_RED_READ

    ADC_INPUT_COUNT
} ADCInput;

// uint32_t getADCInput(ADCInput adc);
// void ADC1_Init(void);
// void DMA1_Init();