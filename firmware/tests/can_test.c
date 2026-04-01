#include "Pedals.h"
#include "Pedals_CAN.h"

#define CAN_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CAN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t CAN_Task_TCB;
StackType_t CAN_Task_Stack_Array[CAN_TASK_STACK_SIZE];

int main() {
  HAL_Init();
  SystemClock_Config();
  initPrintf();
  pedals_CAN_init();

  xTaskCreateStatic(pedals_CAN_test, "CAN testing", CAN_TASK_STACK_SIZE, NULL,
                    CAN_TASK_PRIORITY, CAN_Task_Stack_Array, &CAN_Task_TCB);

  vTaskStartScheduler();

  while (1) {
    printf("YOU are dumb asf\n\r");
  }

  return 0;
}

//void pedals_CAN_test(void *argument) { vTaskDelay(pdMS_TO_TICKS(1000)); }