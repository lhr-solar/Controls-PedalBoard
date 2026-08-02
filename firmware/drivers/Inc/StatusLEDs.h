#pragma once

#include "stm32xx_hal.h"
#include "pinDefs.h"

#define LED_ON  GPIO_PIN_SET
#define LED_OFF GPIO_PIN_RESET

void led_init(void);
void led_set(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
void led_toggle(GPIO_TypeDef *port, uint16_t pin);
void led_heartbeat(void);
void flashThem(void);
