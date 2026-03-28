#pragma once

#include "Pedals_Sensors.h"

/**
 * @brief Read All 6 ADC Channels
 *
 * Outputs the raw & percent (using LUT) ADC values for all 6 ADCs
 *
 * 
 */
void readAll_ADCs_task(void *argument);