//
// Created by kana on 11/2/2024.
//

#include "systick.h"

void SysTick_Init(void) {
    SysTick_Config(8000);
}

uint32_t GetTick(void) {
    return HAL_GetTick();
}