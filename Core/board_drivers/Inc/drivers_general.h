//
// Created by kana on 6/11/23.
//


#ifndef TOY_CAR_DRIVERS_GENERAL_H
#define TOY_CAR_DRIVERS_GENERAL_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f303xc.h"
#include "gpio.h"
#include "stm32f3xx_hal.h"


#define GPIOA_PCLK_EN()    (RCC->AHBENR |= (1 << 17))
#define GPIOB_PCLK_EN()    (RCC->AHBENR |= (1 << 18))
#define GPIOC_PCLK_EN()    (RCC->AHBENR |= (1 << 19))
#define GPIOD_PCLK_EN()    (RCC->AHBENR |= (1 << 20))
#define GPIOE_PCLK_EN()    (RCC->AHBENR |= (1 << 21))
#define GPIOF_PCLK_EN()    (RCC->AHBENR |= (1 << 22))

#define SYSCLK_PCLK_EN()         (RCC->APB2ENR |= (1 << 0))


#define ENABLE      1
#define DISABLE     0
#define SET         ENABLE
#define RESET       DISABLE


#define IRQ_NO_EXTI0            6
#define IRQ_NO_EXTI1            7
#define IRQ_NO_EXTI2_TS         8
#define IRQ_NO_EXTI3            9
#define IRQ_NO_EXTI4            10
#define IRQ_NO_EXTI9_5          23
#define IRQ_NO_EXTI15_10        40

#define IRQ_PRIO_14             14
#define IRQ_PRIO_15             15

#define LED_PORT        GPIOE

#define GREEN_LED_PIN   11
#define RED_LED_PIN     10


void GPIO_InterruptCallback(uint8_t extiLine);

#endif //TOY_CAR_DRIVERS_GENERAL_H
