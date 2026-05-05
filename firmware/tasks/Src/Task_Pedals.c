#include "Pedals.h"
#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"

StaticTask_t Task_Pedals_TCB;
StackType_t Task_Pedals_Stack_Array[PEDALS_TASK_STACK_SIZE];

void Task_Pedals(void *argument) {
	(void)argument;

	CAN_TxHeaderTypeDef tx_header = {0};

	for (;;) {
		readAll_ADCs();

		pedal_brake_adc_t brake_adc = read_pedal_brake_adc();
		pedal_accel_adc_t accel_adc = read_pedal_accel_adc();
		pedal_status_t status = read_main_positions_and_faults();

		if (pedals_CAN_send_pedal_brake_adc(&tx_header, brake_adc) != PEDALS_OK)
			Pedals_Error_Handler();
		if (pedals_CAN_send_pedal_accel_adc(&tx_header, accel_adc) != PEDALS_OK)
			Pedals_Error_Handler();
		if (pedals_CAN_send_pedals_status(&tx_header, status) != PEDALS_OK)
			Pedals_Error_Handler();

		printf("POT pos %%: brake=%u accel=%u | ADC cnt: brake=%u accel=%u (red brk mV=%u red acc cnt=%u)\n\r",
			   (unsigned)status.BrakePedal_Main_Pos,
			   (unsigned)status.AccelPedal_Main_Pos,
			   (unsigned)brake_adc.BrakePedal_Main_ADC,
			   (unsigned)accel_adc.AccelPedal_Main_ADC,
			   (unsigned)brake_adc.BrakePedal_Redundant_ADC,
			   (unsigned)accel_adc.AccelPedal_Redundant_ADC);

		if (pedals_sample_ready_sem != NULL)
			(void)xSemaphoreGive(pedals_sample_ready_sem);

		vTaskDelay(pdMS_TO_TICKS(250));
	}
}
