//
// Created by kana on 11/3/2024.
//

#include "calc.h"

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
}

static void calcPosition(SensorData* filteredAccel, FilteredAngles* angles, PositionData* position, double dt) {
    // Project the filtered acceleration onto the X and Y plane based on orientation
    float accel_x_world = filteredAccel->accel_x * cos(angles->angle_y * M_PI / 180);
    float accel_y_world = filteredAccel->accel_y * cos(angles->angle_x * M_PI / 180);

    // Integrate acceleration to get velocity
    position->vel_x += accel_x_world * dt;
    position->vel_y += accel_y_world * dt;

    // Integrate velocity to get position
    position->pos_x += position->vel_x * dt;
    position->pos_y += position->vel_y * dt;
}

void CalcPosition(PathCalc* pathCalc, SensorData sensorData, double dt) {
    complementaryFilter(sensorData, pathCalc->filteredAngles, dt);
    lowPassFilter(sensorData, pathCalc->filterAccel);
    calcPosition(pathCalc->filterAccel, pathCalc->filteredAngles, &pathCalc->positionData, dt);
}