#include "PedalsSensors.h"
#include "StatusLEDs.h"
#include "PedalsLUT.h"

#define NUM_ADC_CHANNELS 6

static pedal_status_t     Pedals_Status_Msg        = {0};
static pedal_brake_adc_t  Pedals_Brake_ADC_Msg     = {0};
static pedal_accel_adc_t  Pedals_Accel_ADC_Msg     = {0};
static brake_pressure_1_t Pedals_Brake_FL_Front_Msg = {0};
static brake_pressure_2_t Pedals_Brake_FL_Back_Msg  = {0};

static uint8_t Pedals_FrameID = 0;
uint32_t raw_vals[NUM_ADC_CHANNELS] = {0};

/* Queues — indexed by SensorsADCInput_t (0-5) */
static QueueHandle_t adcQueues[NUM_ADC_CHANNELS];
static uint8_t       adcQueueStorage[NUM_ADC_CHANNELS][ADC1_QUEUE_LENGTH * ADC_ITEM_SIZE];
static StaticQueue_t adcQueueBuffers[NUM_ADC_CHANNELS];

/* Maps SensorsADCInput_t index → HAL ADC channel enum */
static const PedalsADCChannel_e ch_map[NUM_ADC_CHANNELS] = {
    [ADC_INPUT_ACCEL_POT]           = ACCEL_POT_BUFF_CHANNEL,
    [ADC_INPUT_BRAKE_POT]           = BRAKE_POT_BUFF_CHANNEL,
    [ADC_INPUT_BRAKE_FL_FRONT]      = BRAKE_FL_FRONT_BUFF_CHANNEL,
    [ADC_INPUT_BRAKE_FL_BACK]       = BRAKE_FL_BACK_BUFF_CHANNEL,
    [ADC_INPUT_ACCEL_POT_REDUNDANT] = ACCEL_POT_REDUNDANT_BUFF_CHANNEL,
    [ADC_INPUT_BRAKE_POT_REDUNDANT] = BRAKE_POT_REDUNDANT_BUFF_CHANNEL,
};

/* ---------- Helpers ---------- */

/* DBC Brake_Pressure_*_ADC: counts 0..4095 scale=1 — use bottom 12b of DR (right-aligned). */
static inline uint16_t adc_raw_counts_12b(uint32_t dr) {
    return (uint16_t)(dr & 0xFFFu);
}

/* ---------- ADC ---------- */

adc_status_t sensors_adc_start_read(PedalsADCChannel_e channel) {
    SensorsADCInput_t idx;
    switch (channel) {
        case BRAKE_POT_BUFF_CHANNEL:           idx = ADC_INPUT_BRAKE_POT;           break;
        case ACCEL_POT_BUFF_CHANNEL:           idx = ADC_INPUT_ACCEL_POT;           break;
        case BRAKE_FL_FRONT_BUFF_CHANNEL:      idx = ADC_INPUT_BRAKE_FL_FRONT;      break;
        case BRAKE_FL_BACK_BUFF_CHANNEL:       idx = ADC_INPUT_BRAKE_FL_BACK;       break;
        case ACCEL_POT_REDUNDANT_BUFF_CHANNEL: idx = ADC_INPUT_ACCEL_POT_REDUNDANT; break;
        case BRAKE_POT_REDUNDANT_BUFF_CHANNEL: idx = ADC_INPUT_BRAKE_POT_REDUNDANT; break;
        default: return ADC_CHANNEL_CONFIG_FAIL;
    }

    if (!adcQueues[idx]) return ADC_INIT_FAIL;

    ADC_ChannelConfTypeDef cfg = {
        .Channel      = channel,
        .SamplingTime = PEDALS_READ_ADC_SAMPLING_TIME,
        .SingleDiff   = ADC_SINGLE_ENDED,
        .OffsetNumber = ADC_OFFSET_NONE,
        .Offset       = 0
    };

    return adc_read(hadc1, &cfg, adcQueues[idx]);
}

