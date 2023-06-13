//
// Created by kana on 6/12/23.
//

#include "drivers_general.h"

void EXTI0_IRQHandler(void) {
    GPIO_IRQHandling(0);
    GPIO_InterruptCallback(0);
}

__attribute__((weak)) void GPIO_InterruptCallback(uint8_t extiLine) {}
