#ifndef DEBUGGING_H
#define DEBUGGING_H

#include "stm32xx_hal.h"
#include <stdio.h>
#include "UART.h"
#include "printf.h"

#define USART_PORT GPIOA
#define USART_TX_PIN GPIO_PIN_9
#define USART_RX_PIN GPIO_PIN_10

extern UART_HandleTypeDef* husart1;
void initPrintf();

#endif