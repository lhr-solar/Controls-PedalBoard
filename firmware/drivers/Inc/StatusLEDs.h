#pragma once

#include "Pedals.h"

#define LED_ON GPIO_PIN_SET
#define LED_OFF GPIO_PIN_RESET

/**
 * @brief Init GPIO for all firmware LEDs
 */
void Status_LEDs_Init(void);

/**
 * @brief Set state of a specific LED
 * 
 * @attention - the user can use ON and OFF macros above
 *
 * @param	port		choose a specific LED port
 * @param	pin         choose a specific LED pin
 * @param   state 		GPIO_PIN_SET (0) = LED ON, 
 *                      GPIO_PIN_RESET (1) = LED OFF
 * 
 * 
 */
void set_LED(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);

/**
 * @brief Toggles the state of an LED
 *
 * @param	port		choose a specific LED port
 * @param	pin         choose a specific LED pin
 * 
 * 
 */
void toggle_LED(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief Toggles PSOM Heartbeat LED
 * 
 */
void HeartBeat();


/**
 * @brief Flashes the user!!!!
 *
 * Inspired by Phoenix from Valorant. Flashes the user by toggling
 * all LEDs at a rate specified by the delay
 *
 * 
 */
void flashThem();
