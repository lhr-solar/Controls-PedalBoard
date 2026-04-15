#include "Pedals_CAN.h"
#include "Pedals_Sensors.h"
#include "LUT.h"

QueueHandle_t can_tx_queue;
uint8_t can_tx_qStorage[CAN_TX_QUEUE_LENGTH * CAN_TX_ITEM_SIZE];
StaticQueue_t xStaticQueue_can_tx;

int8_t Pedals_Faults_t = 0b00000000;

/*
	-------------- CAN status types --------------

	CAN_ERR,   // unsuccessful operation
	CAN_OK,
	CAN_SENT,  // successful send
	CAN_RECV,  // successful recieve
	CAN_EMPTY, // recieved nothing with no errors

	--------------  --------------  --------------
*/

Pedals_Status_t MX_CAN_Init(void) {
	// taken from cubemx
	/* USER CODE BEGIN CAN1_Init 0 */
	/* Initialize queue */
	can_tx_queue = xQueueCreateStatic(CAN_TX_QUEUE_LENGTH, CAN_TX_ITEM_SIZE,
									  can_tx_qStorage, &xStaticQueue_can_tx);
	if (can_tx_queue == NULL)
		return PEDALS_CAN_INIT_FAIL;

	/* USER CODE END CAN1_Init 0 */

	/* USER CODE BEGIN CAN1_Init 1 */

	// create filter
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	/* USER CODE END CAN1_Init 1 */
	hcan1->Instance = CAN1;
	hcan1->Init.Prescaler = 20;
	hcan1->Init.Mode = CAN_MODE_NORMAL;
	hcan1->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1->Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1->Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1->Init.TimeTriggeredMode = DISABLE;
	hcan1->Init.AutoBusOff = DISABLE;
	hcan1->Init.AutoWakeUp = DISABLE;
	hcan1->Init.AutoRetransmission = ENABLE;
	hcan1->Init.ReceiveFifoLocked = DISABLE;
	hcan1->Init.TransmitFifoPriority = ENABLE;
	if (can_init(hcan1, &sFilterConfig) != CAN_OK) {
		return PEDALS_CAN_INIT_FAIL;
	}
	/* USER CODE BEGIN CAN1_Init 2 */
	if (can_start(hcan1) != CAN_OK) {
		return PEDALS_CAN_INIT_FAIL;
	}

	/* USER CODE END CAN1_Init 2 */
	return PEDALS_OK;
}

/**
 * @brief CAN MSP Initialization
 * Configures hardware: clock, GPIO, interrupts
 * @param hcan: CAN handle pointer
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (hcan->Instance == CAN1) {
		/* Peripheral clock enable */
		__HAL_RCC_CAN1_CLK_ENABLE();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**CAN1 GPIO Configuration
		PB8     ------> CAN1_RX
		PB9     ------> CAN1_TX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* CAN1 interrupt Init */
		HAL_NVIC_SetPriority(CAN1_TX_IRQn, CAN_INTERRUPT_PRIO, 0);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, CAN_INTERRUPT_PRIO, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	}
}

/* --------------------------------------------------
	Pedals CAN functions
-------------------------------------------------- */

Pedals_Status_t pedals_CAN_init() {
	if (MX_CAN_Init() != PEDALS_OK)
		return PEDALS_CAN_INIT_FAIL;
	else
		return PEDALS_OK;
}

static void Pack_Brake_Voltage_CANHeader(CAN_TxHeaderTypeDef *tx_header) {
	tx_header->StdId = CAN_ID_PEDAL_BRAKE_RAWV;
	tx_header->RTR = CAN_RTR_DATA;
	tx_header->IDE = CAN_ID_STD;
	tx_header->DLC = CAN_DLC_PEDAL_BRAKE_RAWV;
	tx_header->TransmitGlobalTime = DISABLE;
}

