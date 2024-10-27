//
// Created by kana on 10/26/2024.
//

#ifndef TOY_CAR_WAYPOINT_H
#define TOY_CAR_WAYPOINT_H

typedef struct {
    float x;
    float y;
} Waypoint;

static float distanceBetweenWaypoints(Waypoint a, Waypoint b);

static float calculateAngleBetweenPoints(Waypoint a, Waypoint b);

#endif //TOY_CAR_WAYPOINT_H
