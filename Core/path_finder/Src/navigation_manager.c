//
// Created by kana on 10/26/2024.
//

#include <malloc.h>
#include "navigation_manager.h"

const float dt = 1.0f;  // 1ms sample time

static void calcVelocity(Position* pos);

static void calcHeading(CarState* state);

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

    return manager;
}

void destroyNavigationManager(NavigationManager* manager) {
    if (manager == NULL) {
        return;
    }

    if (manager->waypoints != NULL) {
        free(manager->waypoints);
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

    if (updatePosition(manager) != 0) {
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

uint8_t updatePosition(NavigationManager* manager) {
    float ax, ay, az;
    const float noise_threshold = 0.1f;  // m/s²

    if (Accelerometer_GetAcceleration(&ax, &ay, &az) != 0) {
        return 1;
    }

    if (ax < noise_threshold && ax > -noise_threshold) {
        ax = 0;
    }

    if (ay < noise_threshold && ay > -noise_threshold) {
        ay = 0;
    }

    Position pos = manager->currentState.position;

    pos.prev_ax = pos.ax;
    pos.prev_ay = pos.ay;

    pos.ax = ax;
    pos.ay = ay;

    calcVelocity(&pos);
    calcHeading(&manager->currentState);

    pos.x += pos.vx * dt;
    pos.y += pos.vy * dt;

    manager->currentState.position = pos;

    return 0;
}

// calculating velocity just with accelerometer is quite tough
static void calcVelocity(Position* pos) {
    const float ACCEL_THRESHOLD = 0.5f;    // m/s²
    const float VELOCITY_THRESHOLD = 0.1f;   // m/s
    const uint32_t STATIONARY_TIME = 50;    // samples

    // calculate total acceleration magnitude
    float accel_magnitude = sqrtf(powf(pos->ax, 2) + powf(pos->ay, 2));

    if (accel_magnitude > ACCEL_THRESHOLD) {
        // We're moving
        pos->stationary_count = 0;

        // Update velocities
        pos->vx = pos->ax * dt;
        pos->vy = pos->ay * dt;
    }

    pos->stationary_count++;
    if (pos->stationary_count > STATIONARY_TIME) {
        // We've been "stationary" for a while
        // Gradually reduce velocity
        pos->vx *= 0.95f;
        pos->vy *= 0.95f;
    }

    //If velocity is very small, zero it
    if (fabs(pos->vx) < VELOCITY_THRESHOLD && fabs(pos->vy) < VELOCITY_THRESHOLD) {
        pos->vx = 0;
        pos->vy = 0;
    }
}

static void calcHeading(CarState* state) {
    state->heading = 0;
}

bool isNavigationComplete(NavigationManager* manager) {
    return !manager || !manager->isNavigating;
}