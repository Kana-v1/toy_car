#include "car_driver.h"

void fixBtnDebounce(void) {
    for (uint16_t i = 0; i < 50000; i++) {}
}

void GPIO_InterruptCallback(uint8_t extiLine) {
    fixBtnDebounce();
    switch (extiLine) {
        case TOGGLE_STATE_BTN_PIN:
            handleBtnInterrupt();
            break;

        case OBSTACLES_SCANNER_PIN:
            detourObstacle();
            break;
    }
}

uint8_t main(void) {
    carInit();
    moveForward();

    while (1) {}
    return 0;
}