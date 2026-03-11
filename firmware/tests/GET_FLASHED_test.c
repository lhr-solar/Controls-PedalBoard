#include "Pedals_Sensors.h"
#include "StatusLEDs.h"

//WARNING: THIS TEST MIGHT CAUSE INTENSE EPILEPSEY FOR CERTAIN INDIVIDUALS
//ALL LEDs flash at 500ms period

int main(){
    HAL_Init();
    Status_LEDs_Init();

    while(1){
        flashThem(500);
    }

    return 0;
}
