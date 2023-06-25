//
// Created by kana on 6/13/23.
//

#include "car_driver.h"

void carInit(void) {
    initCarManagementPeripheral();
    initObstaclesSensor();
    initEngines();
}

uint8_t getCarState(void) {
    return GPIOE->ODR & (1 << GREEN_LED_PIN);
}

void toggleCarState(void) {
    GPIO_ToggleOutputPin(LED_PORT, GREEN_LED_PIN);
    GPIO_ToggleOutputPin(LED_PORT, RED_LED_PIN);

    if (getCarState() == CAR_STATE_ON) {
        moveForward();
    } else {
        stop();
    }
}

void initCarManagementPeripheral(void) {
    GPIO_Handle_t btn, led;

    led.pGPIOx = LED_PORT;
    led.GPIO_PinConfig.GPIO_PinNumber = GREEN_LED_PIN;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&led);
    led.GPIO_PinConfig.GPIO_PinNumber = RED_LED_PIN;
    GPIO_Init(&led);
    GPIO_ToggleOutputPin(GPIOE, RED_LED_PIN);

    btn.pGPIOx = TOGGLE_STATE_BTN_PORT;
    btn.GPIO_PinConfig.GPIO_PinNumber = TOGGLE_STATE_BTN_PIN;
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INTERRUPT_FALLING_EDGE;
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    btn.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&btn);

    GPIO_IRQConfig(IRQ_NO_EXTI0, ENABLE);                   // enable interrupts for button
    GPIO_IRQPriorityConfig(IRQ_PRIO_15, IRQ_NO_EXTI0);      // enable interrupts for button
}


void moveForward(void) {
    engineClockWiseRotating(RIGHT_ENGINE);
    engineClockWiseRotating(LEFT_ENGINE);
}

void moveBack(void) {
    engineAnticlockwiseRotating(RIGHT_ENGINE);
    engineAnticlockwiseRotating(LEFT_ENGINE);
}

void stop(void) {
    turnOffEngines();
}

void rotateRight(uint8_t rotateSpeed) {
    engineClockWiseRotating(LEFT_ENGINE);

    if (rotateSpeed == ROTATE_SPEED_FAST) {
        engineAnticlockwiseRotating(RIGHT_ENGINE);
    } else {
        engineHold(RIGHT_ENGINE);
    }
}

void rotateLeft(uint8_t rotateSpeed) {
    engineClockWiseRotating(RIGHT_ENGINE);

    if (rotateSpeed == ROTATE_SPEED_FAST) {
        engineAnticlockwiseRotating(LEFT_ENGINE);
    } else {
        engineHold(LEFT_ENGINE);
    }
}

void handleBtnInterrupt(void) {
    toggleCarState();
}