adc_status_t sensors_adc_init(void) {
    ADC_InitTypeDef init = {0};
    init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
    init.Resolution            = ADC_RESOLUTION_12B;
    init.DataAlign             = ADC_DATAALIGN_RIGHT;
    init.ScanConvMode          = ADC_SCAN_DISABLE;
    init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    init.LowPowerAutoWait      = DISABLE;
    init.ContinuousConvMode    = DISABLE;
    init.NbrOfConversion       = 1;
    init.DiscontinuousConvMode = DISABLE;
    init.ExternalTrigConv      = ADC_SOFTWARE_START;
    init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    init.DMAContinuousRequests = DISABLE;
    init.Overrun               = ADC_OVR_DATA_PRESERVED;
    init.OversamplingMode      = DISABLE;

    for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
        adcQueues[i] = xQueueCreateStatic(
            ADC1_QUEUE_LENGTH, ADC_ITEM_SIZE,
            adcQueueStorage[i], &adcQueueBuffers[i]
        );
        if (!adcQueues[i]) return ADC_INIT_FAIL;
    }

    return (adc_init(&init, hadc1) == ADC_OK) ? ADC_OK : ADC_INIT_FAIL;
}

void sensors_adc_GPIO_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct    = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    if (hadc1->Instance == ADC1) {
        PeriphClkInit.PeriphClockSelection        = RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection           = RCC_ADCCLKSOURCE_PLLSAI1;
        /* HSE 8 MHz×N/M = HSI 16 MHz×N/M → N=16→8 for same PLLSAI1 VCO vs old HSE ADC path */
        PeriphClkInit.PLLSAI1.PLLSAI1Source       = RCC_PLLSOURCE_HSI;
        PeriphClkInit.PLLSAI1.PLLSAI1M            = 1;
        PeriphClkInit.PLLSAI1.PLLSAI1N            = 8;
        PeriphClkInit.PLLSAI1.PLLSAI1P            = RCC_PLLP_DIV7;
        PeriphClkInit.PLLSAI1.PLLSAI1Q            = RCC_PLLQ_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1R            = RCC_PLLR_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1ClockOut     = RCC_PLLSAI1_ADC1CLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = BRAKE_POT_PIN | ACCEL_POT_PIN |
                              BRAKE_FL_FRONT_PIN | BRAKE_FL_BACK_PIN |
                              ACCEL_POT_REDUNDANT_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(BRAKE_POT_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin  = BRAKE_POT_REDUNDANT_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(BRAKE_POT_REDUNDANT_PORT, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(ADC1_IRQn, PEDALS_ADC1_PRIO, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
    }
}

PedalsStatus_t sensors_adc_receive(SensorsADCInput_t ch, uint32_t *val) {
    if (ch >= ADC_INPUT_COUNT || !adcQueues[ch]) return PEDALS_OK;

    if (xQueueReceive(adcQueues[ch], val,
            pdMS_TO_TICKS(PEDALS_RECEIVE_ADC_SAMPLING_MS)) == pdPASS) {
        raw_vals[ch] = *val;
        return PEDALS_OK;
    }

    if (ENABLE_DEBUG)
        printf("Failed to receive ADC[%d] from queue\n\r", ch);
    return PEDALS_OK;
}

/* ---------- Main ---------- */

void readAllADCs(void) {
    if (ENABLE_DEBUG) printf("\033[2J");

    uint32_t vals[NUM_ADC_CHANNELS] = {0};
    for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
        sensors_adc_start_read(ch_map[i]);
        sensors_adc_receive(i, &vals[i]);
    }

    uint16_t adc[NUM_ADC_CHANNELS];
    for (int i = 0; i < NUM_ADC_CHANNELS; i++)
        adc[i] = adc_raw_counts_12b(vals[i]);

    /* Pedal_Brake_ADC (0x753): main + redundant — raw DR counts only, no scaling */
    Pedals_Brake_ADC_Msg.BrakePedal_Main_ADC = adc[ADC_INPUT_BRAKE_POT];
    Pedals_Brake_ADC_Msg.BrakePedal_Redundant_ADC =
        adc[ADC_INPUT_BRAKE_POT_REDUNDANT];

    /* Pedal_Accel_ADC (0x754): main + redundant raw counts */
    Pedals_Accel_ADC_Msg.AccelPedal_Main_ADC      = adc[ADC_INPUT_ACCEL_POT];
    Pedals_Accel_ADC_Msg.AccelPedal_Redundant_ADC = adc[ADC_INPUT_ACCEL_POT_REDUNDANT];

    /*
     * 0x650/0x651: Brake_Pressure = DBC scale 0.1 -> uint16 tenths-of-PSI on bus (decoded PSI = field/10).
     * Brake_Pressure_ADC = raw 12-bit count (LUT / packing use same adc[] values, no divide).
     */
    Pedals_Brake_FL_Front_Msg.Brake_Pressure =
        brakePressurePsiTenthsLUT[adc[ADC_INPUT_BRAKE_FL_FRONT]];
    Pedals_Brake_FL_Front_Msg.Brake_Pressure_ADC = adc[ADC_INPUT_BRAKE_FL_FRONT];
    Pedals_Brake_FL_Back_Msg.Brake_Pressure =
        brakePressurePsiTenthsLUT[adc[ADC_INPUT_BRAKE_FL_BACK]];
    Pedals_Brake_FL_Back_Msg.Brake_Pressure_ADC  = adc[ADC_INPUT_BRAKE_FL_BACK];

    /* Pedal status message — redundant uses inverted LUT (sensor wired backwards) */
    Pedals_Status_Msg.BrakePedal_Main_Pos      = adcPercentBrakeMainLUT[adc[ADC_INPUT_BRAKE_POT]];
    Pedals_Status_Msg.BrakePedal_Redundant_Pos = adcPercentBrakeRedundantLUT[adc[ADC_INPUT_BRAKE_POT_REDUNDANT]];
    Pedals_Status_Msg.AccelPedal_Main_Pos      = adcPercentAccelMainLUT[adc[ADC_INPUT_ACCEL_POT]];
    Pedals_Status_Msg.AccelPedal_Redundant_Pos = adcPercentAccelRedundantLUT[adc[ADC_INPUT_ACCEL_POT_REDUNDANT]];

    /* Frame counter */
    Pedals_FrameID = (Pedals_FrameID + 1) % 255;
    Pedals_Status_Msg.FrameID_Pedals         = Pedals_FrameID;
    Pedals_Brake_ADC_Msg.FrameID_Pedals      = Pedals_FrameID;
    Pedals_Accel_ADC_Msg.FrameID_Pedals      = Pedals_FrameID;
    Pedals_Brake_FL_Front_Msg.FrameID_Pedals = Pedals_FrameID;
    Pedals_Brake_FL_Back_Msg.FrameID_Pedals  = Pedals_FrameID;

    /* LEDs */
    led_set(BRAKE_POT_LED_PORT, BRAKE_POT_LED_PIN,
        adcPercentBrakeMainLUT[adc[ADC_INPUT_BRAKE_POT]] > 50 ? LED_ON : LED_OFF);
    led_set(ACCEL_POT_LED_PORT, ACCEL_POT_LED_PIN,
        adcPercentAccelMainLUT[adc[ADC_INPUT_ACCEL_POT]] > 50 ? LED_ON : LED_OFF);
}

pedal_brake_adc_t  read_pedal_brake_adc(void)        { return Pedals_Brake_ADC_Msg; }
pedal_accel_adc_t  read_pedal_accel_adc(void)        { return Pedals_Accel_ADC_Msg; }
brake_pressure_1_t read_brake_FL_1_raw_voltage(void)  { return Pedals_Brake_FL_Front_Msg; }
brake_pressure_2_t read_brake_FL_2_raw_voltage(void)  { return Pedals_Brake_FL_Back_Msg; }

pedal_status_t read_main_positions_and_faults(void) {
    return Pedals_Status_Msg;
}

void ADC_Error_Handler(void) {
    if (ENABLE_DEBUG) printf("Error Handler: ADC initialization failed\n\r");
    while (1) {
        flashThem();
        vTaskDelay(ERROR_HANDLER_DELAY);
    }
}
