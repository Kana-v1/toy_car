//
// Created by kana on 6/24/23.
//

#ifndef TOY_CAR_OBSTACLES_SENSOR_DRIVER_H
#define TOY_CAR_OBSTACLES_SENSOR_DRIVER_H

#include "car_driver.h"

#define OBSTACLES_SCANNER_PORT  GPIOC
#define OBSTACLES_SCANNER_PIN   14

void initObstaclesSensor(void);

void toggleObstaclesSensor(void);

uint8_t isObstacleAhead(void);


#endif //TOY_CAR_OBSTACLES_SENSOR_DRIVER_H
