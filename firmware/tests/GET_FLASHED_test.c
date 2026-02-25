#include "Pots.h"
#include "StatusLEDs.h"

int main(){
    HAL_Init();
    GPIO_Init();

    while(1){
        flashThem(500);
    }

    return 0;
}
