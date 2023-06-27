//
// Created by kana on 6/24/23.
//

#ifndef TOY_CAR_ENGINE_DRIVERS_H
#define TOY_CAR_ENGINE_DRIVERS_H

#include "car_driver.h"

#define ENGINE_PORT GPIOC

#define RECR    6   // right engine counterclockwise rotating
#define REACR   7   // right engine anticlockwise rotating

#define LECR    8   // left engine counterclockwise rotating
#define LEACR   9   // left engine anticlockwise rotating

#define LEFT_ENGINE      0
#define RIGHT_ENGINE     1

void engineClockWiseRotating(uint8_t engineSide);

void engineAnticlockwiseRotating(uint8_t engineSide);

void engineHold(uint8_t engineSide);

void turnOffEngines(void);

void initEngines(void);


#endif //TOY_CAR_ENGINE_DRIVERS_H
