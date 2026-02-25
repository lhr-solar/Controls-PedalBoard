#include "FreeRTOS.h"
#include "Pots.h"
#include "StatusLEDs.h"
#include "Debugging.h"


#define INIT_TASK_PRIORITY                  (tskIDLE_PRIORITY+1)
#define INIT_TASK_STACK_SIZE                configMINIMAL_STACK_SIZE
StaticTask_t INIT_TASK_TCB;
StackType_t INIT_TASK_Stack_Array[INIT_TASK_STACK_SIZE];

void DebugTask(void *argument) {
    while(1) {
        set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(500));
        set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("Mika Sucks Balls\n");
    }

}

int main(void) {
    HAL_Init();
    GPIO_Init();
    ADC1_Init();

    initPrintf();

    xTaskCreateStatic(
        DebugTask,
        "Debugging sucks",
        INIT_TASK_STACK_SIZE,
        NULL,
        INIT_TASK_PRIORITY,
        INIT_TASK_Stack_Array,
        &INIT_TASK_TCB

    );
    vTaskStartScheduler();

 
    while (1) {
        #ifdef USART1
            set_LED(BRAKE_POT_LED, GPIO_PIN_SET);
            HAL_Delay(500);
        #endif
        #ifdef USART1
            set_LED(BRAKE_POT_LED, GPIO_PIN_RESET);
            HAL_Delay(500);
        #endif


        //printf("Mika Sucks Balls\n");
        //HAL_Delay(500);
    }
}