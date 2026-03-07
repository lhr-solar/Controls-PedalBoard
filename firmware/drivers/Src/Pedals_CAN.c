#include "Pedals.h"
#include "Pedals_CAN.h"

/*    
    -------------- CAN status types --------------

    CAN_ERR,   // unsuccessful operation
    CAN_OK,
    CAN_SENT,  // successful send
    CAN_RECV,  // successful recieve
    CAN_EMPTY, // recieved nothing with no errors

    --------------  --------------  --------------
*/

// can_status_t pedals_CAN_init() {
//     return CAN_OK;
// }