//
// Created by kana on 6/24/23.
//

#include "engine_drivers.h"

static uint8_t getCRPin(uint8_t engineSide);    // counterclockwise pin
static uint8_t getACRPin(uint8_t engineSide);   // anticlockwise pin

void engineClockWiseRotating(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, SET);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, RESET);
}

void engineAnticlockwiseRotating(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, RESET);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, SET);
}

void engineHold(uint8_t engineSide) {
    uint8_t acrPin = getACRPin(engineSide);
    uint8_t crPin = getCRPin(engineSide);

    GPIO_WriteToOutputPin(ENGINE_PORT, crPin, SET);
    GPIO_WriteToOutputPin(ENGINE_PORT, acrPin, SET);
}

void turnOffEngines(void) {
    engineHold(RIGHT_ENGINE);
    engineHold(LEFT_ENGINE);
}

void initEngines(void) {
    GPIO_Handle_t engine;

    engine.pGPIOx = ENGINE_PORT;
    engine.GPIO_PinConfig.GPIO_PinNumber = LECR;
    engine.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    engine.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    engine.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    engine.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&engine);
    engine.GPIO_PinConfig.GPIO_PinNumber = LEACR;
    GPIO_Init(&engine);

    engine.GPIO_PinConfig.GPIO_PinNumber = RECR;
    GPIO_Init(&engine);
    engine.GPIO_PinConfig.GPIO_PinNumber = REACR;
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
