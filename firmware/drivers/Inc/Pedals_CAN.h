#include "CAN.h"
#include "CarCAN_can_msgs.h"
#include "Pedals.h"

#define POTS_P_MSG_ID CAN_ID_ACCEL_BRAKE_POSITION // Percent value 1 byte data
#define POTS_V_MSG_ID                                                          \
	CAN_ID_ACCEL_BRAKE_POSITION_VOLTAGE // Fixed point Voltage (mV) 2 byte data
#define BRAKE_FL_MSG_ID                                                        \
	CAN_ID_BRAKE_PRESSURE // raw adc val (2 bytes) + fixed point voltage (mV) (2
						  // bytes)

#define CAN_INTERRUPT_PRIO configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

#define POTS_PERCENT_QUEUE_SIZE 5
#define POTS_VOLTAGE_QUEUE_SIZE 5
#define BRAKE_FL_QUEUE_SIZE 5

#define POTS_P_MSG_DLC (5) // size of payload in bytes
#define POTS_V_MSG_DLC (8)
#define BRAKE_FL_MSG_DLC (8)

#define CAN_TX_ITEM_SIZE sizeof(can_tx_payload_t)
#define CAN_TX_QUEUE_LENGTH 20
extern QueueHandle_t can_tx_queue;

extern CAN_HandleTypeDef *hcan1;

// returned by MX_CAN_Init()
typedef enum CarCAN_Status {
    CAN_INIT_FAIL,
    CAN_INIT_OK,
    CAN_SEND_FAIL,
    CAN_SEND_OK,
} CarCAN_Status_t;

/* --------------------------------------------------
	Pedals CAN Packet
	Brake = 48b
	Accel = 48b
	FL Brake = 48b
	Faults = 6 bits
-------------------------------------------------- */

typedef struct PedalsMsg {
	/* -------------- Data -------------- */
	int8_t brakePot;
	int16_t brakePot_Voltage;
	int8_t brakePot_Redundant;
	int16_t brakePot_Redundant_Voltage;

	int8_t accelPot;
	int16_t accelPot_Voltage;
	int8_t accelPot_Redundant;
	int16_t accelPot_Redundant_Voltage;

	int8_t brakeFL_front;
	int16_t brakeFL_front_Voltage;
	int8_t brakeFL_back;
	int16_t brakeFL_back_Voltage;

	/* -------------- Faults --------------
		Bit map
			0 - Accel_pot
			1 - Accel_pot_redundant
			2 - Brake_pot
			3 - brake_pot_redundant
			4 - Brake_FL
			5 - Brake_FL_redundant
			6 - TBD
			7 - TBD

	   -------------- ------ -------------- */
	int8_t faults;

} PedalsMsg;

extern PedalsMsg pedals_msg;

/* --------------------------------------------------
	Pedals CAN functions
-------------------------------------------------- */

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan);

PedalsStatus pedals_CAN_init();
PedalsStatus pedals_CAN_start();
PedalsStatus pedals_CAN_stop();
void PackPotsPercentCANHeader(CAN_TxHeaderTypeDef *tx_header);
PedalsStatus pedals_CAN_send_PotsPercent(CAN_TxHeaderTypeDef* tx_header, PedalsMsg* msg, uint8_t tx_data[8]);