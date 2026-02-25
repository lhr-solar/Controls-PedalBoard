/* Override weak _write from Embedded-Sharepoint to use blocking HAL transmit
 * This avoids modifying Embedded-Sharepoint sources while fixing console output
 */
#include "stm32l4xx_hal.h"
#include "Debugging.h"

int _write(int file, char *ptr, int len) {
    (void)file;
    if (husart1 == NULL) return -1;
    if (HAL_UART_Transmit(husart1, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY) != HAL_OK) {
        return -1;
    }
    return len;
}
