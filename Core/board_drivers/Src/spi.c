//
// Created by kana on 11/2/2024.
//

#include "spi.h"

uint8_t SPI_Init(SPI_Handle_t *pSPIHandle) {
    // Enable peripheral clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Initialize GPIO pins
    GPIO_Handle_t gpio_sck, gpio_miso, gpio_mosi;

    // Configure SCK (PA5)
    gpio_sck.pGPIOx = GPIOA;
    gpio_sck.GPIO_PinConfig.GPIO_PinNumber = 5;
    gpio_sck.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_ALT_FN;
    gpio_sck.GPIO_PinConfig.GPIO_PinAltFunNum = 5;
    gpio_sck.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;
    gpio_sck.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio_sck.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GPIO_Init(&gpio_sck);

    // Configure MISO (PA6)
    gpio_miso.pGPIOx = GPIOA;
    gpio_miso.GPIO_PinConfig.GPIO_PinNumber = 6;
    gpio_miso.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_ALT_FN;
    gpio_miso.GPIO_PinConfig.GPIO_PinAltFunNum = 5;
    gpio_miso.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;
    gpio_miso.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GPIO_Init(&gpio_miso);

    // Configure MOSI (PA7)
    gpio_mosi.pGPIOx = GPIOA;
    gpio_mosi.GPIO_PinConfig.GPIO_PinNumber = 7;
    gpio_mosi.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_ALT_FN;
    gpio_mosi.GPIO_PinConfig.GPIO_PinAltFunNum = 5;
    gpio_mosi.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;
    gpio_mosi.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio_mosi.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GPIO_Init(&gpio_mosi);

    // Disable SPI before configuration
    pSPIHandle->pSPIx->CR1 &= ~SPI_CR1_SPE;

    // Reset registers
    pSPIHandle->pSPIx->CR1 = 0;
    pSPIHandle->pSPIx->CR2 = 0;

    // Configure CR2
    pSPIHandle->pSPIx->CR2 = 0;
    pSPIHandle->pSPIx->CR2 |= (0b0111 << SPI_CR2_DS_Pos);  // 8-bit data size
    pSPIHandle->pSPIx->CR2 |= SPI_CR2_FRXTH;   // RXNE event at 8-bit

    // Configure CR1
    uint32_t tempreg = 0;
    tempreg |= SPI_CR1_MSTR;    // Master mode
    tempreg |= SPI_CR1_SSM;     // Software slave management
    tempreg |= SPI_CR1_SSI;     // Internal slave select
    tempreg |= (0b110 << SPI_CR1_BR_Pos);  // fPCLK/256 for slower speed
    tempreg |= SPI_CR1_CPOL;    // Clock polarity = 1
    tempreg |= SPI_CR1_CPHA;    // Clock phase = 1

    pSPIHandle->pSPIx->CR1 = tempreg;

    // Enable SPI
    pSPIHandle->pSPIx->CR1 |= SPI_CR1_SPE;

    return 0;
}

uint8_t SPI_TransmitReceive(SPI_TypeDef *pSPIx, uint8_t data) {
    uint32_t timeout = 0xFFFF;

    // Wait until TXE is set
    while(!(pSPIx->SR & SPI_SR_TXE) && timeout) timeout--;
    if(!timeout) return 0xFF;

    // Send data
    *((__IO uint8_t*)&pSPIx->DR) = data;
    timeout = 0xFFFF;

    // Wait until RXNE is set
    while(!(pSPIx->SR & SPI_SR_RXNE) && timeout) timeout--;
    if(!timeout) return 0xFF;

    // Read received data
    return *((__IO uint8_t*)&pSPIx->DR);
}

// TEST SPI
static void LED_Init(void) {
    GPIO_Handle_t gpio_led;

    // Enable GPIOE clock
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

    // Configure PE8 (Blue LED on STM32F3 Discovery)
    gpio_led.pGPIOx = GPIOE;
    gpio_led.GPIO_PinConfig.GPIO_PinNumber = 8;
    gpio_led.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    gpio_led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;
    gpio_led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio_led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    GPIO_Init(&gpio_led);
}

void SPI_Test(void) {
    LED_Init();
    GPIO_WriteToOutputPin(GPIOE, 8, 0);  // LED off initially

    // Initialize SPI
    SPI_Handle_t spiHandle;
    spiHandle.pSPIx = SPI1;
    SPI_Init(&spiHandle);

    // Wait a bit after initialization
    for(volatile uint32_t i = 0; i < 1000; i++);

    // Test data
    uint8_t testByte = 0xA5;
    uint8_t receivedByte;

    // Send test pattern
    receivedByte = SPI_TransmitReceive(SPI1, testByte);

    // Store values for debug
    volatile uint32_t status = SPI1->SR;
    volatile uint8_t sent = testByte;
    volatile uint8_t received = receivedByte;

    // Check result
    if(testByte == receivedByte) {
        GPIO_WriteToOutputPin(GPIOE, 8, 1);  // Success
    } else {
        // Error - blink LED
        for(int i = 0; i < 3; i++) {
            GPIO_WriteToOutputPin(GPIOE, 8, 1);
            for(volatile uint32_t j = 0; j < 100000; j++);
            GPIO_WriteToOutputPin(GPIOE, 8, 0);
            for(volatile uint32_t j = 0; j < 100000; j++);
        }
    }
}


