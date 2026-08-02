#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

int main(){
    HAL_Init();
    Status_LEDs_Init();

    while(1){
        led_toggle(BRAKE_FL_BACK_PORT, BRAKE_FL_BACK_PIN);
        HAL_Delay(500);
    }

    return 0;
}
