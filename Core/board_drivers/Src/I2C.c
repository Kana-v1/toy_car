//
// Created by kana on 10/26/2024.
//
#include "I2C.h"
#include "gpio.h"


// Debug function to check I2C line state
static uint8_t I2C_CheckLines(void) {
    // Read actual pin states of SCL and SDA
    uint8_t scl_state = (GPIOB->IDR & GPIO_IDR_6) != 0;
    uint8_t sda_state = (GPIOB->IDR & GPIO_IDR_7) != 0;

    // Both lines should be high due to pull-ups
    return (scl_state && sda_state);
}

void I2C_Init(I2C_Handle_t *pI2CHandle) {
    // Enable clocks first
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Force reset I2C peripheral
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    for(volatile int i = 0; i < 100; i++);
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Disable I2C before configuration
    pI2CHandle->pI2Cx->CR1 &= ~I2C_CR1_PE;

    // Configure GPIO first
    // PB6 - SCL, PB7 - SDA
    // Start with them as inputs to check line state
    GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);

    // Set pull-ups
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);

    // Check if lines are high
    if(!I2C_CheckLines()) {
        // Lines are stuck low, try recovery
        // Set as outputs temporarily
        GPIOB->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
        GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);

        // Toggle clock until SDA is released
        for(int i = 0; i < 16; i++) {
            GPIOB->ODR &= ~GPIO_ODR_6;  // SCL low
            for(volatile int j = 0; j < 100; j++);
            GPIOB->ODR |= GPIO_ODR_6;   // SCL high
            for(volatile int j = 0; j < 100; j++);
        }

        // Generate STOP condition
        GPIOB->ODR &= ~GPIO_ODR_7;  // SDA low
        for(volatile int j = 0; j < 100; j++);
        GPIOB->ODR |= GPIO_ODR_7;   // SDA high

        // Back to input mode
        GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    }

    // Now configure for I2C
    GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);  // AF mode
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7);
    GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFRL6_Pos) | (4 << GPIO_AFRL_AFRL7_Pos);
    GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);

    // Configure I2C timing for standard mode (100kHz)
    // These values are for 8MHz clock
    pI2CHandle->pI2Cx->TIMINGR = 0;
    pI2CHandle->pI2Cx->TIMINGR = (3 << 28) |   // PRESC
                                 (0x13 << 0) |   // SCLL
                                 (0xF << 8) |    // SCLH
                                 (0x2 << 20) |   // SDADEL
                                 (0x4 << 16);    // SCLDEL

    // Enable analog filter, disable digital filter
    pI2CHandle->pI2Cx->CR1 &= ~I2C_CR1_ANFOFF;
    pI2CHandle->pI2Cx->CR1 &= ~I2C_CR1_DNF;

    // Enable error interrupts
    pI2CHandle->pI2Cx->CR1 |= I2C_CR1_ERRIE;

    // Clear all error flags
    pI2CHandle->pI2Cx->ICR = 0xFFFFFFFF;

    // Finally enable I2C
    pI2CHandle->pI2Cx->CR1 |= I2C_CR1_PE;

    // Wait for peripheral to be ready
    while((pI2CHandle->pI2Cx->CR1 & I2C_CR1_PE) == 0);
}

uint8_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr, uint8_t Sr) {
    uint32_t timeout = 10000;

    // Wait until BUSY flag is cleared
    while(pI2CHandle->pI2Cx->ISR & I2C_ISR_BUSY && timeout--);
    if(timeout == 0) return 1;

    // Clear CR2 register
    pI2CHandle->pI2Cx->CR2 = 0;

    // Configure transfer
    uint32_t tmpreg = 0;
    tmpreg |= ((slaveAddr << 1) & I2C_CR2_SADD);    // Slave address
    tmpreg |= ((len & 0xFF) << 16);                 // Number of bytes
    tmpreg |= (Sr ? 0 : I2C_CR2_AUTOEND);          // Auto end mode

    pI2CHandle->pI2Cx->CR2 = tmpreg;

    // Generate START condition
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_START;

    // Send each byte
    for(uint32_t i = 0; i < len; i++) {
        timeout = 10000;

        // Wait for TXIS flag
        while(!(pI2CHandle->pI2Cx->ISR & I2C_ISR_TXIS) && timeout--) {
            if(pI2CHandle->pI2Cx->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR)) {
                return 2;
            }
        }
        if(timeout == 0) return 3;

        // Send byte
        pI2CHandle->pI2Cx->TXDR = pTxBuffer[i];
    }

    // Wait for transfer complete
    timeout = 10000;
    while(!(pI2CHandle->pI2Cx->ISR & I2C_ISR_TC) && timeout--) {
        if(pI2CHandle->pI2Cx->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR)) {
            return 4;
        }
    }
    if(timeout == 0) return 5;

    return 0;
}

uint8_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t len, uint8_t slaveAddr) {
    uint32_t timeout;

    // Wait until BUSY flag is cleared
    timeout = 10000;
    while((pI2CHandle->pI2Cx->ISR & I2C_ISR_BUSY) && timeout--);
    if(timeout == 0) return 1;

    // Clear CR2 register
    pI2CHandle->pI2Cx->CR2 = 0;

    // Configure transfer
    pI2CHandle->pI2Cx->CR2 = ((slaveAddr << 1) |          // Slave address
                              (1 << 10) |                  // READ mode
                              ((len & 0xFF) << 16) |       // Number of bytes
                              I2C_CR2_AUTOEND);           // Auto end mode

    // Generate START condition
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_START;

    // Read data
    for(uint32_t i = 0; i < len; i++) {
        // Wait for RXNE flag or error
        timeout = 10000;
        while(!(pI2CHandle->pI2Cx->ISR & I2C_ISR_RXNE) && timeout--) {
            if(pI2CHandle->pI2Cx->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR)) {
                // Clear error flags
                pI2CHandle->pI2Cx->ICR = 0xFFFFFFFF;
                return 2;
            }
        }
        if(timeout == 0) return 3;

        // Read data from RXDR
        pRxBuffer[i] = pI2CHandle->pI2Cx->RXDR;
    }

    // Wait for STOPF flag
    timeout = 10000;
    while(!(pI2CHandle->pI2Cx->ISR & I2C_ISR_STOPF) && timeout--);
    if(timeout == 0) return 4;

    // Clear STOPF flag
    pI2CHandle->pI2Cx->ICR = I2C_ICR_STOPCF;

    return 0;
}

uint8_t I2C_WriteReg(I2C_Handle_t *pI2CHandle, uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint32_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = regAddr;
    for(uint32_t i = 0; i < len; i++) {
        buffer[i + 1] = pData[i];
    }

    return I2C_MasterSendData(pI2CHandle, buffer, len + 1, slaveAddr, 0);
}

uint8_t I2C_ReadReg(I2C_Handle_t *pI2CHandle, uint8_t slaveAddr, uint8_t regAddr, uint8_t *data, uint32_t len) {
    // Send register address
    uint8_t status = I2C_MasterSendData(pI2CHandle, &regAddr, len, slaveAddr, 1);
    if(status != 0) return status;

    // Read data
    return I2C_MasterReceiveData(pI2CHandle, data, len, slaveAddr);
}