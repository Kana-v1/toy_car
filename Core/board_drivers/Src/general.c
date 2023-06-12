//
// Created by kana on 6/12/23.
//

#include "drivers_general.h"

void EXTI0_IRQHandler(void) {
    GPIO_IRQHandling(0);
}