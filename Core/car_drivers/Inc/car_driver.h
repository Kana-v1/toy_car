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

#define ROTATE_SPEED_NORMAL     0
#define ROTATE_SPEED_FAST       1

#define TOGGLE_STATE_BTN_PORT   GPIOA
#define TOGGLE_STATE_BTN_PIN    0

#define TURN_LEFT   0
#define TURN_RIGHT  1

void carInit(void);

uint8_t getCarState(void);

void toggleCarState(void);

void initCarManagementPeripheral(void);

void moveForward(void);

void moveBack(void);

void stop(void);

void rotateRight(uint8_t rotateSpeed);

void rotateLeft(uint8_t rotateSpeed);

void handleBtnInterrupt(void);

#endif //TOY_CAR_CAR_DRIVER_H
