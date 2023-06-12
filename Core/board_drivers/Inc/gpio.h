//
// Created by kana on 6/11/23.
//


#ifndef TOY_CAR_GPIO_H
#define TOY_CAR_GPIO_H

#include "drivers_general.h"

typedef struct {
    uint8_t GPIO_PinNumber;
    uint8_t GPIO_PinMode;           // @GPIO_PinMode
    uint8_t GPIO_PinSpeed;          // @GPIO_PinSpeed
    uint8_t GPIO_PinPuPdControl;    // @GPIO_PinPuPdControl
    uint8_t GPIO_PinOPType;         // @GPIO_PinOPType
    uint8_t GPIO_PinAltFunNum;
} GPIO_PinConfig_t;

typedef struct {
    GPIO_TypeDef *pGPIOx;
    GPIO_PinConfig_t GPIO_PinConfig;
} GPIO_Handle_t;

// @GPIO_PinMode
#define GPIO_PIN_MODE_INPUT                                  0
#define GPIO_PIN_MODE_OUTPUT                                 1
#define GPIO_PIN_MODE_ALT_FN                                 2
#define GPIO_PIN_MODE_ANALOG                                 3
#define GPIO_MODE_INPUT_FALLING_EDGE                         4
#define GPIO_MODE_INPUT_RISING_EDGE                          5
#define GPIO_MODE_INPUT_FALLING_EDGE_TRIGGER                 5
#define GPIO_MODE_INPUT_RAISING_EDGE_TRIGGER                 6
#define GPIO_MODE_INPUT_RAISING_EDGE_FALLING_EDGE_TRIGGER    7

// @GPIO_PinOPType
#define GPIO_OUTPUT_TYPE_PUSH_PULL    0
#define GPIO_OUTPUT_TYPE_OPEN_DRAIN   1

// @GPIO_PinSpeed
#define GPIO_SPEED_LOW        0
#define GPIO_SPEED_MEDIUM     1
#define GPIO_SPEED_FAST       2
#define GPIO_SPEED_HiGH       3

// @GPIO_PinPuPdControl
#define GPIO_NO_PUPD    0
#define GPIO_PU         1
#define GPIO_PD         2

void GPIO_Init(GPIO_Handle_t *pGPIOHandle);

void GPIO_PeriClockEnable(GPIO_TypeDef *pGPIOx);

uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber);

uint32_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx);

void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber, uint8_t vlaue);

void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint16_t value);

void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber);

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t enOrDi);

void GPIO_IRQPriorityConfig(uint8_t priority, uint8_t IRQNumber);

void GPIO_IRQHandling(uint8_t pinNumber);

#endif //TOY_CAR_GPIO_H
