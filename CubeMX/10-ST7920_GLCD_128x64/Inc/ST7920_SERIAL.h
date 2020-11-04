#ifndef ST7920_SERIAL_H
#define ST7920_SERIAL_H


#include "stdint.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

extern uint8_t image[(128 * 64)/8];

//	SPI init
void ST7920_SPI_Init(SPI_HandleTypeDef *spiLcdHandle);

// SPI TRANSMIT !
void SendByte_SPII(uint8_t bytt);

// A replacement for SPI_TRANSMIT
void SendByteSPI(uint8_t byte);

// Send the command to the LCD via SPI
void SendCommand_SPI(uint8_t com);
// Send the data to the LCD via SPI
void SendData_SPI(uint8_t data);
// Send the command to the LCD
void ST7920_SendCmd (uint8_t cmd);


// send the data to the LCD
void ST7920_SendData (uint8_t data);


/* send the string to the LCD
 * 'row' = starting ROW for the string (from 0 to 3)
 * 'col' = starting COL for the string (from 0 to 7)
 */
void ST7920_SendString(int row, int col, char* string);


/* ENABLE or DISABLE the graphic mode
 * enable =1 --> graphic mode enabled
 */
void ST7920_GraphicMode (int enable);


// clear screen in any mode
void ST7920_Clear(void);


// Draw bitmap on the display
void ST7920_DrawBitmap(const unsigned char* graphic);



// Update the display with the selected graphics
void ST7920_Update(void);



// Initialize the display
void ST7920_Init (void);


/* Common functions used
 * in other LCDs also
 */


// Set a pixel on the display
void SetPixel(uint8_t x, uint8_t y);



// draw line from (X0, Y0) to (X1, Y1)
void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);


// draw rectangle from (X,Y) w- width, h- height
void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



// draw filled rectangle
void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



// draw circle with centre (X0, Y0) and radius= radius
void DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius);



// Draw Filled Circle with centre (X0, Y0) and radius= r

void DrawFilledCircle(int16_t x0, int16_t y0, int16_t r);



// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);



// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);

//LCD dimensions defines
#define ILI9341_WIDTH       240
#define ILI9341_HEIGHT      320
#define ILI9341_PIXEL_COUNT	ILI9341_WIDTH * ILI9341_HEIGHT
//ILI9341 LCD commands
#define ILI9341_RESET			 		    	0x01
#define ILI9341_SLEEP_OUT		  			0x11
#define ILI9341_GAMMA			    			0x26
#define ILI9341_DISPLAY_OFF					0x28
#define ILI9341_DISPLAY_ON					0x29
#define ILI9341_COLUMN_ADDR					0x2A
#define ILI9341_PAGE_ADDR			  		0x2B
#define ILI9341_GRAM				    		0x2C
#define ILI9341_TEARING_OFF					0x34
#define ILI9341_TEARING_ON					0x35
#define ILI9341_DISPLAY_INVERSION		0xb4
#define ILI9341_MAC			        		0x36
#define ILI9341_PIXEL_FORMAT    		0x3A
#define ILI9341_WDB			    	  		0x51
#define ILI9341_WCD				      		0x53
#define ILI9341_RGB_INTERFACE   		0xB0
#define ILI9341_FRC					    	0xB1
#define ILI9341_BPC					    	0xB5
#define ILI9341_DFC				 	    	0xB6
#define ILI9341_Entry_Mode_Set		0xB7
#define ILI9341_POWER1						0xC0
#define ILI9341_POWER2						0xC1
#define ILI9341_VCOM1							0xC5
#define ILI9341_VCOM2							0xC7
#define ILI9341_POWERA						0xCB
#define ILI9341_POWERB						0xCF
#define ILI9341_PGAMMA						0xE0
#define ILI9341_NGAMMA						0xE1
#define ILI9341_DTCA							0xE8
#define ILI9341_DTCB							0xEA
#define ILI9341_POWER_SEQ					0xED
#define ILI9341_3GAMMA_EN					0xF2
#define ILI9341_INTERFACE					0xF6
#define ILI9341_PRC				   	  	0xF7
#define ILI9341_VERTICAL_SCROLL 	0x33

#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

#endif

