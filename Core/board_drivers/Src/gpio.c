//
// Created by kana on 6/11/23.
//

#include "gpio.h"

static uint8_t gpioBaseAddrToCode(GPIO_TypeDef *pGPIOx);

void GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
    GPIO_PeriClockEnable(pGPIOHandle->pGPIOx);

    uint32_t tmp = 0;

    if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_PIN_MODE_ANALOG) {
        tmp |= pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        pGPIOHandle->pGPIOx->MODER &= ~(0b11 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        pGPIOHandle->pGPIOx->MODER |= tmp;
        tmp = 0;
    } else {
        // configure interrupts
        if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_INTERRUPT_FALLING_EDGE) {
            EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        } else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_INTERRUPT_RISING_EDGE) {
            EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        } else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_INTERRUPT_RAISING_EDGE_FALLING_EDGE) {
            EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        }

        EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

        uint8_t groupsInExtiRegister = 4;
        uint8_t bitsInExtiGroup = 4;

        uint8_t extiIndex = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / groupsInExtiRegister;
        uint8_t bitsOffset = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % groupsInExtiRegister * bitsInExtiGroup;

        uint8_t portCode = gpioBaseAddrToCode(pGPIOHandle->pGPIOx);
        SYSCLK_PCLK_EN();
        SYSCFG->EXTICR[extiIndex] = (portCode << bitsOffset);
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

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t enOrDi) {
    uint8_t iserBits = 32;
    uint8_t iserIndex = IRQNumber / iserBits;
    uint8_t iserBitOffset = IRQNumber % iserBits;

    if (enOrDi == ENABLE) {
        NVIC->ISER[iserIndex] |= (1 << iserBitOffset);
        return;
    }

    NVIC->ISER[iserIndex] &= ~(1 << iserBitOffset);
}

void GPIO_IRQPriorityConfig(uint8_t priority, uint8_t IRQNumber) {
    uint8_t numNoImplementedBits = 4;

    uint8_t ipRegisterSections = 4;
    uint8_t bitsInSection = 8;

    uint8_t ipIndex = IRQNumber / ipRegisterSections;
    uint8_t ipOffset = IRQNumber % ipRegisterSections * bitsInSection + (8 - numNoImplementedBits);

    NVIC->IP[ipIndex] = (priority << ipOffset);
}

void GPIO_IRQHandling(uint8_t pinNumber) {
    // clear the exti pr register corresponding to the pin's number
    if (EXTI->PR & (1 << pinNumber)) {
        // clear the pr register
        EXTI->PR |= (1 << pinNumber);
    }
}

static uint8_t gpioBaseAddrToCode(GPIO_TypeDef *pGPIOx) {
    if (pGPIOx == GPIOA) {
        return 0b000;
    }

    if (pGPIOx == GPIOB) {
        return 0b001;
    }

    if (pGPIOx == GPIOC) {
        return 0b010;
    }

    if (pGPIOx == GPIOD) {
        return 0b011;
    }

    if (pGPIOx == GPIOE) {
        return 0b100;
    }

    if (pGPIOx == GPIOF) {
        return 0b101;
    }
}