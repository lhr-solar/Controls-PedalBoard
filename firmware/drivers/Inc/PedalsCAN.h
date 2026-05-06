#pragma once

#include "CAN.h"
#include "CarCAN_can_msgs.h"
#include "PedalsSensors.h"

#define CAN_INTERRUPT_PRIO   configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define CARCAN_TIMEOUT_TICKS pdMS_TO_TICKS(250)
#define CANTX_ITEM_SIZE      sizeof(can_tx_payload_t)
#define CANTX_QUEUE_LENGTH   20

extern QueueHandle_t can_tx_queue;
extern CAN_HandleTypeDef *hcan1;

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan);
PedalsStatus_t pedals_can_init(void);
PedalsStatus_t pedals_can_stop(void);

PedalsStatus_t pedals_can_send_pedal_brake_adc(CAN_TxHeaderTypeDef *tx_header, pedal_brake_adc_t *payload);
PedalsStatus_t pedals_can_send_pedal_accel_adc(CAN_TxHeaderTypeDef *tx_header, pedal_accel_adc_t *payload);
PedalsStatus_t pedals_can_send_brake_pressure_1_voltage(CAN_TxHeaderTypeDef *tx_header, brake_pressure_1_t *payload);
PedalsStatus_t pedals_can_send_brake_pressure_2_voltage(CAN_TxHeaderTypeDef *tx_header, brake_pressure_2_t *payload);
PedalsStatus_t pedals_can_send_pedals_status(CAN_TxHeaderTypeDef *tx_header, pedal_status_t *payload);

void pedals_print_payload(void);
