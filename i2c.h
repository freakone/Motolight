#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"

void i2c_init();
void i2c_read(uint8_t device, uint8_t reg, uint8_t bytes);
void i2c_single_write(uint8_t device, uint8_t reg, uint8_t byte);
void i2c_write(uint8_t device, uint8_t reg, uint8_t bytes);
void i2c_wait();
void I2C2_IRQHandler(void);