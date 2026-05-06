#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32xx_hal.h"

// Thread Priorities
#define INIT_TASK_PRIORITY           (tskIDLE_PRIORITY + 5)
#define HEARTBEAT_TASK_PRIORITY      (tskIDLE_PRIORITY + 4)
#define PEDALS_TASK_PRIORITY         (tskIDLE_PRIORITY + 3)
#define BRAKEPRESSURE_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)

// Task Stack Sizes
#define INIT_TASK_STACK_SIZE           (configMINIMAL_STACK_SIZE * 4u)
#define HEARTBEAT_TASK_STACK_SIZE      (configMINIMAL_STACK_SIZE)
#define PEDALS_TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE * 4u)
#define BRAKEPRESSURE_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE * 4u)

// Init Task Prototype
void Task_Init(void *args __attribute__((unused)));

// General Error Handler
void PedalsErrorHandler(void);