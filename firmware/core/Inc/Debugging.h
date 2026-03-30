#pragma once

#include "stm32xx_hal.h"
#include <stdio.h>
#include "UART.h"
#include "printf.h"
#include "pinDefs.h"

/**
 * @brief Initialize PrintF 
 *
 * This function initializes the printf() function, mostly used for debugging\
 * 
 * @attention IMPORTANT - only call initPrintf() AND use printf() in a RTOS task
 * 
 */

void initPrintf();
