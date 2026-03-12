#include "Pedals_CAN.h"

GPIO_Pin PEDALS_CAN_RX = {GPIOB, GPIO_PIN_8};
GPIO_Pin PEDALS_CAN_TX = {GPIOB, GPIO_PIN_9};

PedalsMsg pedals_msg = {
	/* -------------- Data -------------- */
	.brakePot = 0,
	.brakePot_Voltage = 0,
	.brakePot_Redundant = 0,
	.brakePot_Redundant_Voltage = 0,

	.accelPot = 0,
	.accelPot_Voltage = 0,
	.accelPot_Redundant = 0,
	.accelPot_Redundant_Voltage = 0,

	.brakeFL_front = 0,
	.brakeFL_front_Voltage = 0,
	.brakeFL_back = 0,
	.brakeFL_back_Voltage = 0,

	/* -------------- Faults -------------- */
	.faults = 63 // all faults set to 1
};

/*
	-------------- CAN status types --------------

	CAN_ERR,   // unsuccessful operation
	CAN_OK,
	CAN_SENT,  // successful send
	CAN_RECV,  // successful recieve
	CAN_EMPTY, // recieved nothing with no errors

	--------------  --------------  --------------
*/

static bool MX_CAN_Init() {
	/* Create CAN filter */
	/* For production, reject all incoming IDs */
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterActivation = DISABLE;

	/* For testing: accept all incoming IDs */
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

	/* CAN1 Init Struct */
	// Baud rate is 250 kbit/s
	hcan1->Instance = CAN1;
	hcan1->Init.Prescaler = 20;
	hcan1->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1->Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1->Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1->Init.Mode = CAN_MODE_NORMAL; // TODO: change for testing
	hcan1->Init.TimeTriggeredMode = DISABLE;
	hcan1->Init.AutoBusOff = DISABLE;
	hcan1->Init.AutoWakeUp = DISABLE;
	hcan1->Init.AutoRetransmission = DISABLE;
	hcan1->Init.ReceiveFifoLocked = DISABLE;

	// If TransmitFifoPriority is disabled, the hardware selects the mailbox
	// based on the message ID priority. If enabled, the hardware uses a FIFO
	// mechanism to select the mailbox based on the order of transmission
	// requests.
	hcan1->Init.TransmitFifoPriority = ENABLE;

	/* Initialize CAN1 */
	if (can_init(hcan1, &sFilterConfig) != CAN_OK)
		return false;

	return true;
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
		GPIO_InitStruct.Pin = PEDALS_CAN_RX.pin | PEDALS_CAN_TX.pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
		HAL_GPIO_Init(PEDALS_CAN_RX.port, &GPIO_InitStruct);

		/* CAN1 interrupt Init */
		HAL_NVIC_SetPriority(CAN1_TX_IRQn,
							 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn,
							 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	}
}

/* --------------------------------------------------
	Pedals CAN functions
-------------------------------------------------- */

PedalsStatus pedals_CAN_init() {
	if (!MX_CAN_Init())
		return PEDALS_CAN_INIT_FAIL;
	else
		return PEDALS_OK;
}

PedalsStatus pedals_CAN_start() {
	if (can_start(hcan1) != CAN_OK)
		return PEDALS_CAN_START_FAIL;
	return PEDALS_OK;
}

static void potsP_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
	tx_data[0] = (uint8_t)msg.accelPot;
	tx_data[1] = (uint8_t)msg.accelPot_Redundant;
	tx_data[2] = (uint8_t)msg.brakePot;
	tx_data[3] = (uint8_t)msg.faults;
}

static void potsV_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
	// split 16 bit fixed point mV values to two 8 bit vals (little endian - LSB
	// is 1st byte)
	tx_data[0] = (uint8_t)(msg.accelPot_Voltage & 0xFF);
	tx_data[1] = (uint8_t)((msg.accelPot_Voltage >> 8) & 0xFF);

	tx_data[2] = (uint8_t)(msg.accelPot_Redundant_Voltage & 0xFF);
	tx_data[3] = (uint8_t)((msg.accelPot_Redundant_Voltage >> 8) & 0xFF);

	tx_data[4] = (uint8_t)(msg.brakePot_Voltage & 0xFF);
	tx_data[5] = (uint8_t)((msg.brakePot_Voltage >> 8) & 0xFF);

	tx_data[6] = (uint8_t)(msg.brakePot_Redundant_Voltage & 0xFF);
	tx_data[7] = (uint8_t)((msg.brakePot_Redundant_Voltage >> 8) & 0xFF);
}

static void brakeFL_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
	// split 16 bit fixed point mV values to two 8 bit vals (little endian - LSB
	// is 1st byte)
	tx_data[0] = (uint8_t)(msg.accelPot_Voltage & 0xFF);
	tx_data[1] = (uint8_t)((msg.accelPot_Voltage >> 8) & 0xFF);

	tx_data[2] = (uint8_t)(msg.accelPot_Redundant_Voltage & 0xFF);
	tx_data[3] = (uint8_t)((msg.accelPot_Redundant_Voltage >> 8) & 0xFF);

	tx_data[4] = (uint8_t)(msg.brakePot_Voltage & 0xFF);
	tx_data[5] = (uint8_t)((msg.brakePot_Voltage >> 8) & 0xFF);

	tx_data[6] = (uint8_t)(msg.brakePot_Redundant_Voltage & 0xFF);
	tx_data[7] = (uint8_t)((msg.brakePot_Redundant_Voltage >> 8) & 0xFF);
}

PedalsStatus pedals_CAN_send_PotsP(PedalsMsg msg, TickType_t delayTicks) {
	// Create CAN payload - test only for pots
	CAN_TxHeaderTypeDef tx_header = {0};
	tx_header.StdId = POTS_P_MSG_ID;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.IDE = CAN_ID_STD;
	tx_header.DLC = POTS_P_MSG_DLC;
	tx_header.TransmitGlobalTime = DISABLE;

	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
	potsP_FillPayload(tx_data, msg);

	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;

	return PEDALS_OK;
}

PedalsStatus pedals_CAN_send_PotsV(PedalsMsg msg, TickType_t delayTicks) {
	// Create CAN payload - test only for pots
	CAN_TxHeaderTypeDef tx_header = {0};
	tx_header.StdId = POTS_V_MSG_ID;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.IDE = CAN_ID_STD;
	tx_header.DLC = POTS_V_MSG_DLC;
	tx_header.TransmitGlobalTime = DISABLE;

	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
	potsV_FillPayload(tx_data, msg);

	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;

	return PEDALS_OK;
}

PedalsStatus pedals_CAN_send_BrakeFL(PedalsMsg msg, TickType_t delayTicks) {
	// Create CAN payload - test only for pots
	CAN_TxHeaderTypeDef tx_header = {0};
	tx_header.StdId = BRAKE_FL_MSG_ID;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.IDE = CAN_ID_STD;
	tx_header.DLC = BRAKE_FL_MSG_DLC;
	tx_header.TransmitGlobalTime = DISABLE;

	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
	brakeFL_FillPayload(tx_data, msg);

	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
		return PEDALS_CAN_SEND_FAIL;

	return PEDALS_OK;
}

PedalsStatus pedals_CAN_stop() {
	return can_stop(hcan1) == CAN_OK ? PEDALS_OK : PEDALS_CAN_STOP_FAIL;
}