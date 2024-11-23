//
// Created by kana on 11/3/2024.
//

#ifndef TOY_CAR_CALC_H
#define TOY_CAR_CALC_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "waypoint.h"

#define ALPHA 0.98  // Complementary filter coefficient
#define FILTER_ALPHA 0.5  // Low-pass filter coefficient, adjustable

typedef struct {
    float pos_x, pos_y;
    float vel_x, vel_y;
    float heading;
} PositionData;

typedef struct {
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;
} SensorData;

typedef struct {
    float angle_x, angle_y;  // Filtered angles
} FilteredAngles;

typedef struct {
    PositionData positionData;
    FilteredAngles* filteredAngles;
    SensorData* filterAccel; // Filtered accelerometer readings
} PathCalc;

typedef struct {
    float x;
    float y;
} Vector;


PathCalc* InitPathCalc(SensorData sensorData);

void ResetPathCalc(PathCalc* pathCalc, SensorData sensorData);

void CalcPosition(PathCalc* pathCalc, SensorData sensorData, double dt);

Vector CalcVector(PositionData position, Waypoint goal, float distanceToObstacle);

#endif //TOY_CAR_CALC_H
