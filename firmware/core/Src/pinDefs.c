// Pedals.c
#include "Pedals.h"

/* -------- ADC --------*/
const GPIO_Pin BRAKE_POT       = {GPIOA, GPIO_PIN_2};
const GPIO_Pin ACCEL_POT       = {GPIOA, GPIO_PIN_0};
const GPIO_Pin BRAKE_FL_FRONT  = {GPIOA, GPIO_PIN_4};
const GPIO_Pin BRAKE_FL_BACK   = {GPIOA, GPIO_PIN_5};
const GPIO_Pin ACCEL_POT_RED   = {GPIOA, GPIO_PIN_6};
const GPIO_Pin BRAKE_POT_RED   = {GPIOB, GPIO_PIN_0};

/* -------- Status LEDs --------*/
const GPIO_Pin BRAKE_POT_LED   = {GPIOB, GPIO_PIN_12};
const GPIO_Pin ACCEL_POT_LED   = {GPIOB, GPIO_PIN_7};
const GPIO_Pin BRAKE_FL_LED    = {GPIOB, GPIO_PIN_6};
const GPIO_Pin PSOM_HB         = {GPIOA, GPIO_PIN_7};