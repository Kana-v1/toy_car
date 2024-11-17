//
// Created by kana on 11/3/2024.
//

#include "calc.h"

#define VELOCITY_THRESHOLD 0.05
#define VELOCITY_DECAY 0.95
#define FILTER_THRESHOLD 0.05

PathCalc* InitPathCalc(SensorData sensorData) {
    PathCalc* pathCalc = (PathCalc*) malloc(sizeof(PathCalc));
    ResetPathCalc(pathCalc, sensorData);

    return pathCalc;
}

void ResetPathCalc(PathCalc* pathCalc, SensorData sensorData) {
    pathCalc->filteredAngles = (FilteredAngles*) malloc(sizeof(FilteredAngles));
    pathCalc->filterAccel = (SensorData*) malloc(sizeof(SensorData));

    // init complementary filter
    pathCalc->filteredAngles->angle_x = atan2(sensorData.accel_y, sensorData.accel_z) * 180 / M_PI;
    pathCalc->filteredAngles->angle_y = atan2(-sensorData.accel_x, sqrt(pow(sensorData.accel_y, 2) + pow(sensorData.accel_z, 2))) * 180 / M_PI;

    pathCalc->positionData.pos_x = 0;
    pathCalc->positionData.pos_y = 0;
    pathCalc->positionData.vel_x = 0;
    pathCalc->positionData.vel_y = 0;
    pathCalc->positionData.heading = 0;

    pathCalc->filterAccel->accel_x = 0;
    pathCalc->filterAccel->accel_y = 0;
    pathCalc->filterAccel->accel_z = 0;

    pathCalc->filterAccel->gyro_x = 0;
    pathCalc->filterAccel->gyro_y = 0;
    pathCalc->filterAccel->gyro_z = 0;
}

static void complementaryFilter(SensorData sensorData, FilteredAngles* filteredAngles, double dt) {
    // calc angles from accelerometer
    float accelAngleX = atan2(sensorData.accel_y, sensorData.accel_z) * 180 / M_PI;
    float accelAngleY = atan2(-sensorData.accel_x, sqrt(pow(sensorData.accel_y, 2) + pow(sensorData.accel_z, 2))) * 180 / M_PI;

    // Integrate the gyroscope data to estimate angles
    filteredAngles->angle_x = ALPHA * (filteredAngles->angle_x + sensorData.gyro_x * dt) + (1 - ALPHA) * accelAngleX;
    filteredAngles->angle_y = ALPHA * (filteredAngles->angle_y + sensorData.gyro_y * dt) + (1 - ALPHA) * accelAngleY;
}

static void lowPassFilter(SensorData sensorData, SensorData* filterAccel) {
    filterAccel->accel_x = FILTER_ALPHA * filterAccel->accel_x + (1 - FILTER_ALPHA) * sensorData.accel_x;
    filterAccel->accel_y = FILTER_ALPHA * filterAccel->accel_y + (1 - FILTER_ALPHA) * sensorData.accel_y;
    filterAccel->accel_z = FILTER_ALPHA * filterAccel->accel_z + (1 - FILTER_ALPHA) * sensorData.accel_z;

    filterAccel->gyro_x = FILTER_ALPHA * filterAccel->gyro_x + (1 - FILTER_ALPHA) * sensorData.gyro_x;
    filterAccel->gyro_y = FILTER_ALPHA * filterAccel->gyro_y + (1 - FILTER_ALPHA) * sensorData.gyro_y;
    filterAccel->gyro_z = FILTER_ALPHA * filterAccel->gyro_z + (1 - FILTER_ALPHA) * sensorData.gyro_z;

    if (fabs(filterAccel->accel_x) < FILTER_THRESHOLD) filterAccel->accel_x = 0;
    if (fabs(filterAccel->accel_y) < FILTER_THRESHOLD) filterAccel->accel_y = 0;
    if (fabs(filterAccel->accel_z) < FILTER_THRESHOLD) filterAccel->accel_z = 0;


    if (fabs(filterAccel->gyro_x) < FILTER_THRESHOLD) filterAccel->gyro_x = 0;
    if (fabs(filterAccel->gyro_y) < FILTER_THRESHOLD) filterAccel->gyro_y = 0;
    if (fabs(filterAccel->gyro_z) < FILTER_THRESHOLD) filterAccel->gyro_z = 0;
}

static void calcPosition(SensorData* filteredAccel, FilteredAngles* angles, PositionData* position, double dt) {
    position->heading += filteredAccel->gyro_z * dt;

    // Normalize heading to 0-360 degrees
    while (position->heading >= 360.0f) position->heading -= 360.0f;
    while (position->heading < 0.0f) position->heading += 360.0f;

    // Convert heading to radians for trigonometry
    float heading_rad = position->heading * M_PI / 180.0f;

    // Project accelerations to world coordinates using heading
    float cos_heading = cosf(heading_rad);
    float sin_heading = sinf(heading_rad);

    // Project the filtered acceleration onto the X and Y plane based on orientation
    float accel_x_world = filteredAccel->accel_x * cos_heading - filteredAccel->accel_y * sin_heading;
    float accel_y_world = filteredAccel->accel_x * sin_heading + filteredAccel->accel_y * cos_heading;

    // Integrate acceleration to get velocity
    position->vel_x += accel_x_world * dt * VELOCITY_DECAY / 100;
    position->vel_y += accel_y_world * dt * VELOCITY_DECAY / 100;

    if (fabs(position->vel_x < VELOCITY_THRESHOLD) && fabs(position->vel_y < VELOCITY_THRESHOLD)) {
        position->vel_x = 0;
        position->vel_y = 0;
    }

    // Integrate velocity to get position
    position->pos_x += position->vel_x * dt;
    position->pos_y += position->vel_y * dt;
}


void CalcPosition(PathCalc* pathCalc, SensorData sensorData, double dt) {
    complementaryFilter(sensorData, pathCalc->filteredAngles, dt);
    lowPassFilter(sensorData, pathCalc->filterAccel);
    calcPosition(pathCalc->filterAccel, pathCalc->filteredAngles, &pathCalc->positionData, dt);
}