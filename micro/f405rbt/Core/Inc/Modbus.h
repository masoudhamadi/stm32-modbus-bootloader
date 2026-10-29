
#ifndef MODBUS_H
#define MODBUS_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

#define MODBUS_SLAVE_ID  1U


/* Set RS485 direction control if required */
#define MODBUS_USE_DE  1U

#if MODBUS_USE_DE
#define MODBUS_DE_PORT  GPIOC
#define MODBUS_DE_PIN   LL_GPIO_PIN_12
#endif


/* Initialization and polling */
void Modbus_Init(void);
void FrameHandler(void);


/* Coil/register access */
uint16_t mb_read_register(uint8_t db, int16_t address);
void     mb_write_register(uint8_t db, int16_t address, const uint16_t *data, uint16_t len);

#endif
