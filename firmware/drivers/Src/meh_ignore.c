//#include "Pedals_CAN.h"

//GPIO_Pin PEDALS_CAN_RX = {GPIOB, GPIO_PIN_8};
//GPIO_Pin PEDALS_CAN_TX = {GPIOB, GPIO_PIN_9};

//PedalsMsg pedals_msg = {
//	/* -------------- Data -------------- */
//	.brakePot = 0,
//	.brakePot_Voltage = 0,
//	.brakePot_Redundant = 0,
//	.brakePot_Redundant_Voltage = 0,

//	.accelPot = 0,
//	.accelPot_Voltage = 0,
//	.accelPot_Redundant = 0,
//	.accelPot_Redundant_Voltage = 0,

//	.brakeFL_front = 0,
//	.brakeFL_front_Voltage = 0,
//	.brakeFL_back = 0,
//	.brakeFL_back_Voltage = 0,

//	/* -------------- Faults -------------- */
//	.faults = 63 // all faults set to 1
//};

///*
//	-------------- CAN status types --------------

//	CAN_ERR,   // unsuccessful operation
//	CAN_OK,
//	CAN_SENT,  // successful send
//	CAN_RECV,  // successful recieve
//	CAN_EMPTY, // recieved nothing with no errors

//	--------------  --------------  --------------
//*/

///**
// * @brief CAN1 Initialization Function
// * @param None
// * @retval None
// */
//static void MX_CAN_Init(void) {

//	hcan1.Instance = CAN1;
//	hcan1.Init.Prescaler = 20;
//	hcan1.Init.Mode = CAN_MODE_NORMAL;
//	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
//	hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
//	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
//	hcan1.Init.TimeTriggeredMode = DISABLE;
//	hcan1.Init.AutoBusOff = DISABLE;
//	hcan1.Init.AutoWakeUp = DISABLE;
//	hcan1.Init.AutoRetransmission = ENABLE;
//	hcan1.Init.ReceiveFifoLocked = DISABLE;
//	hcan1.Init.TransmitFifoPriority = DISABLE;
//	if (HAL_CAN_Init(&hcan1) != HAL_OK) {
//		Error_Handler();
//	}
//}

//void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {}

///* --------------------------------------------------
//	Pedals CAN functions
//-------------------------------------------------- */

//PedalsStatus pedals_CAN_init() {
//	if (!MX_CAN_Init())
//		return PEDALS_CAN_INIT_FAIL;
//	else
//		return PEDALS_OK;
//}

//PedalsStatus pedals_CAN_start() {
//	if (can_start(hcan1) != CAN_OK)
//		return PEDALS_CAN_START_FAIL;
//	return PEDALS_OK;
//}

//static void potsP_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
//	tx_data[0] = (uint8_t)msg.accelPot;
//	tx_data[1] = (uint8_t)msg.accelPot_Redundant;
//	tx_data[2] = (uint8_t)msg.brakePot;
//	tx_data[3] = (uint8_t)msg.faults;
//}

//static void potsV_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
//	// split 16 bit fixed point mV values to two 8 bit vals (little endian - LSB
//	// is 1st byte)
//	tx_data[0] = (uint8_t)(msg.accelPot_Voltage & 0xFF);
//	tx_data[1] = (uint8_t)((msg.accelPot_Voltage >> 8) & 0xFF);

//	tx_data[2] = (uint8_t)(msg.accelPot_Redundant_Voltage & 0xFF);
//	tx_data[3] = (uint8_t)((msg.accelPot_Redundant_Voltage >> 8) & 0xFF);

//	tx_data[4] = (uint8_t)(msg.brakePot_Voltage & 0xFF);
//	tx_data[5] = (uint8_t)((msg.brakePot_Voltage >> 8) & 0xFF);

//	tx_data[6] = (uint8_t)(msg.brakePot_Redundant_Voltage & 0xFF);
//	tx_data[7] = (uint8_t)((msg.brakePot_Redundant_Voltage >> 8) & 0xFF);
//}

//static void brakeFL_FillPayload(uint8_t *tx_data, PedalsMsg msg) {
//	// split 16 bit fixed point mV values to two 8 bit vals (little endian - LSB
//	// is 1st byte)
//	tx_data[0] = (uint8_t)(msg.accelPot_Voltage & 0xFF);
//	tx_data[1] = (uint8_t)((msg.accelPot_Voltage >> 8) & 0xFF);

//	tx_data[2] = (uint8_t)(msg.accelPot_Redundant_Voltage & 0xFF);
//	tx_data[3] = (uint8_t)((msg.accelPot_Redundant_Voltage >> 8) & 0xFF);

//	tx_data[4] = (uint8_t)(msg.brakePot_Voltage & 0xFF);
//	tx_data[5] = (uint8_t)((msg.brakePot_Voltage >> 8) & 0xFF);

//	tx_data[6] = (uint8_t)(msg.brakePot_Redundant_Voltage & 0xFF);
//	tx_data[7] = (uint8_t)((msg.brakePot_Redundant_Voltage >> 8) & 0xFF);
//}

//PedalsStatus pedals_CAN_send_PotsP(PedalsMsg msg, TickType_t delayTicks) {
//	// Create CAN payload - test only for pots
//	CAN_TxHeaderTypeDef tx_header = {0};
//	tx_header.StdId = POTS_P_MSG_ID;
//	tx_header.RTR = CAN_RTR_DATA;
//	tx_header.IDE = CAN_ID_STD;
//	tx_header.DLC = POTS_P_MSG_DLC;
//	tx_header.TransmitGlobalTime = DISABLE;

//	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
//	potsP_FillPayload(tx_data, msg);

//	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
//		return PEDALS_CAN_SEND_FAIL;

//	return PEDALS_OK;
//}

//PedalsStatus pedals_CAN_send_PotsV(PedalsMsg msg, TickType_t delayTicks) {
//	// Create CAN payload - test only for pots
//	CAN_TxHeaderTypeDef tx_header = {0};
//	tx_header.StdId = POTS_V_MSG_ID;
//	tx_header.RTR = CAN_RTR_DATA;
//	tx_header.IDE = CAN_ID_STD;
//	tx_header.DLC = POTS_V_MSG_DLC;
//	tx_header.TransmitGlobalTime = DISABLE;

//	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
//	potsV_FillPayload(tx_data, msg);

//	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
//		return PEDALS_CAN_SEND_FAIL;

//	return PEDALS_OK;
//}

//PedalsStatus pedals_CAN_send_BrakeFL(PedalsMsg msg, TickType_t delayTicks) {
//	// Create CAN payload - test only for pots
//	CAN_TxHeaderTypeDef tx_header = {0};
//	tx_header.StdId = BRAKE_FL_MSG_ID;
//	tx_header.RTR = CAN_RTR_DATA;
//	tx_header.IDE = CAN_ID_STD;
//	tx_header.DLC = BRAKE_FL_MSG_DLC;
//	tx_header.TransmitGlobalTime = DISABLE;

//	uint8_t tx_data[POTS_P_MSG_DLC] = {0};
//	brakeFL_FillPayload(tx_data, msg);

//	if (can_send(hcan1, &tx_header, tx_data, delayTicks) != CAN_OK)
//		return PEDALS_CAN_SEND_FAIL;

//	return PEDALS_OK;
//}

//PedalsStatus pedals_CAN_stop() {
//	return can_stop(hcan1) == CAN_OK ? PEDALS_OK : PEDALS_CAN_STOP_FAIL;
//}