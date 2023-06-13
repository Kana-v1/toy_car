#include "car_api_general.h"


uint8_t main(void) {
    carInit();
        uint8_t buf = 0;
    while(1) {
        buf = isObstacleAhead();

        if (buf == 1) {} else{}
    }
    return 0;
}