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

/* --------------------------------------------------
	Pedals CAN Packet
	Brake = 48b
	Accel = 48b
	FL Brake = 48b
	Faults = 6 bits
-------------------------------------------------- */


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

------------------------------------ */
extern int8_t Pedals_Pots_Faults_t;
extern accel_brake_position_t Pedals_Accel_Brake_Percent_Msg;
extern accel_brake_position_voltage_t Pedals_Accel_brake_Voltage_Msg;
extern brake_pressure_t Pedals_Brake_FL_Msg; 


/* --------------------------------------------------
	Pedals CAN functions
-------------------------------------------------- */

/**
 * @brief Init HAL CAN properties
 *
 * Configs ADC channel queue and puts the ADC value read onto the queue
 *
 * @param hcan       pointer to hcan handle from Embedded Sharepoint
 * 
 *
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan);

/**
 * @brief Calls Cube_MX CAN init functions
 *
 * Purpose is to create a wrapper function for CAN init
 * error handling
 *
 * @return Pedals_Status_t  return PEDALS_OK if all is jolly, else get to debugging boi
 * 
 */
Pedals_Status_t pedals_CAN_init();

/**
 * @brief Wrapper function for can_stop(), custom error handling.
 *
 * @return Pedals_Status_t  return PEDALS_OK if all is jolly, else get to debugging boi
 * 
 */
Pedals_Status_t pedals_CAN_stop();


/**
 * @brief Packs Payload with Data from Pedals_Msg_t
 *
 *
 * @param tx_header      pointer to payload's header for accessing byte values on payload
 * @param msg            pointer to Pedals_Msg_t with all data to be transmitted
 * @param tx_data 		 array on call stack that acts as temporary payload, popped after CAN transmission
 *
 * @return Pedals_Status_t  return PEDALS_OK if all is jolly, else get to debugging boi
 * 
 *
 */
Pedals_Status_t pedals_CAN_send_PotsPercent(CAN_TxHeaderTypeDef* tx_header, Pedals_Msg_t* msg, uint8_t tx_data[8]);