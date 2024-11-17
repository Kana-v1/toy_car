//
// Created by kana on 10/26/2024.
//
#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f303xc.h"
#include <stdbool.h>


typedef struct {
    I2C_TypeDef *pI2Cx;
} I2C_Handle_t;

// Clock enable macros
#define I2C1_PCLK_EN()    (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C2_PCLK_EN()    (RCC->APB1ENR |= RCC_APB1ENR_I2C2EN)

void I2C_Init(I2C_Handle_t *pI2CHandle);
uint8_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t len, uint8_t slaveAddr);
uint8_t I2C_WriteReg(I2C_Handle_t *pI2CHandle, uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint32_t len);
uint8_t I2C_ReadReg(I2C_Handle_t *pI2CHandle, uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint32_t len);


#endif //TOY_CAR_I2C_H
