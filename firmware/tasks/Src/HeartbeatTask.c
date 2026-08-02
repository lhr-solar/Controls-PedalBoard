#include "HeartbeatTask.h"
#include "InitTask.h"
#include "StatusLEDs.h"
#include "Debugging.h"


void Task_Heartbeat(void *args __attribute__((unused))) {
    while (true) {
        led_toggle(PSOM_HB_PORT, PSOM_HB_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
