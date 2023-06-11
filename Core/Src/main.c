#include "drivers_general.h"


uint8_t main(void) {
    GPIO_Handle_t btn, led;


    memset(&btn, 0, sizeof(btn));
    memset(&led, 0, sizeof(led));

    led.pGPIOx = GPIOE;
    led.GPIO_PinConfig.GPIO_PinNumber = 11;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_OUTPUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&led);

    btn.pGPIOx = GPIOA;
    btn.GPIO_PinConfig.GPIO_PinNumber = 0;
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_PIN_MODE_INPUT;
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PD;
    btn.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUTPUT_TYPE_PUSH_PULL;

    GPIO_Init(&btn);

    while (!GPIO_ReadFromInputPin(btn.pGPIOx, btn.GPIO_PinConfig.GPIO_PinNumber));

    GPIO_ToggleOutputPin(led.pGPIOx, led.GPIO_PinConfig.GPIO_PinNumber);

    while(1);

    return 0;
}