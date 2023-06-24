//
// Created by kana on 6/13/23.
//

#ifndef TOY_CAR_CAR_DRIVER_H
#define TOY_CAR_CAR_DRIVER_H

#include "drivers_general.h"
#include "engine_drivers.h"
#include "obstacles_sensor_driver.h"

#define CAR_STATE_ON    1
#define CAR_STATE_OFF   0


void carInit(void);

uint8_t getCarState(void);

void toggleCarState(void);

void initCarManagementPeripheral(void);

#endif //TOY_CAR_CAR_DRIVER_H
