#include "stm32xx_hal.h"

#ifndef PINMODE_H
#define PINMODE_H

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} GPIO_Pin;

#endif