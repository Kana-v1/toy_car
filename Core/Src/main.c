#include "car_driver.h"

void fixBtnDebounce(void) {
    for (uint16_t i = 0; i < 50000; i++) {}
}

void GPIO_InterruptCallback(uint8_t extiLine) {
    fixBtnDebounce();
    if (extiLine == TOGGLE_STATE_BTN_PIN) {
        handleBtnInterrupt();
    }
}

uint8_t main(void) {
    carInit();

    while (1) {
        while (isObstacleAhead() == OBSTACLE_NOT_DEFINED) {
            moveForward();
        }

        for (uint16_t i = 0; i < 200000; i++) {
            moveBack();
        }

        int whereToTurn = rand() % 2;

        for (uint16_t i = 0; i < 20000; i++) {
            if (whereToTurn == TURN_LEFT) {
                rotateLeft(ROTATE_SPEED_NORMAL);
            } else {
                rotateRight(ROTATE_SPEED_NORMAL);
            }
        }

    }
    return 0;
}