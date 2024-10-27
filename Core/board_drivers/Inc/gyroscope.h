//
// Created by kana on 10/27/2024.
//

#ifndef TOY_CAR_GYROSCOPE_H
#define TOY_CAR_GYROSCOPE_H

#include "stm32f3xx.h"
#include "i2c.h"

#define L3GD20_ADDR        0x6B    // SAD[0] = 1 for the F3 Discovery
#define WHO_AM_I           0x0F
#define CTRL_REG1          0x20
#define CTRL_REG2          0x21
#define CTRL_REG3          0x22
#define CTRL_REG4          0x23
#define CTRL_REG5          0x24
#define REFERENCE          0x25
#define OUT_TEMP           0x26
#define STATUS_REG         0x27
#define OUT_X_L            0x28
#define OUT_X_H            0x29
#define OUT_Y_L            0x2A
#define OUT_Y_H            0x2B
#define OUT_Z_L            0x2C
#define OUT_Z_H            0x2D

typedef struct {
    int16_t x;          // Angular velocity around X axis
    int16_t y;          // Angular velocity around Y axis
    int16_t z;          // Angular velocity around Z axis
    float heading;      // Current heading in radians
} GyroData;

uint8_t Gyroscope_Init(void);
uint8_t Gyroscope_ReadData(int16_t* x, int16_t* y, int16_t* z);

#endif //TOY_CAR_GYROSCOPE_H
