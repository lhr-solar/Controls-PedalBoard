#include "Pedals.h"
#include "StatusLEDs.h"

StaticTask_t Task_Heartbeat_TCB;
StackType_t Task_Heartbeat_Stack_Array[HEARTBEAT_TASK_STACK_SIZE];

void Task_Heartbeat(void *argument) {
	(void)argument;

	for (;;) {
		led_toggle(PSOM_HB_PORT, PSOM_HB_PIN);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
