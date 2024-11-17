//
// Created by kana on 10/26/2024.
//

#ifndef GYROSCOPE_H
#define GYROSCOPE_H


#include "spi.h"
#include "gpio.h"
#include <math.h>
#include <stdint.h>

// L3GD20 Register Addresses
#define L3GD20_WHO_AM_I          0x0F
#define L3GD20_WHO_AM_I_VALUE    0xD3
#define L3GD20_CTRL_REG1         0x20
#define L3GD20_CTRL_REG2         0x21
#define L3GD20_CTRL_REG3         0x22
#define L3GD20_CTRL_REG4         0x23
#define L3GD20_CTRL_REG5         0x24
#define L3GD20_REFERENCE         0x25
#define L3GD20_STATUS_REG        0x27
#define L3GD20_OUT_X_L           0x28
#define L3GD20_OUT_X_H           0x29
#define L3GD20_OUT_Y_L           0x2A
#define L3GD20_OUT_Y_H           0x2B
#define L3GD20_OUT_Z_L           0x2C
#define L3GD20_OUT_Z_H           0x2D

#define GYRO_THRESHOLD 5

uint8_t Gyroscope_Init(void);
void Gyroscope_ReadCookedData(int16_t* gx, int16_t* gy, int16_t* gz, float bias);
void Gyroscope_ReadRawData(int16_t* gx, int16_t* gy, int16_t* gz);
float Gyroscope_Calibrate(uint16_t samples);

#endif //GYROSCOPE_H