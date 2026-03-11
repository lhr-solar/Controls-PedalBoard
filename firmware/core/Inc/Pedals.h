#pragma once

#include "ADC.h"
#include "Debugging.h"
#include "stm32xx_hal.h"

#define INIT_TASK_PRIORITY (5)
#define INIT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
extern StaticTask_t INIT_TASK_TCB;
extern StackType_t INIT_TASK_Stack_Array[INIT_TASK_STACK_SIZE];

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} GPIO_Pin;


/* --------------------------------------------------
	GPIO/ADC PinDefs
-------------------------------------------------- */


/* -------- ADC --------*/

const GPIO_Pin BRAKE_POT = {GPIOA, GPIO_PIN_2};
const GPIO_Pin ACCEL_POT = {GPIOA, GPIO_PIN_0};
const GPIO_Pin BRAKE_FL_FRONT = {GPIOA, GPIO_PIN_4};
const GPIO_Pin BRAKE_FL_BACK = {GPIOA, GPIO_PIN_5};
const GPIO_Pin ACCEL_POT_RED = {GPIOA, GPIO_PIN_6};
const GPIO_Pin BRAKE_POT_RED = {GPIOB, GPIO_PIN_0};


/* -------- Status LEDS --------*/

//Port B - out
const GPIO_Pin BRAKE_POT_LED = {GPIOB , GPIO_PIN_12};
const GPIO_Pin ACCEL_POT_LED = {GPIOB , GPIO_PIN_7};
const GPIO_Pin BRAKE_FL_LED = {GPIOB , GPIO_PIN_6};

//port A - out
const GPIO_Pin PSOM_HB = {GPIOA, GPIO_PIN_7};




/* --------------------------------------------------
	Pedals Status
-------------------------------------------------- */

typedef enum PedalsStatus {
	// Faults
	BRAKE_POT_OK = 1,
	BRAKE_POT_FAIL = 0,
	ACCEL_POT_OK = 1,
	ACCEL_POT_FAIL = 0,
	FL_BRAKE_POT_OK = 1,
	FL_BRAKE_POT_FAIL = 0,

	// General Status
	PEDALS_CAN_START_FAIL,
	PEDALS_CAN_INIT_FAIL,
	PEDALS_CAN_SEND_FAIL,
	PEDALS_CAN_STOP_FAIL,
	PEDALS_OK

} PedalsStatus;

/* -------------------------------------------------- */

void initAll_Task(void *argument);

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) !=
		HAL_OK) {
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
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
								  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}
