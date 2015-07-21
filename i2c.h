#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"

void i2c_init();
void i2c_read(uint8_t device, uint8_t reg, uint8_t bytes);
void I2C2_IRQHandler(void);