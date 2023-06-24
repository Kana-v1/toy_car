//
// Created by kana on 6/24/23.
//

#include "obstacles_sensor_driver.h"

void initObstaclesSensor(void) {
    GPIO_Handle_t btn, led, obstaclesScanner;
    obstaclesScanner.pGPIOx = OBSTACLES_SCANNER_PORT;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinNumber = OBSTACLES_SCANNER_PIN;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_INPUT;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;
    GPIO_Init(&obstaclesScanner);
}

void toggleObstaclesSensor(void) {
    RCC->AHBENR ^= (1 << 19);
}

uint8_t isObstacleAhead(void) {
    return !(GPIO_ReadFromInputPin(OBSTACLES_SCANNER_PORT, OBSTACLES_SCANNER_PIN));
}