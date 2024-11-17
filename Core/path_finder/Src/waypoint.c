//
// Created by kana on 10/26/2024.
//
#include "waypoint.h"
#include <math.h>

float distanceBetweenWaypoints(Waypoint a, Waypoint b) {
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;

    return sqrtf(powf(deltaX, 2) + powf(deltaY, 2));
}

float calculateAngleBetweenPoints(Waypoint a, Waypoint b) {
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;

    return atan2f(deltaY, deltaX) * 180.0f / M_PI;
}
