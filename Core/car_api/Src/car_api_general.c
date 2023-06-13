//
// Created by kana on 6/13/23.
//

#include "car_api_general.h"

static void fixBtnDebounce(void);

void carInit(void) {
    GPIO_Handle_t btn, led, obstaclesScanner;

    led.pGPIOx = GPIOE;
    led.GPIO_PinConfig.GPIO_PinNumber = GREEN_LED_PIN;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&led);
    led.GPIO_PinConfig.GPIO_PinNumber = RED_LED_PIN;
    GPIO_Init(&led);
    GPIO_ToggleOutputPin(GPIOE, RED_LED_PIN);

    btn.pGPIOx = GPIOA;
    btn.GPIO_PinConfig.GPIO_PinNumber = 0;
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INTERRUPT_FALLING_EDGE;
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    btn.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&btn);

    obstaclesScanner.pGPIOx = OBSTACLES_SCANNER_PORT;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinNumber = OBSTACLES_SCANNER_PIN;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_INPUT;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    obstaclesScanner.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&obstaclesScanner);


    GPIO_IRQConfig(IRQ_NO_EXTI0, ENABLE);                   // enable interrupts for button
    GPIO_IRQPriorityConfig(IRQ_PRIO_15, IRQ_NO_EXTI0);      // enable interrupts for button
}

uint8_t getCarState(void) {
    return GPIOE->ODR & (1 << GREEN_LED_PIN);
}

void toggleCarState(void) {
    GPIO_ToggleOutputPin(GPIOE, GREEN_LED_PIN);
    GPIO_ToggleOutputPin(GPIOE, RED_LED_PIN);
}

void toggleObstaclesScanner(void) {
    RCC->AHBENR ^= (1 << 19);
}

uint8_t isObstacleAhead(void) {
    return !(GPIO_ReadFromInputPin(OBSTACLES_SCANNER_PORT, OBSTACLES_SCANNER_PIN));
}

void GPIO_InterruptCallback(uint8_t extiLine) {
    fixBtnDebounce();
    toggleCarState();
}

void fixBtnDebounce(void) {
    for (uint16_t i = 0; i < 20000; i++) {}
}