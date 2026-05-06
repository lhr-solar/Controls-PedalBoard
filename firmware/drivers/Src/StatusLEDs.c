#include "StatusLEDs.h"

void led_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// GPIO Ports Clock Enable
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
    
	// Configure GPIO pin Output Level
	HAL_GPIO_WritePin(PSOM_HB_PORT, PSOM_HB_PIN, LED_OFF);

	// Configure GPIO pin Output Level
	HAL_GPIO_WritePin(BRAKE_POT_LED_PORT,
					  BRAKE_POT_LED_PIN | BRAKE_FL_LED_PIN | ACCEL_POT_LED_PIN,
					  LED_OFF);

	// Configure GPIO pins : PSOM_HB
	GPIO_InitStruct.Pin = PSOM_HB_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PSOM_HB_PORT, &GPIO_InitStruct);

	// Configure GPIO pins: BrakePot LED, AccelPot LED, BrakeFL LED
	GPIO_InitStruct.Pin =
		BRAKE_POT_LED_PIN | BRAKE_FL_LED_PIN | ACCEL_POT_LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BRAKE_POT_LED_PORT, &GPIO_InitStruct);
}

void led_set(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state) {
	HAL_GPIO_WritePin(port, pin, state);
}

void led_toggle(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_TogglePin(port, pin);
}

void flashThem() {
	led_toggle(BRAKE_POT_LED_PORT, BRAKE_POT_LED_PIN);
	led_toggle(ACCEL_POT_LED_PORT, ACCEL_POT_LED_PIN);
	led_toggle(BRAKE_FL_LED_PORT, BRAKE_FL_LED_PIN);
}
