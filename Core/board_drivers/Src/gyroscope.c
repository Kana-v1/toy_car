//
// Created by kana on 10/26/2024.
//

#include "gyroscope.h"

static void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < (us * 8); i++);
}

static uint8_t Gyroscope_ReadRegister(uint8_t reg) {
    uint8_t receivedByte;

    delay_us(10);  // Delay before CS low
    GPIO_WriteToOutputPin(GPIOE, 3, 0);  // CS low
    delay_us(10);  // Delay after CS low

    SPI_TransmitReceive(SPI1, reg | 0x80);  // Send address with read bit
    delay_us(10);  // Wait after address

    receivedByte = SPI_TransmitReceive(SPI1, 0x00);  // Read data

    delay_us(10);  // Delay before CS high
    GPIO_WriteToOutputPin(GPIOE, 3, 1);  // CS high
    delay_us(10);  // Delay after CS high

    return receivedByte;
}


static void Gyroscope_WriteRegister(uint8_t reg, uint8_t value) {
    delay_us(10);
    GPIO_WriteToOutputPin(GPIOE, 3, 0);  // CS low
    delay_us(10);

    SPI_TransmitReceive(SPI1, reg);  // Send address
    delay_us(10);

    SPI_TransmitReceive(SPI1, value);  // Send value
    delay_us(10);

    GPIO_WriteToOutputPin(GPIOE, 3, 1);  // CS high
    delay_us(10);
}

uint8_t Gyroscope_Init(void) {
     // Initialize CS pin
    GPIO_Handle_t gpio_cs;
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;  // Enable GPIOE clock

    gpio_cs.pGPIOx = GPIOE;
    gpio_cs.GPIO_PinConfig.GPIO_PinNumber = 3;
    gpio_cs.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    gpio_cs.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;
    gpio_cs.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio_cs.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GPIO_Init(&gpio_cs);

    GPIO_WriteToOutputPin(GPIOE, 3, 1);  // CS high initially
    delay_us(100);

    // Verify device ID
    uint8_t whoami = Gyroscope_ReadRegister(L3GD20_WHO_AM_I);
    if(whoami != L3GD20_WHO_AM_I_VALUE) {
        return whoami;
    }

    // Configure the gyroscope
    // Power down first
    Gyroscope_WriteRegister(L3GD20_CTRL_REG1, 0x00);
    delay_us(100);

    // Configure registers
    Gyroscope_WriteRegister(L3GD20_CTRL_REG1, 0x0F);  // Normal mode, all axes enabled
    delay_us(100);

    Gyroscope_WriteRegister(L3GD20_CTRL_REG4, 0x00);  // 250 dps, normal mode
    delay_us(100);

    return 0;
}

void Gyroscope_ReadData(int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t xl = Gyroscope_ReadRegister(L3GD20_OUT_X_L);
    uint8_t xh = Gyroscope_ReadRegister(L3GD20_OUT_X_H);
    uint8_t yl = Gyroscope_ReadRegister(L3GD20_OUT_Y_L);
    uint8_t yh = Gyroscope_ReadRegister(L3GD20_OUT_Y_H);
    uint8_t zl = Gyroscope_ReadRegister(L3GD20_OUT_Z_L);
    uint8_t zh = Gyroscope_ReadRegister(L3GD20_OUT_Z_H);

    *gx = (int16_t)(xh << 8 | xl);
    *gy = (int16_t)(yh << 8 | yl);
    *gz = (int16_t)(zh << 8 | zl);
}