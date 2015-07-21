#include "i2c.h"

uint8_t i2c_bytes_read[50];
uint8_t i2c_bytes_write[50];

void i2c_init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
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
uint8_t write_counter = 0;
uint8_t stop = 0;
void I2C2_IRQHandler(void)
{
	
	if(I2C2->ISR & I2C_ISR_RXNE)
	{		
		i2c_bytes_read[read_counter++] = I2C2->RXDR;	
	}
	
	if(I2C2->ISR & I2C_ISR_TXE)
	{					
		if(read_after_transfer > 0)
		{
			I2C2->CR2 &= ~(0xFF<<16);
			I2C2->CR2 |= (read_after_transfer<<16); //num bytes to read
			I2C2->CR2 |= I2C_CR2_RD_WRN;
			I2C2->CR2 |= I2C_CR2_START;	

			read_after_transfer = 0;	
		}
	}
	
	if(I2C2->ISR & I2C_ISR_TXIS)
	{			
		I2C2->TXDR = i2c_bytes_write[write_counter++];
	}
	
	if(I2C2->ISR & I2C_ISR_TC)
	{	
		I2C2->CR2 |= I2C_CR2_STOP;
		stop = 1;
	}
	
	if(I2C2->ISR & I2C_ISR_BERR)
	{

		I2C2->ICR |= I2C_ICR_BERRCF;
	}		
}

void i2c_wait()
{
		while(stop == 0);
		stop = 0;	
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

void i2c_single_write(uint8_t device, uint8_t reg, uint8_t byte)
{
		i2c_bytes_write[0] = byte;
		I2C2->CR2 = 0;
		I2C2->CR2 |= device;
		I2C2->CR2 |= (2<<16);
		I2C2->TXDR = reg;
		I2C2->CR2 |= I2C_CR2_START;			
		write_counter = 0;		

}

void i2c_write(uint8_t device, uint8_t reg, uint8_t bytes)
{
		I2C2->CR2 = 0;
		I2C2->CR2 |= device;
		I2C2->CR2 |= ((bytes+1)<<16);
		I2C2->TXDR = reg;
		I2C2->CR2 |= I2C_CR2_START;			
		write_counter = 0;		
}

