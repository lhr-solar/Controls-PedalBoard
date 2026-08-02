#pragma once

// -------- ADC --------
#define BRAKE_POT_PORT              GPIOA
#define BRAKE_POT_PIN               GPIO_PIN_2
#define ACCEL_POT_PORT              GPIOA
#define ACCEL_POT_PIN               GPIO_PIN_0
/* Brake pressure sensors → CAN Brake_Pressure_1 / Brake_Pressure_2 (0x650 / 0x651) */
#define BRAKE_FL_FRONT_PORT         GPIOA
#define BRAKE_FL_FRONT_PIN          GPIO_PIN_4 /* pressure 1, ADC1_IN9 */
#define BRAKE_FL_BACK_PORT          GPIOA
#define BRAKE_FL_BACK_PIN           GPIO_PIN_5 /* pressure 2, ADC1_IN10 */
#define ACCEL_POT_REDUNDANT_PORT    GPIOA
#define ACCEL_POT_REDUNDANT_PIN     GPIO_PIN_6
#define BRAKE_POT_REDUNDANT_PORT    GPIOB
#define BRAKE_POT_REDUNDANT_PIN     GPIO_PIN_0

// -------- Status LEDs --------
#define BRAKE_POT_LED_PORT          GPIOB
#define BRAKE_POT_LED_PIN           GPIO_PIN_12
#define ACCEL_POT_LED_PORT          GPIOB
#define ACCEL_POT_LED_PIN           GPIO_PIN_7
#define BRAKE_FL_LED_PORT           GPIOB
#define BRAKE_FL_LED_PIN            GPIO_PIN_6
#define PSOM_HB_PORT                GPIOA
#define PSOM_HB_PIN                 GPIO_PIN_7

// -------- UART PinDefs --------
#define USART_PORT                  GPIOA
#define USART_TX_PIN                GPIO_PIN_9
#define USART_RX_PIN                GPIO_PIN_10