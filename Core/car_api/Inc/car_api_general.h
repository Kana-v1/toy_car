//
// Created by kana on 6/13/23.
//

#ifndef TOY_CAR_CAR_API_GENERAL_H
#define TOY_CAR_CAR_API_GENERAL_H

#include "drivers_general.h"

#define CAR_STATE_ON    1
#define CAR_STATE_OFF   0

#define GREEN_LED_PIN   11
#define RED_LED_PIN     10

void carInit(void);
uint8_t getCarState(void);
void toggleCarState(void);

#endif //TOY_CAR_CAR_API_GENERAL_H
