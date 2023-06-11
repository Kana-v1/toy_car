//
// Created by kana on 6/11/23.
//


#ifndef TOY_CAR_DRIVERS_GENERAL_H
#define TOY_CAR_DRIVERS_GENERAL_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stm32f303xc.h"
#include "gpio.h"


#define GPIOA_PCLK_EN()    (RCC->AHBENR |= (1 << 17))
#define GPIOB_PCLK_EN()    (RCC->AHBENR |= (1 << 18))
#define GPIOC_PCLK_EN()    (RCC->AHBENR |= (1 << 19))
#define GPIOD_PCLK_EN()    (RCC->AHBENR |= (1 << 20))
#define GPIOE_PCLK_EN()    (RCC->AHBENR |= (1 << 21))
#define GPIOF_PCLK_EN()    (RCC->AHBENR |= (1 << 22))

#define ENABLE      1
#define DISABLE     0
#define SET         ENABLE
#define RESET       DISABLE

#endif //TOY_CAR_DRIVERS_GENERAL_H
