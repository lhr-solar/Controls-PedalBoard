#include "Pedals.h"
#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

#define CAN_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CAN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
StaticTask_t CAN_TASK_TCB;
StackType_t CAN_TASK_Stack_Array[CAN_TASK_STACK_SIZE];

void pedals_CAN_Send_test(void *argument);
void CAN_Error_Handler_Brake_Voltage();
void CAN_Error_Handler_Accel_Voltage();
void CAN_Error_Handler_Brake_Pressure_1_Voltage();
void CAN_Error_Handler_Brake_Pressure_2_Voltage();
void CAN_Error_Handler_Pedals_Status();
void CAN_Error_Handler();

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
	sensors_adc_GPIO_init();
	Status_LEDs_Init();

	printf("init done\n\r");

	if (sensors_adc_init() != ADC_OK) {
		if (ENABLE_DEBUG)
			printf("ADC_Init Failed");
		return;
	}

	if (pedals_CAN_init() != PEDALS_OK) {
		printf("you are dum - init failed\n\r");
		CAN_Error_Handler();
	}

	CAN_TxHeaderTypeDef tx_header = {0};

	while (1) {
		readAll_ADCs();
		pedal_brake_adc_t brake_payload = read_pedal_brake_adc();
		pedal_accel_adc_t accel_payload = read_pedal_accel_adc();
		brake_pressure_1_t brake_pressure_1_payload = read_brakeFL_1_raw_voltage();
		brake_pressure_2_t brake_pressure_2_payload = read_brakeFL_2_raw_voltage();

		pedal_status_t pedals_status_payload = read_main_positions_and_faults();
		
		if (pedals_CAN_send_pedal_brake_adc(&tx_header, brake_payload) != PEDALS_OK)
			CAN_Error_Handler_Brake_Voltage();
		if (pedals_CAN_send_pedal_accel_adc(&tx_header, accel_payload) != PEDALS_OK)
			CAN_Error_Handler_Accel_Voltage();
		if (pedals_CAN_send_brake_pressure_1_voltage(&tx_header, brake_pressure_1_payload) != PEDALS_OK) CAN_Error_Handler_Brake_Pressure_1_Voltage();
		if (pedals_CAN_send_brake_pressure_2_voltage(&tx_header, brake_pressure_2_payload) != PEDALS_OK) CAN_Error_Handler_Brake_Pressure_2_Voltage();
		if (pedals_CAN_send_pedals_status(&tx_header, pedals_status_payload) != PEDALS_OK) CAN_Error_Handler_Pedals_Status();
		printf("%ld\n\r", tx_header.DLC);

		if(ENABLE_DEBUG) pedals_print_payload();

		led_toggle(PSOM_HB_PORT, PSOM_HB_PIN);
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

void CAN_Error_Handler_Brake_Voltage() {
	printf("CAN failed to send brake voltage :(");
	while (1);
	
}

void CAN_Error_Handler_Accel_Voltage() {
	printf("CAN failed to send accel voltage :(");
	while (1);
}

void CAN_Error_Handler_Brake_Pressure_1_Voltage() {
	printf("CAN failed to send brake pressure 1 voltage :(");
	while (1);
}

void CAN_Error_Handler_Brake_Pressure_2_Voltage() {
	printf("CAN failed to send brake pressure 2 voltage :(");
	while (1);
}

void CAN_Error_Handler_Pedals_Status() {
	printf("CAN failed to send pedals status :(");
	while (1);
}

void CAN_Error_Handler() {
	printf("CAN initialization failed :(");
	while(1);
}

