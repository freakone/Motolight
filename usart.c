#include "usart.h"

void usart_init()
{
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
		GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
		GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10);
		GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10;
		GPIOA->AFR[1] |= 0x00000110;
	
	
		USART1->BRR |= 417;
	
		USART1->CR1 |= USART_CR1_RXNEIE;
		USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
		USART1->CR1 |= USART_CR1_UE;
}

void usart_write(uint8_t data)
{
		USART1->TDR = data;
		while(!(USART1->ISR & USART_ISR_TXE));
}