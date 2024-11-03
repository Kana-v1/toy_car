//
// Created by kana on 10/26/2024.
//

#include "accelerometer.h"
#include "stm32f3xx_hal.h"

#define SAMPLES_PER_CALIBRATION 50
#define CALIBRATION_DELAY_MS    20

static I2C_Handle_t i2cHandle;

typedef struct {
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;
} Accel_Calibration_t;

static Accel_Calibration_t calibration = {0};

static void calibrateAccel(void) {
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;
    int16_t raw_x, raw_y, raw_z;

    for (int i = 0; i < SAMPLES_PER_CALIBRATION; i++) {
        if (Accelerometer_ReadAccel(&raw_x, &raw_y, &raw_z) != 0) {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);  // Error LED
            return;
        }

        sum_x += raw_x;
        sum_y += raw_y;
        sum_z += raw_z;

        HAL_Delay(CALIBRATION_DELAY_MS);
    }

    // Calculate offsets needed to bring X and Y to 0, and Z to expected rest value
    int16_t avg_x = (int16_t) (sum_x / SAMPLES_PER_CALIBRATION);
    int16_t avg_y = (int16_t) (sum_y / SAMPLES_PER_CALIBRATION);
    int16_t avg_z = (int16_t) (sum_z / SAMPLES_PER_CALIBRATION);

    calibration.offset_x = abs(avg_x);
    calibration.offset_y = abs(avg_y);
    calibration.offset_z = abs(avg_z);
}


static void applyLowPassFilter(int16_t* value, int16_t new_value) {
    // Alpha = 0.1 in fixed point (0.1 * 256 = 26)
    const int16_t ALPHA = 26;
    *value = (ALPHA * new_value + (256 - ALPHA) * (*value)) / 256;
}

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

    HAL_Delay(1000);
    calibrateAccel();

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
    static int16_t filtered_ax = 0, filtered_ay = 0, filtered_az = 0;
    const int16_t THRESHOLD = 30;  // Adjust based on raw value noise

    int16_t raw_ax, raw_ay, raw_az;

    // Read accelerometer data
    if (Accelerometer_ReadAccel(&raw_ax, &raw_ay, &raw_az) != 0) {
        return 1;
    }

    int sign = 1;

    if (raw_ax > 0) {
        sign = -1;
    }
    raw_ax += sign * calibration.offset_x;
    if (raw_ay > 0) {
        sign = -1;
    }
    raw_ay += sign * calibration.offset_y;
    if (raw_az > 0) {
        sign = -1;
    }
    raw_az += sign * calibration.offset_z;


    applyLowPassFilter(&filtered_ax, raw_ax);
    applyLowPassFilter(&filtered_ay, raw_ay);
    applyLowPassFilter(&filtered_az, raw_az);

    if (abs(filtered_ax) < THRESHOLD) filtered_ax = 0;
    if (abs(filtered_ay) < THRESHOLD) filtered_ay = 0;
    if (abs(filtered_az) < THRESHOLD) filtered_az = 0;

    // Convert to g's (±2g range, 12-bit resolution)
    float ax_g = ((float) filtered_ax / 2048.0f);
    float ay_g = ((float) filtered_ay / 2048.0f);
    float az_g = ((float) filtered_az / 2048.0f);

    // Convert to m/s²
    *ax = ax_g * 9.81f;
    *ay = ay_g * 9.81f;
    *az = az_g * 9.81f;

    return 0;
}
