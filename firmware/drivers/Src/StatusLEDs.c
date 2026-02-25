#include "StatusLEDs.h"

const GPIO_Pin BRAKE_POT_LED = {GPIOB , GPIO_PIN_12};
const GPIO_Pin ACCEL_POT_LED = {GPIOB , GPIO_PIN_7};
const GPIO_Pin BRAKE_FL_LED = {GPIOB , GPIO_PIN_6};

const GPIO_Pin PSOM_HB = {GPIOA, GPIO_PIN_7};

void GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA7 PA8 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB12 PB14 PB15
                           PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void set_LED(GPIO_Pin LED, GPIO_PinState state) {
  HAL_GPIO_WritePin(LED.port, LED.pin, state);
}

void toggle_LED(GPIO_Pin LED) {
  HAL_GPIO_TogglePin(LED.port, LED.pin);
}
void flashThem(uint16_t delay) {
  set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
  set_LED(ACCEL_POT_LED, GPIO_PIN_SET);
  set_LED(BRAKE_FL_LED, GPIO_PIN_SET);
  HAL_Delay(delay);
  set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
  set_LED(ACCEL_POT_LED, GPIO_PIN_RESET);
  set_LED(BRAKE_FL_LED, GPIO_PIN_RESET);
  HAL_Delay(delay);
}

