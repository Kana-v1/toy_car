//
// Created by kana on 10/26/2024.
//

#include <malloc.h>
#include "navigation_manager.h"

static void calcVelocity(Position* pos);

static void calcHeading(CarState* state, uint32_t currentTime);

static uint32_t tick;

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

static uint8_t readSensorData(SensorData *sensorData, float gyroBias) {
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

bool addWaypoint(NavigationManager* manager, Waypoint waypoint) {
    if (!manager) {
        return false;
    }

    if (manager->waypointsCap == manager->numWaypoints) {
        resizeWaypointsArray(manager);
    }

    manager->waypoints[manager->numWaypoints++] = waypoint;

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
        stopCar();
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

    if (fabsf(angleDif) > HEADING_THRESHOLD) {
        if (angleDif > 0) {
            rotateRight(ROTATE_SPEED_NORMAL);
            return;
        }

        if (angleDif < 0) {
            rotateLeft(ROTATE_SPEED_NORMAL);
            return;
        }
    }

    moveForward();
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

// TEST NAVIGATION MANAGER
void testNavManager() {
    NavigationManager* manager = createNavigationManager(-1);
    uint8_t status;

    while (1) {
        status = updatePosition(manager, GetTick());
        float volatile heading = manager->currentState.heading;
        if (status != 0) {
            return;
        }
    }
}