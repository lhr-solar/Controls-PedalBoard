#include "Pedals.h"
#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

#define CAN_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CAN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t CAN_TASK_TCB;
StackType_t CAN_TASK_Stack_Array[CAN_TASK_STACK_SIZE];

void pedals_CAN_Send_test(void *argument);
void CAN_Error_Handler();

PedalsMsg test_data = {
	/* -------------- Data -------------- */
	.brakePot = 100,
	.brakePot_Voltage = 90,
	.brakePot_Redundant = 80,
	.brakePot_Redundant_Voltage = 70,

	.accelPot = 60,
	.accelPot_Voltage = 40,
	.accelPot_Redundant = 30,
	.accelPot_Redundant_Voltage = 2000,

	.brakeFL_front = 10,
	.brakeFL_front_Voltage = 200,
	.brakeFL_back = 67,
	.brakeFL_back_Voltage = 400,

	/* -------------- Faults -------------- */
	.faults = 0 // clear faults
};

int main() {

	HAL_Init();
	SystemClock_Config();

	xTaskCreateStatic(pedals_CAN_Send_test, "CAN Send testing",
					  CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY,
					  CAN_TASK_Stack_Array, &CAN_TASK_TCB);

	vTaskStartScheduler();

	while (1) {
	}
	return 67;
}

void pedals_CAN_Send_test(void *argument) {
	initPrintf();
	Status_LEDs_Init();
	adc_GPIO_init();

	toggle_LED(BRAKE_FL_LED);

	printf("init done");

	if (pedals_CAN_init() != PEDALS_OK) {
		printf("you are dum - init failed\n\r");
	}

	if (pedals_CAN_start() != PEDALS_OK) {
		printf("you are quite dum - start failed\n\r");
	}

	while (1) {

		if (pedals_CAN_send_PotsP(test_data, portMAX_DELAY) != PEDALS_OK) {
			printf("mayne CAN send failed");
			CAN_Error_Handler();
		} else {
			printf("can sent!\n\r");
		}

		printf("shuopu");

		toggle_LED(PSOM_HB);
		vTaskDelay(pdMS_TO_TICKS(500));

	}
}

void CAN_Error_Handler() {
	printf("CAN failed :(");
	while (1) {
	}
}