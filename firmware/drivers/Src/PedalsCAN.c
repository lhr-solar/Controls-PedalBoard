#include "PedalsCAN.h"

QueueHandle_t can_tx_queue;
static uint8_t       can_tx_qStorage[CANTX_QUEUE_LENGTH * CANTX_ITEM_SIZE];
static StaticQueue_t xStaticQueue_can_tx;

/* ===================== COMMON HELPERS ===================== */

static inline void pack_header(CAN_TxHeaderTypeDef *h, uint32_t id, uint8_t dlc) {
    h->StdId              = id;
    h->RTR                = CAN_RTR_DATA;
    h->IDE                = CAN_ID_STD;
    h->DLC                = dlc;
    h->TransmitGlobalTime = DISABLE;
}

static inline PedalsStatus_t send_frame(CAN_TxHeaderTypeDef *h, uint8_t *data) {
    return (can_send(hcan1, h, data, CARCAN_TIMEOUT_TICKS) == CAN_OK)
               ? PEDALS_OK
               : PEDALS_CAN_SEND_FAIL;
}

/* ===================== INIT ===================== */

static PedalsStatus_t MX_CAN_Init(void) {
    can_tx_queue = xQueueCreateStatic(CANTX_QUEUE_LENGTH, CANTX_ITEM_SIZE,
                                      can_tx_qStorage, &xStaticQueue_can_tx);
    if (!can_tx_queue) return PEDALS_CAN_INIT_FAIL;

    CAN_FilterTypeDef f = {
        .FilterBank           = 0,
        .FilterMode           = CAN_FILTERMODE_IDMASK,
        .FilterScale          = CAN_FILTERSCALE_32BIT,
        /* Match PS-VoltTemp: reject all RX IDs on TX-focused node. */
        .FilterIdHigh         = 0xFFFFu,
        .FilterIdLow          = 0xFFFFu,
        .FilterMaskIdHigh     = 0xFFFFu,
        .FilterMaskIdLow      = 0xFFFFu,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterActivation     = ENABLE,
        .SlaveStartFilterBank = 14
    };

    hcan1->Instance = CAN1;
    hcan1->Init = (CAN_InitTypeDef){
        .Prescaler           = 20,
        .Mode                = CAN_MODE_NORMAL,
        .SyncJumpWidth       = CAN_SJW_1TQ,
        .TimeSeg1            = CAN_BS1_13TQ,
        .TimeSeg2            = CAN_BS2_2TQ,
        .TimeTriggeredMode   = DISABLE,
        .AutoBusOff          = ENABLE,
        .AutoWakeUp          = DISABLE,
        .AutoRetransmission  = ENABLE,
        .ReceiveFifoLocked   = DISABLE,
        .TransmitFifoPriority = ENABLE
    };

    if (can_init(hcan1, &f)  != CAN_OK) return PEDALS_CAN_INIT_FAIL;
    if (can_start(hcan1)     != CAN_OK) return PEDALS_CAN_INIT_FAIL;

    return PEDALS_OK;
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != CAN1) return;

    GPIO_InitTypeDef g = {0};

    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    g.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
    g.Mode      = GPIO_MODE_AF_PP;
    g.Pull      = GPIO_NOPULL;
    g.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    g.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &g);

    HAL_NVIC_SetPriority(CAN1_TX_IRQn,  CAN_INTERRUPT_PRIO, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, CAN_INTERRUPT_PRIO, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

PedalsStatus_t pedals_can_init(void) { return MX_CAN_Init(); }

PedalsStatus_t pedals_can_stop(void) {
    return (can_stop(hcan1) == CAN_OK) ? PEDALS_OK : PEDALS_CAN_STOP_FAIL;
}

uint32_t pedals_can_get_error(void) {
    return HAL_CAN_GetError(hcan1);
}

/* ===================== SEND FUNCTIONS ===================== */

PedalsStatus_t pedals_can_send_pedal_brake_adc(CAN_TxHeaderTypeDef *h,
                                               pedal_brake_adc_t *p) {
    pack_header(h, CAN_ID_PEDAL_BRAKE_ADC, CAN_DLC_PEDAL_BRAKE_ADC);
    uint8_t d[h->DLC];

    uint16_t main_adc = p->BrakePedal_Main_ADC & 0x0FFFu;
    uint16_t red_adc  = p->BrakePedal_Redundant_ADC & 0x0FFFu;

    d[0] = (uint8_t)(main_adc & 0xFFu);
    d[1] = (uint8_t)((main_adc >> 8) & 0x0Fu);
    d[2] = (uint8_t)(red_adc & 0xFFu);
    d[3] = (uint8_t)((red_adc >> 8) & 0x0Fu);
    d[4] = p->FrameID_Pedals;

    return send_frame(h, d);
}

PedalsStatus_t pedals_can_send_pedal_accel_adc(CAN_TxHeaderTypeDef *h,
                                               pedal_accel_adc_t *p) {
    pack_header(h, CAN_ID_PEDAL_ACCEL_ADC, CAN_DLC_PEDAL_ACCEL_ADC);
    uint8_t d[h->DLC];

    uint16_t main_adc = p->AccelPedal_Main_ADC & 0x0FFFu;
    uint16_t red_adc  = p->AccelPedal_Redundant_ADC & 0x0FFFu;

    d[0] = (uint8_t)(main_adc & 0xFFu);
    d[1] = (uint8_t)((main_adc >> 8) & 0x0Fu);
    d[2] = (uint8_t)(red_adc & 0xFFu);
    d[3] = (uint8_t)((red_adc >> 8) & 0x0Fu);
    d[4] = p->FrameID_Pedals;

    return send_frame(h, d);
}

PedalsStatus_t pedals_can_send_brake_pressure_1_voltage(CAN_TxHeaderTypeDef *h,
                                                        brake_pressure_1_t *p) {
    /*
     * DBC 0x650: bits 0-15 Brake_Pressure (LE uint16 tenths PSI, physical = raw * 0.1);
     * bits 16-27 Brake_Pressure_ADC raw 0..4095; Shift ops below split into bytes nibbles — not scaling.
     */
    pack_header(h, CAN_ID_BRAKE_PRESSURE_1, CAN_DLC_BRAKE_PRESSURE_1);
    uint8_t d[h->DLC];

    uint16_t psi = p->Brake_Pressure;
    uint16_t adc = p->Brake_Pressure_ADC & 0x0FFFu;

    d[0] = (uint8_t)(psi & 0xFFu);
    d[1] = (uint8_t)((psi >> 8) & 0xFFu);
    d[2] = (uint8_t)(adc & 0xFFu);
    d[3] = (uint8_t)((adc >> 8) & 0x0Fu);
    d[4] = p->FrameID_Pedals;

    return send_frame(h, d);
}

PedalsStatus_t pedals_can_send_brake_pressure_2_voltage(CAN_TxHeaderTypeDef *h,
                                                        brake_pressure_2_t *p) {
    /* Same packing as 0x650 — DBC 0x651. */
    pack_header(h, CAN_ID_BRAKE_PRESSURE_2, CAN_DLC_BRAKE_PRESSURE_2);
    uint8_t d[h->DLC];

    uint16_t psi = p->Brake_Pressure;
    uint16_t adc = p->Brake_Pressure_ADC & 0x0FFFu;

    d[0] = (uint8_t)(psi & 0xFFu);
    d[1] = (uint8_t)((psi >> 8) & 0xFFu);
    d[2] = (uint8_t)(adc & 0xFFu);
    d[3] = (uint8_t)((adc >> 8) & 0x0Fu);
    d[4] = p->FrameID_Pedals;

    return send_frame(h, d);
}

PedalsStatus_t pedals_can_send_pedals_status(CAN_TxHeaderTypeDef *h,
                                             pedal_status_t *p) {
    pack_header(h, CAN_ID_PEDAL_STATUS, CAN_DLC_PEDAL_STATUS);
    uint8_t d[h->DLC];

    d[0] = p->AccelPedal_Main_Pos;
    d[1] = p->AccelPedal_Redundant_Pos;
    d[2] = p->BrakePedal_Main_Pos;
    d[3] = p->BrakePedal_Redundant_Pos;
    d[4] = (uint8_t)(
        (p->AccelPedal_Main_Fault      << 0) |
        (p->AccelPedal_Redundant_Fault << 1) |
        (p->BrakePedal_Main_Fault      << 2) |
        (p->BrakePedal_Redundant_Fault << 3) |
        (p->Brake_Pressure_1_Fault     << 4) |
        (p->Brake_Pressure_2_Fault     << 5)
    );
    d[5] = p->FrameID_Pedals;

    return send_frame(h, d);
}

void pedals_print_payload(void) {
    pedal_brake_adc_t  b  = read_pedal_brake_adc();
    pedal_accel_adc_t  a  = read_pedal_accel_adc();
    brake_pressure_1_t p1 = read_brake_FL_1_raw_voltage();
    brake_pressure_2_t p2 = read_brake_FL_2_raw_voltage();

    printf("Brake ADC cnt: main=%u redundant=%u | Accel ADC cnt: main=%u redundant=%u\n\r",
           (unsigned)b.BrakePedal_Main_ADC, (unsigned)b.BrakePedal_Redundant_ADC,
           (unsigned)a.AccelPedal_Main_ADC, (unsigned)a.AccelPedal_Redundant_ADC);
    printf("Brake pressure CAN raw (0.1 PSI): P1=%u P2=%u | ADC counts: P1=%u P2=%u\n\r",
           (unsigned)p1.Brake_Pressure, (unsigned)p2.Brake_Pressure,
           (unsigned)p1.Brake_Pressure_ADC, (unsigned)p2.Brake_Pressure_ADC);
}
