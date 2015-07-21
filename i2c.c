#include "i2c.h"

uint8_t i2c_bytes[50];

void i2c_init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_DMA1EN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	GPIOB->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11;
	GPIOB->AFR[1] |= 0x00001100;
	
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
	
	I2C2->CR1 &= ~I2C_CR1_PE;
	I2C2->CR1 |= I2C_CR1_SWRST;
	I2C2->CR1 &= ~I2C_CR1_SWRST;
	I2C2->CR1 |= I2C_CR1_ANFOFF;
	
	uint32_t SCLL  = 0x09		;
	uint32_t SCLH  = 0x0300 		;
	uint32_t SDADEL= 0x030000 	;
	uint32_t SCLDEL= 0x0300000 	;
	uint32_t PRESC = 0xB0000000   ;	
	I2C2->TIMINGR |=  SCLL | SCLH | SDADEL | SCLDEL | PRESC;
	
	I2C2->TIMEOUTR |= 0x05;
	I2C2->TIMEOUTR |= I2C_TIMEOUTR_TIMOUTEN | I2C_TIMEOUTR_TIDLE;
	
	I2C2->CR1 |= I2C_CR1_TXIE;
	I2C2->CR1 |= I2C_CR1_RXIE;
	I2C2->CR1 |= I2C_CR1_ERRIE;
	I2C2->CR1 |= I2C_CR1_TCIE;

	I2C2->CR1 |= I2C_CR1_PE;// enable peripheral
}

uint8_t read_after_transfer = 0;
uint8_t read_counter = 0;
void I2C2_IRQHandler(void)
{
	
	if(I2C2->ISR & I2C_ISR_RXNE)
	{		
		i2c_bytes[read_counter++] = I2C2->RXDR;	
	}
	
	if(I2C2->ISR & I2C_ISR_TXE && read_after_transfer > 0)
	{			
		I2C2->CR2 &= ~(0xFF<<16);
		I2C2->CR2 |= (read_after_transfer<<16); //num bytes to read
		I2C2->CR2 |= I2C_CR2_RD_WRN;
		I2C2->CR2 |= I2C_CR2_START;	

		read_after_transfer = 0;		
	}
	
	if(I2C2->ISR & I2C_ISR_TC)
	{	
		I2C2->CR2 |= I2C_CR2_STOP;
	}
	
	if(I2C2->ISR & I2C_ISR_BERR)
	{

		I2C2->ICR |= I2C_ICR_BERRCF;
	}		
}

void i2c_read(uint8_t device, uint8_t reg, uint8_t bytes)
{
		I2C2->CR2 = 0;
		I2C2->CR2 |= device;
		I2C2->CR2 |= (1<<16);
		I2C2->TXDR = reg;
		I2C2->CR2 |= I2C_CR2_START;		
		read_after_transfer = bytes;	
		read_counter = 0;		
}

