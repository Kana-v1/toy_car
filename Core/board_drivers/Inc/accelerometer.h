//
// Created by kana on 10/26/2024.
//



#ifndef TOY_CAR_ACCELEROMETER_H
#define TOY_CAR_ACCELEROMETER_H

#include "stm32f303xc.h"
#include "i2c.h"

// LSM303 is accelerometer
#define LSM303_ADDR_ACCEL    0x19    // 7-bit address
#define LSM303_CTRL_REG1_A   0x20
#define LSM303_CTRL_REG4_A   0x23
#define LSM303_OUT_X_L_A     0x28
#define LSM303_OUT_X_H_A     0x29
#define LSM303_OUT_Y_L_A     0x2A
#define LSM303_OUT_Y_H_A     0x2B
#define LSM303_OUT_Z_L_A     0x2C
#define LSM303_OUT_Z_H_A     0x2D

#define LSM303_ACCEL_RATE        0x67  // 100Hz, normal mode, XYZ enabled
#define LSM303_ACCEL_SCALE       0x00  // ±2g scale

uint8_t Accelerometer_Init(void);

uint8_t Accelerometer_ReadAccel(int16_t* ax, int16_t* ay, int16_t* az);

uint8_t Accelerometer_GetAcceleration(float* ax, float* ay, float* az);

uint8_t LSM303_Init(void);

void GetAcceleration(float* ax_g, float* ay_g, float* az_g);

#endif //TOY_CAR_ACCELEROMETER_H
