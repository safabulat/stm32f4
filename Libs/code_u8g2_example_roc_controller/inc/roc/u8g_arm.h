/*
 * u8g_arm.h
 *
 *  Created on: 16.03.2019
 *      Author: Johannes Berndorfer (berndoJ)
 */

#ifndef INC_ROC_U8G_ARM_H_
#define INC_ROC_U8G_ARM_H_

#include "lib/u8g_v2/u8g2.h"
#include "sys/_stdint.h"
#include "stm32f0xx_hal.h"

#define DISPLAY_SPI_TIMEOUT_MS 10

//#define __U8G_ARM_DBG__

void u8g_hw_port_init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port,
		uint16_t cs_pin, GPIO_TypeDef* rst_port, uint16_t rst_pin,
		GPIO_TypeDef* dc_port, uint16_t dc_pin,
		GPIO_TypeDef* spi_sck_port, uint16_t spi_sck_pin,
		GPIO_TypeDef* spi_mosi_port, uint16_t spi_mosi_pin);

uint8_t u8g_hw_port_gpio_delay_cb(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8g_hw_port_byte_cb(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int, void* arg_ptr);

void u8g_hw_port_delay_ns(uint8_t ns);
void u8g_hw_port_delay_100ns(uint8_t ns);
void u8g_hw_port_delay_10us(uint8_t ns);

#endif /* INC_ROC_U8G_ARM_H_ */
