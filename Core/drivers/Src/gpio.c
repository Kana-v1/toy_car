//
// Created by kana on 6/11/23.
//

#include "gpio.h"


void GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
    GPIO_PeriClockEnable(pGPIOHandle->pGPIOx);

    uint32_t tmp = 0;

    if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_PIN_MODE_ANALOG) {
        tmp |= pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        pGPIOHandle->pGPIOx->MODER &= ~(0b11 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        pGPIOHandle->pGPIOx->MODER |= tmp;
        tmp = 0;
    } else {

    }

    tmp |= pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0b11 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OSPEEDR |= tmp;
    tmp = 0;

    tmp |= pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->PUPDR &= ~(0b11 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->PUPDR |= tmp;
    tmp = 0;

    tmp |= pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
    pGPIOHandle->pGPIOx->OTYPER &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OTYPER |= tmp;
    tmp = 0;

    if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_PIN_MODE_ALT_FN) {
        uint8_t afrRegisters = 8;
        uint8_t afrRegisterBits = 4;

        uint8_t afrNum = pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunNum / afrRegisters;
        uint8_t afrOffset = pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunNum % afrRegisters;

        pGPIOHandle->pGPIOx->AFR[afrNum] &= ~(0b1111 << (afrRegisterBits * afrOffset));
        pGPIOHandle->pGPIOx->AFR[afrNum] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunNum << (afrRegisterBits * afrOffset));
    }
}

void GPIO_PeriClockEnable(GPIO_TypeDef *pGPIOx) {
    if (pGPIOx == GPIOA) {
        GPIOA_PCLK_EN();
        return;
    }
    if (pGPIOx == GPIOB) {
        GPIOB_PCLK_EN();
        return;
    }
    if (pGPIOx == GPIOC) {
        GPIOC_PCLK_EN();
        return;
    }
    if (pGPIOx == GPIOD) {
        GPIOD_PCLK_EN();
        return;
    }
    if (pGPIOx == GPIOE) {
        GPIOE_PCLK_EN();
        return;
    }

    if (pGPIOx == GPIOF) {
        GPIOF_PCLK_EN();
        return;
    }
}

uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber) {
    return (uint8_t) ((pGPIOx->IDR >> pinNumber) & 1);
}

uint32_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx) {
    return pGPIOx->IDR;
}

void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber, uint8_t value) {
    if (value == SET) {
        pGPIOx->ODR |= (1 << pinNumber);
    } else {
        pGPIOx->ODR &= ~(1 << pinNumber);
    }
}

void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint16_t value) {
    pGPIOx->ODR = value;
}

void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t pinNumber) {
    pGPIOx->ODR ^= (1 << pinNumber);
}

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t enOrDi) {

}

void GPIO_IRQHandling(uint8_t pinNumber) {

}