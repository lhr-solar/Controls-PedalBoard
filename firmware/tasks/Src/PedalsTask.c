#include "PedalsTask.h"
#include "InitTask.h"
#include "PedalsCAN.h"
#include "PedalsSensors.h"
#include "Debugging.h"


void Task_Pedals(void *args __attribute__((unused))) {
    CAN_TxHeaderTypeDef tx_header = {0};

    while (true) {
        readAllADCs();

        pedal_brake_adc_t brake_adc = read_pedal_brake_adc();
        pedal_accel_adc_t accel_adc = read_pedal_accel_adc();
        pedal_status_t    status    = read_main_positions_and_faults();
        brake_pressure_1_t p1 = read_brake_FL_1_raw_voltage();
        brake_pressure_2_t p2 = read_brake_FL_2_raw_voltage();

        if (pedals_can_send_pedal_brake_adc(&tx_header, &brake_adc) != PEDALS_OK) {
            PedalsErrorHandler();
        }
        if (pedals_can_send_pedal_accel_adc(&tx_header, &accel_adc) != PEDALS_OK) {
            PedalsErrorHandler();
        }
        if (pedals_can_send_pedals_status(&tx_header, &status) != PEDALS_OK) {
            PedalsErrorHandler();
        }
        if (pedals_can_send_brake_pressure_1_voltage(&tx_header, &p1) != PEDALS_OK) {
            PedalsErrorHandler();
        }
        if (pedals_can_send_brake_pressure_2_voltage(&tx_header, &p2) != PEDALS_OK) {
            PedalsErrorHandler();
        }
            
        printf(
            "POT pos: brake:%u accel:%u | ADC cnt: brake:%u accel:%u red_brk:%u red_acc:%u | "
            "PSI (0.1): P1:%u.%u P2:%u.%u | ADC pres: P1:%u P2:%u\n",

            // pedal positions
            (unsigned)status.BrakePedal_Main_Pos,
            (unsigned)status.AccelPedal_Main_Pos,

            // pedal ADCs
            (unsigned)brake_adc.BrakePedal_Main_ADC,
            (unsigned)accel_adc.AccelPedal_Main_ADC,
            (unsigned)brake_adc.BrakePedal_Redundant_ADC,
            (unsigned)accel_adc.AccelPedal_Redundant_ADC,

            // Brake_Pressure holds tenths-PSI for DBC ×0.1; /10 only for uart display
            (unsigned)(p1.Brake_Pressure / 10u),
            (unsigned)(p1.Brake_Pressure % 10u),
            (unsigned)(p2.Brake_Pressure / 10u),
            (unsigned)(p2.Brake_Pressure % 10u),

            // pressure ADC counts
            (unsigned)p1.Brake_Pressure_ADC,
            (unsigned)p2.Brake_Pressure_ADC
        );

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

