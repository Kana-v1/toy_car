//
// Created by kana on 10/26/2024.
//

#include "accelerometer.h"

static I2C_Handle_t i2cHandle;

uint8_t Accelerometer_Init(void) {
    i2cHandle.pI2Cx = I2C1;

    // Wait after initialization
    for (volatile uint32_t i = 0; i < 100000; i++);

    // Configure CTRL_REG1_A for 100Hz data rate, normal mode, all axes enabled
    uint8_t config = 0x57;  // 0101 0111b
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {LSM303_CTRL_REG1_A, config}, 2, LSM303_ADDR_ACCEL, 0);

    // Wait between configurations
    for (volatile uint32_t i = 0; i < 10000; i++);

    // Configure CTRL_REG4_A for ±2g range, high resolution mode
    config = 0x08;  // 0000 1000b
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {LSM303_CTRL_REG4_A, config}, 2, LSM303_ADDR_ACCEL, 0);

    return 0;
}

uint8_t Accelerometer_ReadAccel(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t data[6];

    // Read all 6 bytes at once using auto-increment
    uint8_t reg = LSM303_OUT_X_L_A | 0x80;  // Set MSB for auto-increment
    if (I2C_MasterSendData(&i2cHandle, &reg, 1, LSM303_ADDR_ACCEL, 1) != 0) {
        return 1;
    }

    if (I2C_MasterReceiveData(&i2cHandle, data, 6, LSM303_ADDR_ACCEL) != 0) {
        return 1;
    }

    // Combine bytes and shift right by 4 to get 12-bit values
    *ax = ((int16_t) ((data[1] << 8) | data[0])) >> 4;
    *ay = ((int16_t) ((data[3] << 8) | data[2])) >> 4;
    *az = ((int16_t) ((data[5] << 8) | data[4])) >> 4;
}

uint8_t Accelerometer_GetAcceleration(float* ax, float* ay, float* az) {
    int16_t raw_ax, raw_ay, raw_az;

    // Read accelerometer data
    if (Accelerometer_ReadAccel(&raw_ax, &raw_ay, &raw_az) != 0) {
        return 1;
    }

    // Convert to g's (±2g range, 12-bit resolution)
    float ax_g = ((float) raw_ax / 2048.0f);
    float ay_g = ((float) raw_ay / 2048.0f);
    float az_g = ((float) raw_az / 2048.0f);

    // Convert to m/s²
    *ax = ax_g * 9.81f;
    *ay = ay_g * 9.81f;
    *az = az_g * 9.81f;

    return 0;
}
