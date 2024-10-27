#include "car_driver.h"
#include "accelerometer.h"
#include "navigation_manager.h"


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

void testNavManager(void);

uint8_t initPeripherals(void) {
    I2C_Handle_t i2cHandle;
    i2cHandle.pI2Cx = I2C1;
    I2C_Init(&i2cHandle);

    uint8_t status = Accelerometer_Init();
    if (status != 0) {
        return status;
    }

    return 0;
}

uint8_t main(void) {
    if (initPeripherals() != 0) {
        return 1;
    }

    testNavManager();
//    carInit();
//    moveForward();

    while (1) {}
    return 0;
}

void testNavManager() {
    NavigationManager* manager = createNavigationManager(-1);
    uint8_t status;

    while(1) {
        status = updatePosition(manager);
        if (status != 0) {
            return;
        }
    }

}