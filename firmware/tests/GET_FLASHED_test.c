#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

/*
*
* WARNING: THIS TEST MIGHT CAUSE INTENSE EPILEPSEY FOR CERTAIN INDIVIDUALS
* ALL LEDs flash at DELAY ms period
*
* IMPORTANT - make sure GET_FLASHED uses HAL_Delay and not vTaskDelay for this test
*
*/

#define DELAY 500

int main(){
    HAL_Init();
	SystemClock_Config();
    Status_LEDs_Init();

    while(1){
		toggle_LED(PSOM_HB_PORT, PSOM_HB_PIN);
        flashThem();
		HAL_Delay(DELAY);
    }

    return 0;
}
