#include "CAN.h"
#include "CarCAN_can_msgs.h"
#include "Pedals.h"


#define CAN_INTERRUPT_PRIO 		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define CAR_CAN_TIMEOUT_TICKS	pdMS_TO_TICKS(250)

#define POTS_PERCENT_QUEUE_SIZE 5
#define POTS_VOLTAGE_QUEUE_SIZE 5
#define BRAKE_FL_QUEUE_SIZE 5

#define CAN_TX_ITEM_SIZE sizeof(can_tx_payload_t)
#define CAN_TX_QUEUE_LENGTH 20
extern QueueHandle_t can_tx_queue;

extern CAN_HandleTypeDef *hcan1;


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


Pedals_Status_t pedals_CAN_send_pedal_brake_adc(CAN_TxHeaderTypeDef *tx_header,
												pedal_brake_adc_t payload);
Pedals_Status_t pedals_CAN_send_pedal_accel_adc(CAN_TxHeaderTypeDef *tx_header,
												pedal_accel_adc_t payload);
Pedals_Status_t pedals_CAN_send_brake_pressure_1_voltage(CAN_TxHeaderTypeDef *tx_header, brake_pressure_1_t payload);
Pedals_Status_t pedals_CAN_send_brake_pressure_2_voltage(CAN_TxHeaderTypeDef *tx_header, brake_pressure_2_t payload);


Pedals_Status_t pedals_CAN_send_pedals_status(CAN_TxHeaderTypeDef *tx_header, pedal_status_t payload);

void pedals_print_payload(void);