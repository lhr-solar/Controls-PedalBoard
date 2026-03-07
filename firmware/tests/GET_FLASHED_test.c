#include "Pots.h"
#include "StatusLEDs.h"

int main(){
    HAL_Init();
    Status_LEDs_Init();

    while(1){
        flashThem(500);
    }

    return 0;
}
