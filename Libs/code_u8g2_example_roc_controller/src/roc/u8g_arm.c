/*
 * u8g_arm.c
 *
 *  Created on: 16.03.2019
 *      Author: Johannes Berndorfer (berndoJ)
 *
 *  Hardware port of the ARM-version of the u8g2-library. Include u8g_arm.h for use of the u8g2 library.
 *
 */

#include "roc/u8g_arm.h"
#include "lib/u8g_v2/u8g2.h"
#include "roc/debug.h"
#include "stm32f0xx_hal.h"

#define U8X8_MSG_DELAY_MILLI_DELAY_FUNC_TYPE 0x01 // 0x00: osDelay (FreeRTOS), 0x01: HAL_Delay;
#define DISPLAY_SPI_WAIT_FOR_TX_CPLT_DELAY_FUNC_TYPE 0x01 // 0x00: osDelay (FreeRTOS), 0x01: HAL_Delay;

uint8_t spi_transmission_cplt = 0x01;

SPI_HandleTypeDef* display_spi_handle;

GPIO_TypeDef* display_cs_port;
uint16_t display_cs_pin;

GPIO_TypeDef* display_dc_port;
uint16_t display_dc_pin;

GPIO_TypeDef* display_rst_port;
uint16_t display_rst_pin;

GPIO_TypeDef* display_spi_sck_port;
uint16_t display_spi_sck_pin;

GPIO_TypeDef* display_spi_mosi_port;
uint16_t display_spi_mosi_pin;

uint8_t display_mode_byte = 0x00;

void u8g_hw_port_init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port,
		uint16_t cs_pin, GPIO_TypeDef* rst_port, uint16_t rst_pin,
		GPIO_TypeDef* dc_port, uint16_t dc_pin, GPIO_TypeDef* spi_sck_port,
		uint16_t spi_sck_pin, GPIO_TypeDef* spi_mosi_port,
		uint16_t spi_mosi_pin)
{
	display_spi_handle = hspi;
	display_cs_port = cs_port;
	display_cs_pin = cs_pin;
	display_dc_port = dc_port;
	display_dc_pin = dc_pin;
	display_spi_sck_port = spi_sck_port;
	display_spi_sck_pin = spi_sck_pin;
	display_spi_mosi_port = spi_mosi_port;
	display_spi_mosi_pin = spi_mosi_pin;
	display_rst_port = rst_port;
	display_rst_pin = rst_pin;
}

// GPIO and delay callback function / HAL wrapper for u8glib v2.
uint8_t u8g_hw_port_gpio_delay_cb(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int,
		void* arg_ptr)
{
	switch (msg)
	{
	case U8X8_MSG_GPIO_AND_DELAY_INIT: // Init message.
		// Used for initialisation of GPIO. Not used here.
		break;
	case U8X8_MSG_DELAY_NANO:
		u8g_hw_port_delay_ns(arg_int);
		break;
	case U8X8_MSG_DELAY_100NANO:
		u8g_hw_port_delay_100ns(arg_int);
		break;
	case U8X8_MSG_DELAY_10MICRO:
		u8g_hw_port_delay_10us(arg_int);
		break;
	case U8X8_MSG_DELAY_MILLI: // Delay a given amount of milliseconds.
		if (U8X8_MSG_DELAY_MILLI_DELAY_FUNC_TYPE == 0x00)
			osDelay(arg_int);
		else
			HAL_Delay(arg_int);
		break;
	case U8X8_MSG_DELAY_I2C:
		// Not used due to SPI?
		break;
	case U8X8_MSG_GPIO_SPI_CLOCK: // Write to the SCLK pin.
		HAL_GPIO_WritePin(display_spi_sck_port, display_spi_sck_pin, arg_int);
		break;
	case U8X8_MSG_GPIO_SPI_DATA: // Write to the MOSI pin.
		HAL_GPIO_WritePin(display_spi_mosi_port, display_spi_mosi_pin, arg_int);
		break;
	case U8X8_MSG_GPIO_CS: // Write to the CS pin of the LCD.
		HAL_GPIO_WritePin(display_cs_port, display_cs_pin, arg_int);
		break;
	case U8X8_MSG_GPIO_DC: // Write to the DC / RS pin of the LCD.
		HAL_GPIO_WritePin(display_dc_port, display_dc_pin, arg_int);
		break;
	case U8X8_MSG_GPIO_RESET:
		HAL_GPIO_WritePin(display_rst_port, display_rst_pin, arg_int);
		break;
	default:
		u8x8_SetGPIOResult(u8g2, 1); // Default return value.
		break;
	}
	return 1;
}

uint8_t u8g_hw_port_byte_cb(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int,
		void* arg_ptr)
{
	uint8_t* data_ptr;
	switch (msg)
	{
	case U8X8_MSG_BYTE_INIT:
		// Init.
		u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_disable_level);
		break;
	case U8X8_MSG_BYTE_SET_DC:
		u8x8_gpio_SetDC(u8g2, arg_int); // Pass the new set value of DC (in arg_int) to the gpio function.
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_enable_level);
		u8g2->gpio_and_delay_cb(u8g2, U8X8_MSG_DELAY_NANO, u8g2->display_info->post_chip_enable_wait_ns, NULL);
		break;
	case U8X8_MSG_BYTE_SEND:
		data_ptr = (uint8_t*) arg_ptr;
		/*while (spi_transmission_cplt == 0x00)
		{
			if (U8X8_MSG_DELAY_MILLI_DELAY_FUNC_TYPE == 0x00)
				osDelay(arg_int);
			else
				HAL_Delay(arg_int);
			break;
		}
		spi_transmission_cplt = 0x00; // Occupy the spi transmission line.*/
		/*char dbg_data[8];
		sprintf(dbg_data, "%x", arg_int);
		debug_cons_print("S-");
		debug_cons_println(dbg_data);
		for (uint8_t i = 0; i < arg_int; i++)
		{
			sprintf(dbg_data, "%x", *(data_ptr+i));
			debug_cons_print("D-");
			debug_cons_println(dbg_data);
		}*/
		HAL_SPI_Transmit(display_spi_handle, data_ptr, arg_int, 10);
	case U8X8_MSG_BYTE_END_TRANSFER:
		u8g2->gpio_and_delay_cb(u8g2, U8X8_MSG_DELAY_NANO, u8g2->display_info->pre_chip_disable_wait_ns, NULL);
		u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_disable_level);
		break;
	default:
		return 0; // Return 0 if the message is not implemented (as specified in porting guide)
	}
	return 1;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
	spi_transmission_cplt = 0x01;
}

void u8g_hw_port_delay_ns(uint8_t ns)
{
	// Core @72 MHZ: 14ns per instruction.
	// __NOP(); is direct "nop;" instruction to cpu.
	// Divide ns / 28 (extra instruction for jump back to beginning of the loop) for loop cycles.
	for (uint8_t i = 0; i < (ns / 28); i++)
	{
		__NOP();
	}
}

void u8g_hw_port_delay_100ns(uint8_t ns)
{
	// Same as in u8g_hw_port_delay_ns function.
	// 100 / 28 = 3.57;
	for (uint16_t i = 0; i < (ns * 3.57); i++)
	{
		__NOP();
	}
}

void u8g_hw_port_delay_10us(uint8_t us)
{
	// Same as in u8g_hw_port_delay_ns function.
	// 3.57 * 100 ? 357;
	for (uint16_t i = 0; i < (us * 357); i++)
	{
		__NOP();
	}
}