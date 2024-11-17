//
// Created by kana on 11/2/2024.
//

#ifndef TOY_CAR_SPI_H
#define TOY_CAR_SPI_H

#include "gpio.h"
#include <stdint.h>

typedef struct {
    SPI_TypeDef *pSPIx;
    uint8_t SPI_DeviceMode;
    uint8_t SPI_BusConfig;
    uint8_t SPI_SclkSpeed;
    uint8_t SPI_DFF;
    uint8_t SPI_CPOL;
    uint8_t SPI_CPHA;
    uint8_t SPI_SSM;
} SPI_Handle_t;

#define SPI1_GPIO GPIOA
#define SPI1_SCK_PIN     5   // PA5
#define SPI1_MISO_PIN    6   // PA6
#define SPI1_MOSI_PIN    7   // PA7

uint8_t SPI_Init(SPI_Handle_t *pSPIHandle);
uint8_t SPI_TransmitReceive(SPI_TypeDef *pSPIx, uint8_t data);

#endif //TOY_CAR_SPI_H
