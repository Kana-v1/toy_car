//
// Created by kana on 6/24/23.
//

#include "engine_drivers.h"

static uint8_t getCRPin(uint8_t engineSide);    // counterclockwise pin
static uint8_t getACRPin(uint8_t engineSide);   // anticlockwise pin

void clockWiseRotating(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, ENABLE);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, DISABLE);
}

void anticlockwiseRotating(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, DISABLE);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, ENABLE);
}

void hold(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, ENABLE);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, ENABLE);
}

void turnOffEngines(void) {
    hold(RIGHT_ENGINE);
    hold(LEFT_ENGINE);
}

void initEngines(void) {
    GPIO_Handle_t engine;

    engine.pGPIOx = ENGINE_PORT;
    engine.GPIO_PinConfig.GPIO_PinNumber = LEFT_ENGINE;
    engine.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    engine.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    engine.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    engine.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&engine);
    engine.GPIO_PinConfig.GPIO_PinNumber = RIGHT_ENGINE;
    GPIO_Init(&engine);
}

uint8_t getCRPin(uint8_t engineSide) {
    if (engineSide == LEFT_ENGINE) {
        return LECR;
    }

    return RECR;
}

uint8_t getACRPin(uint8_t engineSide) {
    if (engineSide == LEFT_ENGINE) {
        return LEACR;
    }

    return REACR;
}
