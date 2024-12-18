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
    return (LED_PORT->ODR >> GREEN_LED_PIN) & 1;
}

void toggleCarState(void) {
    GPIO_ToggleOutputPin(LED_PORT, GREEN_LED_PIN);
    GPIO_ToggleOutputPin(LED_PORT, RED_LED_PIN);

    uint8_t newCarState = getCarState();
    if (newCarState == CAR_STATE_ON) {
        carMoveForward();
    } else {
        carStop();
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
    GPIO_ToggleOutputPin(LED_PORT, GREEN_LED_PIN);

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


void carMoveForward(void) {
    if (getCarState() == CAR_STATE_OFF) {
        return;
    }

    engineClockWiseRotating(RIGHT_ENGINE);
    engineClockWiseRotating(LEFT_ENGINE);
}

void carMoveBack(void) {
    if (getCarState() == CAR_STATE_OFF) {
        return;
    }

    engineAnticlockwiseRotating(RIGHT_ENGINE);
    engineAnticlockwiseRotating(LEFT_ENGINE);
}

void carStop(void) {
    turnOffEngines();
}

void carRotateRight(uint8_t rotateSpeed) {
    if (getCarState() == CAR_STATE_OFF) {
        return;
    }

    engineClockWiseRotating(LEFT_ENGINE);

    if (rotateSpeed == ROTATE_SPEED_FAST) {
        engineAnticlockwiseRotating(RIGHT_ENGINE);
    } else {
        engineHold(RIGHT_ENGINE);
    }
}

void carRotateLeft(uint8_t rotateSpeed) {
    if (getCarState() == CAR_STATE_OFF) {
        return;
    }

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

void carRotate(float radians) {

    float rotatePerMSDeg = 0.01f;
    float angleDeg = radians * (float) (180 / M_PI);

    angleDeg = fmodf(angleDeg, 360.0f);

    float angleToRotate;

    if (angleDeg > 180) {
        angleToRotate = 360 - angleDeg;
        carRotateLeft(ROTATE_SPEED_NORMAL);
    } else {
        angleToRotate = angleDeg;
        carRotateRight(ROTATE_SPEED_NORMAL);
    }

    float rotationTime = ceilf(angleToRotate / rotatePerMSDeg);
    HAL_Delay((uint32_t) rotationTime);
}

