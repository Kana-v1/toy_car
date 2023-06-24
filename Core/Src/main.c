#include "car_driver.h"


uint8_t main(void) {
    carInit();
//    rotateLeft(ROTATE_SPEED_NORMAL);
//    moveForward();
    moveBack();
    while (1);

    uint8_t buf = 0;
    while (1) {
        buf = isObstacleAhead();

        if (buf == 1) {} else {}
    }
    return 0;
}