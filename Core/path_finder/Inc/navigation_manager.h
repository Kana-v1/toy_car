//
// Created by kana on 10/26/2024.
//

#ifndef TOY_CAR_NAVIGATION_MANAGER_H
#define TOY_CAR_NAVIGATION_MANAGER_H

#include "waypoint.h"
#include "car_driver.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "systick.h"

#define HEADING_THRESHOLD 5    // Degrees of acceptable heading error
#define INITIAL_WAYPOINTS_CAP 5
#define POSITION_THRESHOLD 5

typedef struct {
    float x, y;           // position
    float vx, vy;         // velocity
    float ax, ay;         // current acceleration
    float prev_ax, prev_ay; // previous acceleration m/s²
    uint32_t stationary_count; // counter for stationary detection
} Position;

typedef struct {
    Position position;
    float heading;
    float headingBias;
    uint32_t lastUpdateAt;
} CarState;

typedef struct {
    Waypoint* waypoints;
    uint8_t numWaypoints;
    uint8_t waypointsCap;
    uint8_t currentWaypoint;
    CarState currentState;
    bool isNavigating;
    bool obstacleDetected;
} NavigationManager;

NavigationManager* createNavigationManager(uint8_t waypointsCap);

void destroyNavigationManager(NavigationManager* manager);

bool addWaypoint(NavigationManager* manager, Waypoint waypoint);

void startNavigation(NavigationManager* manager);

void stopNavigation(NavigationManager* manager);

void updateNavigation(NavigationManager* manager);

uint8_t updatePosition(NavigationManager* manager);

bool isNavigationComplete(NavigationManager* manager);

void testNavManager(void);

#endif //TOY_CAR_NAVIGATION_MANAGER_H
