#include <stddef.h>
#include "stm32f0xx.h"
#include <stdlib.h>
#include <stdio.h>
#include "i2c.h"

int main(void)
{
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	
	
	i2c_init();
	i2c_read(0x38, 0x0D, 6);
	
	while(1)
	{
		GPIOB->ODR ^= GPIO_ODR_7;
		
		for(int i = 0; i < 2000000; i++);
		
	}
	
}
