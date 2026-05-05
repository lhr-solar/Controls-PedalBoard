#include "Pedals.h"
#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"

StaticTask_t Task_BrakePressure_TCB;
StackType_t Task_BrakePressure_Stack_Array[BRAKE_PRESSURE_TASK_STACK_SIZE];

void Task_BrakePressure(void *argument) {
	(void)argument;

	CAN_TxHeaderTypeDef tx_header = {0};

	for (;;) {
		if (pedals_sample_ready_sem != NULL)
			(void)xSemaphoreTake(pedals_sample_ready_sem, portMAX_DELAY);

		brake_pressure_1_t p1 = read_brakeFL_1_raw_voltage();
		brake_pressure_2_t p2 = read_brakeFL_2_raw_voltage();

		if (pedals_CAN_send_brake_pressure_1_voltage(&tx_header, p1) != PEDALS_OK)
			Pedals_Error_Handler();
		if (pedals_CAN_send_brake_pressure_2_voltage(&tx_header, p2) != PEDALS_OK)
			Pedals_Error_Handler();

		printf("PSI (0.1 decoded): P1=%u.%u P2=%u.%u | ADC cnt: P1=%u P2=%u\n\r",
			   (unsigned)(p1.Brake_Pressure / 10u),
			   (unsigned)(p1.Brake_Pressure % 10u),
			   (unsigned)(p2.Brake_Pressure / 10u),
			   (unsigned)(p2.Brake_Pressure % 10u),
			   (unsigned)p1.Brake_Pressure_ADC,
			   (unsigned)p2.Brake_Pressure_ADC);
	}
}
