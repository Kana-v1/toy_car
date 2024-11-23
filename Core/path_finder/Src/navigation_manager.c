//
// Created by kana on 10/26/2024.
//

#include <malloc.h>
#include "navigation_manager.h"

static uint32_t tick;

const float realDistanceTowardsObstacle = 15.0f; // 15cm

// Ensures angle is in the range 0-360 degrees
static float normalizeAngle(float angle) {
    while (angle > 360.0f) angle -= 360.0f;
    while (angle < 0.0f) angle += 360.0f;
    return angle;
}

// Calculates angle difference between alpha and beta heading
// Returns value in range -180 to 180 degrees
static float getAngleDifference(float alpha, float beta) {
    float diff = normalizeAngle(beta - alpha);
    if (diff > 180.0f) {
        diff -= 360.0f;
    }

    return diff;
}

static uint8_t readSensorData(SensorData* sensorData, float gyroBias) {
    float accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;

    if (Accelerometer_GetAcceleration(&accel_x, &accel_y, &accel_z) != 0) {
        return 1;
    }

    Gyroscope_ReadCookedData(&gyro_x, &gyro_y, &gyro_z, gyroBias);

    sensorData->gyro_x = gyro_x;
    sensorData->gyro_y = gyro_y;
    sensorData->gyro_z = gyro_z;

    sensorData->accel_x = accel_x;
    sensorData->accel_y = accel_y;
    sensorData->accel_z = accel_z;

    return 0;
}

NavigationManager* createNavigationManager(uint8_t waypointsCap) {
    if (waypointsCap < 1) {
        waypointsCap = INITIAL_WAYPOINTS_CAP;
    }

    NavigationManager* manager = (NavigationManager*) malloc(sizeof(NavigationManager));
    if (!manager) {
        return NULL;
    }

    manager->waypoints = (Waypoint*) malloc(waypointsCap * sizeof(Waypoint));
    if (!manager->waypoints) {
        free(manager);
        return NULL;
    }

    manager->numWaypoints = 0;
    manager->currentWaypoint = 0;
    manager->waypointsCap = waypointsCap;
    manager->isNavigating = false;
    manager->obstacleDetected = false;
    manager->currentState.position = (Position) {0};
    manager->currentState.heading = 0.0f;
    manager->currentState.lastUpdateAt = GetTick();
    manager->currentState.headingBias = Gyroscope_Calibrate(100);

    SensorData sensorData = {0};
    readSensorData(&sensorData, manager->currentState.headingBias);
    manager->pathCalc = InitPathCalc(sensorData);

    return manager;
}

void destroyNavigationManager(NavigationManager* manager) {
    if (!manager) {
        return;
    }

    if (manager->waypoints) {
        free(manager->waypoints);
    }

    if (manager->pathCalc) {
        free(manager->pathCalc);
    }

    free(manager);
}

static bool resizeWaypointsArray(NavigationManager* manager) {
    uint8_t newCapacity = manager->waypointsCap * 2;
    Waypoint* newArr = (Waypoint*) realloc(manager->waypoints, newCapacity * sizeof(Waypoint));

    if (!newArr) {
        return false;
    }

    manager->waypoints = newArr;
    manager->waypointsCap = newCapacity;

    return true;
}

static Waypoint convertWaypointToSensorCoordinates(CalibrationValues cal, Waypoint waypoint) {
    Waypoint sensorPoint;

    sensorPoint.x = (waypoint.x / cal.xScale) + cal.xZero;
    sensorPoint.y = (waypoint.y / cal.xScale) + cal.yZero;

    return sensorPoint;
}

bool addWaypoint(NavigationManager* manager, Waypoint waypoint) {
    if (!manager) {
        return false;
    }

    if (manager->waypointsCap == manager->numWaypoints) {
        resizeWaypointsArray(manager);
    }

    Waypoint sensorWaypoint = convertWaypointToSensorCoordinates(manager->calibrationValues, waypoint);
    manager->waypoints[manager->numWaypoints++] = sensorWaypoint;

    return true;
}

bool removeWaypoint(NavigationManager* manager, uint8_t index) {
    if (!manager || index < 0 || index > manager->numWaypoints - 1) {
        return false;
    }

    for (uint8_t i = index; i < manager->numWaypoints - 2; i++) {
        manager->waypoints[i] = manager->waypoints[i + 1];
    }

    return true;
}

void clearWayPoints(NavigationManager* manager) {
    if (!manager) {
        return;
    }

    manager->numWaypoints = 0;
    manager->currentWaypoint = 0;
    manager->isNavigating = false;

    if (!manager->waypoints) {
        return;
    }

    free(manager->waypoints);
    manager->waypoints = NULL;
    manager->waypointsCap = 0;
}

void startNavigation(NavigationManager* manager) {
    if (manager && manager->waypoints) {
        manager->isNavigating = true;
        manager->currentWaypoint = 0;
    }
}