static void Pack_Accel_Voltage_CANHeader(CAN_TxHeaderTypeDef *tx_header) {
	tx_header->StdId = CAN_ID_PEDAL_ACCEL_RAWV;
	tx_header->RTR = CAN_RTR_DATA;
	tx_header->IDE = CAN_ID_STD;
	tx_header->DLC = CAN_DLC_PEDAL_ACCEL_RAWV;
	tx_header->TransmitGlobalTime = DISABLE;
}

static void Pack_Brake_Pressure_1_CANHeader(CAN_TxHeaderTypeDef *tx_header) {
	tx_header->StdId = CAN_ID_BRAKE_PRESSURE_1;
	tx_header->RTR = CAN_RTR_DATA;
	tx_header->IDE = CAN_ID_STD;
	tx_header->DLC = CAN_DLC_BRAKE_PRESSURE_1;
	tx_header->TransmitGlobalTime = DISABLE;
}

static void Pack_Brake_Pressure_2_CANHeader(CAN_TxHeaderTypeDef *tx_header) {
	tx_header->StdId = CAN_ID_BRAKE_PRESSURE_2;
	tx_header->RTR = CAN_RTR_DATA;
	tx_header->IDE = CAN_ID_STD;
	tx_header->DLC = CAN_DLC_BRAKE_PRESSURE_2;
	tx_header->TransmitGlobalTime = DISABLE;
}

static void Pack_Pedals_Status_CANHeader(CAN_TxHeaderTypeDef *tx_header) {
	tx_header->StdId = CAN_ID_PEDAL_STATUS;
	tx_header->RTR = CAN_RTR_DATA;
	tx_header->IDE = CAN_ID_STD;
	tx_header->DLC = CAN_DLC_PEDAL_STATUS;
	tx_header->TransmitGlobalTime = DISABLE;
}

