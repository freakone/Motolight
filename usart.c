#include "usart.h"

void usart_init()
{
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
}