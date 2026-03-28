#include "StatusLEDs.h"

/* --------------------------------------------------
      Status LED inits
   -------------------------------------------------- */

void Status_LEDs_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PSOM_HB_PIN, GPIO_PIN_RESET); //|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BRAKE_POT_LED_PIN|BRAKE_FL_LED_PIN|ACCEL_POT_LED_PIN, GPIO_PIN_RESET); //GPIO_PIN_11||GPIO_PIN_14|GPIO_PIN_15

  /*Configure GPIO pins : PA7 PA8 PA12 PA15 */
  GPIO_InitStruct.Pin = PSOM_HB_PIN;//|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB12 PB14 PB15
                           PB6 PB7 */
  GPIO_InitStruct.Pin = BRAKE_POT_LED_PIN|BRAKE_FL_LED_PIN|ACCEL_POT_LED_PIN; //GPIO_PIN_11||GPIO_PIN_14|GPIO_PIN_15
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void set_LED(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state) {
  HAL_GPIO_WritePin(port, pin, state);
}

void toggle_LED(GPIO_TypeDef *port, uint16_t pin) {
  HAL_GPIO_TogglePin(port, pin);
}

void flashThem(uint16_t delay) {
  toggle_LED(BRAKE_POT_LED_PORT, BRAKE_POT_LED_PIN);
  toggle_LED(ACCEL_POT_LED_PORT, ACCEL_POT_LED_PIN);
  toggle_LED(BRAKE_FL_LED_PORT,  BRAKE_FL_LED_PIN);
  vTaskDelay(pdMS_TO_TICKS(delay));
}
