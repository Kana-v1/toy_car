#include "car_driver.h"
#include "accelerometer.h"
#include "navigation_manager.h"
#include "gyroscope.h"
#include "spi.h"
#include "systick.h"

bool isFirstTime = true;

NavigationManager* manager;

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
            if (isFirstTime) {
                isFirstTime = false;
                return;
            }

            if (!manager->isNavigating) {
                return;
            }

            if (manager->navigationLocked) {
                return;
            }

            disableNavigation(manager);

            Vector vector = CalcVector(
                    manager->pathCalc->positionData,
                    manager->waypoints[manager->numWaypoints],
                    manager->calibrationValues.distanceTowardsObstacle);

            float targetAngle = atan2f(vector.y, vector.x);

            carStop();
            carRotate(targetAngle);
            carMoveForward();

            // move forward for some time, so we're far enough from the obstacle
            HAL_Delay(10000);

            enableNavigation(manager);

        default:
            return;

    }
}

uint8_t initPeripherals(void) {
    uint8_t status;

    SysTick_Init();

    I2C_Handle_t i2cHandle;
    i2cHandle.pI2Cx = I2C1;
    I2C_Init(&i2cHandle);

    status = Accelerometer_Init();
    if (status != 0) {
        return 1;
    }

    SPI_Handle_t hspi;
    hspi.pSPIx = SPI1;
    status = SPI_Init(&hspi);
    if (status != 0) {
        return 2;
    }

    status = Gyroscope_Init();
    if (status != 0) {
        return 3;
    }

    return 0;
}

uint8_t main(void) {
    if (initPeripherals() != 0) {
        return 1;
    }
    carInit();
    manager = createNavigationManager(2);
    calibrateNavigationManager(manager);
    Waypoint pointA = {
            .x =  0,
            .y =  0,
    };
    Waypoint pointB = {
            .x =  100,
            .y =  100,
    };
    addWaypoint(manager, pointA);
    addWaypoint(manager, pointB);

    moveWithNavigation(manager);

    while (1) {
    }
}