void stopNavigation(NavigationManager* manager) {
    if (manager && manager->isNavigating) {
        manager->isNavigating = false;
        carStop();
    }
}

void updateNavigation(NavigationManager* manager) {
    if (!manager || !manager->isNavigating || manager->currentWaypoint >= manager->numWaypoints) {
        return;
    }

    if (updatePosition(manager, GetTick()) != 0) {
        return;
    }

    Waypoint target = manager->waypoints[manager->currentWaypoint];
    Waypoint current = {
            .x =  manager->currentState.position.x,
            .y = manager->currentState.position.x
    };

    float distance = distanceBetweenWaypoints(current, target);
    if (distance < POSITION_THRESHOLD) {
        manager->currentWaypoint++;

        if (manager->currentWaypoint == manager->numWaypoints) {
            stopNavigation(manager);
            clearWayPoints(manager);

            return;
        }
    }

    float targetAngle = calculateAngleBetweenPoints(current, target);
    float angleDif = getAngleDifference(manager->currentState.heading, targetAngle);

    if (!carMightChangeDirection()) {
        return;
    }

    if (fabsf(angleDif) > HEADING_THRESHOLD) {
        if (angleDif > 0) {
            carRotateRight(ROTATE_SPEED_NORMAL);
            return;
        }

        if (angleDif < 0) {
            carRotateLeft(ROTATE_SPEED_NORMAL);
            return;
        }
    }

    carMoveForward();
}

uint8_t updatePosition(NavigationManager* manager, uint32_t currentTime) {
    double dt = fabs((currentTime - manager->currentState.lastUpdateAt) / (double) 1000.0f); // convert ms to s
    SensorData sensorData = {0};
    readSensorData(&sensorData, manager->currentState.headingBias);

    CalcPosition(manager->pathCalc, sensorData, dt);

    return 0;
}

bool isNavigationComplete(NavigationManager* manager) {
    return !manager || !manager->isNavigating;
}

void moveWithNavigation(NavigationManager* manager) {
    uint32_t lastUpdateTime = GetTick();
    const uint32_t UPDATE_INTERVAL_MS = 50;
    startNavigation(manager);

    while (!isNavigationComplete(manager)) {
        uint32_t currentTime = GetTick();

        if (currentTime - lastUpdateTime < UPDATE_INTERVAL_MS) {
            continue;
        }

        updateNavigation(manager);
    }
}

static void moveForOneSec(NavigationManager* manager, bool forward) {
    uint8_t status = 0;
    uint32_t lastUpdateTime = GetTick();
    const uint32_t UPDATE_INTERVAL_MS = 50;
    const uint32_t MOVE_TIME_MS = 10000;

    uint32_t initTime = GetTick();
    if (forward) {
        carMoveForward();
    } else {
        carMoveBack();
    }

    while (true) {
        uint32_t currentTime = GetTick();
        if (currentTime - initTime >= MOVE_TIME_MS) {
            carStop();
            return;
        }

        if (currentTime - lastUpdateTime < UPDATE_INTERVAL_MS) {
            continue;
        }

        status = updatePosition(manager, currentTime);

        if (status != 0) {
            carStop();
            return;
        }
        lastUpdateTime = currentTime;
    }
}

void calibrateNavigationManager(NavigationManager* manager) {
    SensorData sensorData = {0};
    const float KNOWN_DISTANCE = 5.0f;

    struct {
        float minX, maxX;
        float minY, maxY;
        float sumX, sumY;
    } cal = {0};

    int samplesNum = 3;
    for (int i = 0; i < samplesNum; i++) {
        // Forward movement to get Y axis calibration
        moveForOneSec(manager, true);
        cal.maxY = fmaxf(cal.maxY, manager->pathCalc->positionData.pos_y);
        cal.minY = fminf(cal.minY, manager->pathCalc->positionData.pos_y);
        ResetPathCalc(manager->pathCalc, sensorData);

        HAL_Delay(10000);

        // Backward movement
        moveForOneSec(manager, false);
        cal.maxY = fmaxf(cal.maxY, manager->pathCalc->positionData.pos_y);
        cal.minY = fminf(cal.minY, manager->pathCalc->positionData.pos_y);
        ResetPathCalc(manager->pathCalc, sensorData);
    }

    // calculate scaling factors
    float yRange = cal.maxY - cal.minY;
    // convert to real units. Divide by 2 since we're trying to find a "center"
    float yZero = (cal.maxY + cal.minY) / 2.0f;

    float xRange = cal.maxX - cal.minX;
    float xZero = (cal.maxX + cal.minX) / 2.0f;

    manager->calibrationValues.yScale = KNOWN_DISTANCE / (yRange / 2.0f);
    manager->calibrationValues.yZero = yZero;

    manager->calibrationValues.xScale = KNOWN_DISTANCE / (xRange / 2.0f);;
    manager->calibrationValues.xZero = xZero;

    manager->calibrationValues.distanceTowardsObstacle = manager->calibrationValues.yZero * realDistanceTowardsObstacle;
}