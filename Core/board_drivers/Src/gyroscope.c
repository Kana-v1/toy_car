//
// Created by kana on 10/27/2024.
//

#include "gyroscope.h"

static I2C_Handle_t i2cHandle;

uint8_t Gyroscope_Init(void) {
    // Initialize I2C
    i2cHandle.pI2Cx = I2C1;

    // Check device ID
    uint8_t who_am_i;
    I2C_ReadReg(&i2cHandle, L3GD20_ADDR, WHO_AM_I, &who_am_i, 1);
    if (who_am_i != 0xD4) {  // L3GD20 should return 0xD4
        return 1;  // Device not found
    }

    // Configure CTRL_REG1
    // ODR = 95Hz (0b10), Cut-off = 25 (0b10), Enable all axes (0b111)
    uint8_t config = 0x4F;  // 0b01001111
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {CTRL_REG1, config}, 2, L3GD20_ADDR, 0);

    // Configure CTRL_REG2
    // High-pass filter normal mode, cut-off frequency = 8Hz
    config = 0x00;
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {CTRL_REG2, config}, 2, L3GD20_ADDR, 0);

    // Configure CTRL_REG3
    // No interrupts
    config = 0x00;
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {CTRL_REG3, config}, 2, L3GD20_ADDR, 0);

    // Configure CTRL_REG4
    // ±250 dps full scale
    config = 0x00;
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {CTRL_REG4, config}, 2, L3GD20_ADDR, 0);

    // Configure CTRL_REG5
    // High-pass filter disabled
    config = 0x00;
    I2C_MasterSendData(&i2cHandle, (uint8_t[]) {CTRL_REG5, config}, 2, L3GD20_ADDR, 0);

    return 0;
}

uint8_t Gyroscope_ReadData(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t data[6];

    // Read all axes data
    uint8_t reg = OUT_X_L | 0x80;  // Set MSB for auto-increment
    if (I2C_MasterSendData(&i2cHandle, &reg, 1, L3GD20_ADDR, 1) != 0) {
        return 1;
    }

    if (I2C_MasterReceiveData(&i2cHandle, data, 6, L3GD20_ADDR) != 0) {
        return 2;
    }

    *x = (int16_t)((data[1] << 8) | data[0]);
    *y = (int16_t)((data[3] << 8) | data[2]);
    *z = (int16_t)((data[5] << 8) | data[4]);

    return 0;
}