#include "InitTask.h"
#include "PedalsTask.h"
#include "PressureTask.h"
#include "HeartbeatTask.h"
#include "PedalsCAN.h"
#include "PedalsSensors.h"
#include "StatusLEDs.h"
#include "Debugging.h"

StaticTask_t HeartbeatTaskTCB;
StackType_t  HeartbeatTaskStackArray[HEARTBEAT_TASK_STACK_SIZE];

StaticTask_t PedalsTaskTCB;
StackType_t  PedalsTaskStackArray[PEDALS_TASK_STACK_SIZE];

StaticTask_t BrakePressureTaskTCB;
StackType_t  BrakePressureTaskStackArray[BRAKEPRESSURE_TASK_STACK_SIZE];


void PedalsErrorHandler(void) {
    printf("Errored out!\n");
    while (1) {
        flashThem();
    }
}

void Task_Init(void *args __attribute__((unused))) {
    initPrintf();
    sensors_adc_GPIO_init();
    led_init();

    if (sensors_adc_init() != ADC_OK) {
        PedalsErrorHandler();
    }

    if (pedals_can_init() != PEDALS_OK) {
        PedalsErrorHandler();
    }

    xTaskCreateStatic(Task_Pedals,
                      "Pedals",
                      PEDALS_TASK_STACK_SIZE,
                      NULL,
                      PEDALS_TASK_PRIORITY,
                      PedalsTaskStackArray,
                      &PedalsTaskTCB);

    xTaskCreateStatic(Task_Heartbeat,
                      "Heartbeat",
                      HEARTBEAT_TASK_STACK_SIZE,
                      NULL,
                      HEARTBEAT_TASK_PRIORITY,
                      HeartbeatTaskStackArray,
                      &HeartbeatTaskTCB);

    vTaskDelete(NULL);
}