Pedals_Status_t pedals_CAN_send_brake_voltage(CAN_TxHeaderTypeDef *tx_header,
											  pedal_brake_rawv_t payload) {
	Pack_Brake_Voltage_CANHeader(tx_header);
	uint8_t tx_data[tx_header->DLC];

	tx_data[0] = (payload.BrakePedal_Main_RawV) & 0xFF;
	tx_data[1] = (payload.BrakePedal_Main_RawV >> 8) & 0xFF;
	tx_data[2] = (payload.BrakePedal_Redundant_RawV) & 0xFF;
	tx_data[3] = (payload.BrakePedal_Redundant_RawV >> 8) & 0xFF;
	tx_data[4] = (payload.FrameID_Pedals);

	if (can_send(hcan1, tx_header, tx_data, CAR_CAN_TIMEOUT_TICKS) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;
	return PEDALS_OK;
}

Pedals_Status_t pedals_CAN_send_accel_voltage(CAN_TxHeaderTypeDef *tx_header,
											  pedal_accel_rawv_t payload) {
	Pack_Accel_Voltage_CANHeader(tx_header);
	uint8_t tx_data[tx_header->DLC];

	tx_data[0] = (payload.AccelPedal_Main_RawV) & 0xFF;
	tx_data[1] = (payload.AccelPedal_Main_RawV >> 8) & 0xFF;
	tx_data[2] = (payload.AccelPedal_Redundant_RawV) & 0xFF;
	tx_data[3] = (payload.AccelPedal_Redundant_RawV >> 8) & 0xFF;
	tx_data[4] = (payload.FrameID_Pedals);

	if (can_send(hcan1, tx_header, tx_data, CAR_CAN_TIMEOUT_TICKS) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;
	return PEDALS_OK;
}


Pedals_Status_t pedals_CAN_send_brake_pressure_1_voltage(CAN_TxHeaderTypeDef *tx_header,
											  brake_pressure_1_t payload) {
	Pack_Brake_Pressure_1_CANHeader(tx_header);
	uint8_t tx_data[tx_header->DLC];

	tx_data[0] = (payload.Brake_Pressure) & 0xFF;
	tx_data[1] = (payload.Brake_Pressure >> 8) & 0xFF;
	tx_data[2] = (payload.Brake_Pressure_RawV) & 0xFF;
	tx_data[3] = (payload.Brake_Pressure_RawV >> 8) & 0xFF;
	tx_data[4] = (payload.FrameID_Pedals);

	if (can_send(hcan1, tx_header, tx_data, CAR_CAN_TIMEOUT_TICKS) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;
	return PEDALS_OK;
}


Pedals_Status_t pedals_CAN_send_brake_pressure_2_voltage(CAN_TxHeaderTypeDef *tx_header,
											  brake_pressure_2_t payload) {
	Pack_Brake_Pressure_2_CANHeader(tx_header);
	uint8_t tx_data[tx_header->DLC];

	tx_data[0] = (payload.Brake_Pressure) & 0xFF;
	tx_data[1] = (payload.Brake_Pressure >> 8) & 0xFF;
	tx_data[2] = (payload.Brake_Pressure_RawV) & 0xFF;
	tx_data[3] = (payload.Brake_Pressure_RawV >> 8) & 0xFF;
	tx_data[4] = (payload.FrameID_Pedals);

	if (can_send(hcan1, tx_header, tx_data, CAR_CAN_TIMEOUT_TICKS) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;
	return PEDALS_OK;
}

Pedals_Status_t pedals_CAN_send_pedals_status(CAN_TxHeaderTypeDef *tx_header,
											  pedal_status_t payload) {
	Pack_Pedals_Status_CANHeader(tx_header);
	uint8_t tx_data[tx_header->DLC];

	tx_data[0] = (payload.AccelPedal_Main_Pos);
	tx_data[1] = (payload.AccelPedal_Redundant_Pos);
	tx_data[2] = (payload.BrakePedal_Main_Pos);
	tx_data[3] = (payload.BrakePedal_Redundant_Pos);
	tx_data[4] = (payload.AccelPedal_Main_Fault >> 0) | (payload.AccelPedal_Redundant_Fault << 1) | (payload.BrakePedal_Main_Fault << 2) |
				 (payload.BrakePedal_Redundant_Fault << 3) | (payload.Brake_Pressure_1_Fault << 4) |
				 (payload.Brake_Pressure_2_Fault << 5);
	tx_data[5] = (payload.FrameID_Pedals);

	if (can_send(hcan1, tx_header, tx_data, CAR_CAN_TIMEOUT_TICKS) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;
	return PEDALS_OK;
}

static void print_raw_vals() {
	printf("Brake %%: %lu\n\r", raw_vals[0]);
	printf("Accel %%: %lu\n\r", raw_vals[1]);
	printf("Brake FL Front %%: %lu\n\r", raw_vals[2]);
	printf("Brake FL Back %%: %lu\n\r", raw_vals[3]);
	printf("Brake Pot Redundant %%: %lu\n\r", raw_vals[4]);
	printf("Accel Pot Redundant %%: %lu\n\r", raw_vals[5]);
}

void  pedals_print_payload() {
	printf("Brake Pedal Main RawV: %u mV  |  Brake Pedal Redundant RawV: %u mV\n\r",
		   (read_brake_raw_voltage()).BrakePedal_Main_RawV,
		   (read_brake_raw_voltage()).BrakePedal_Redundant_RawV);
	printf("Accel Pedal Main RawV: %u mV  |  Accel Pedal Redundant RawV: %u mV\n\r",
		   (read_accel_raw_voltage()).AccelPedal_Main_RawV,
		   (read_accel_raw_voltage()).AccelPedal_Redundant_RawV);
	printf("Brake FL Front Pressure: %u mV  |  Brake FL Back Pressure: %u mV\n\r",
		   (read_brakeFL_1_raw_voltage()).Brake_Pressure,
		   (read_brakeFL_2_raw_voltage()).Brake_Pressure);

	print_raw_vals();
}


Pedals_Status_t pedals_CAN_stop() {
	return can_stop(hcan1) == CAN_OK ? PEDALS_OK : PEDALS_CAN_STOP_FAIL;
}
