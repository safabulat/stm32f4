/*
Library:					SPI GLCD - ST7920
Written by:				Safa BULAT
Date Written:			21/08/2020
Last modified:		-/-
Description:			This is an STM32 device driver library for the ST7920 SPI gLCD display, using STM HAL libraries
										
*/
#ifndef GLCD_MY_H
#define GLCD_MY_H

//***** List of includes *****//
#include <stdbool.h>
//****MCU****
//** CHANGE BASED ON STM32 CHIP F4/F7/F1...**//
#include "stm32f4xx_hal.h"   
//****Libs****
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//*****Endlist

//***** Parameters of GLCD *****//
#define GLCD_TYPE GLCD_TYPE_ST7920
#define GLCD_IO 8
#define GLCD_WIDTH 128
#define GLCD_HEIGHT 160
#define GLCDFontWidth 6
//*****Endlist	

//***** Functions prototypes *****//
//1. Write Command to LCD
void GLCD_SendCommand(uint8_t cmd);
//2. Write data to LCD
void GLCD_SendData(uint8_t data);
//3. Lib INIT
void GLCD_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN);
//4. Text printing functions
void GLCD_printText(char text[], int16_t col, int16_t row);
//5. Write data to a single pixel
void GLCD_LightPixel(uint16_t x, uint16_t y); 
//6. LCD char print
void GLCD_print(char *chaer, int16_t add);
//7. gLCD Clear
void GLCD_clear(void);
//*****Endlist
//***** ST7920 gLCD commands *****//
// Instruction Set 1: (RE=0: Basic Instruction)
#define DISPLAY_CLEAR           0x01 // Fill DDRAM with "20H" and set DDRAM address counter (AC) to "00H"
#define RETURN_HOME             0x02 // Set DDRAM address counter (AC) to "00H", and put cursor
// to origin &#65533;Gthe content of DDRAM are not changed
#define ENTRY_MODE_SET          0x04 // Set cursor position and display shift when doing write or read
// operation
#define DISPLAY_CONTROL         0x08 // D=1: Display ON, C=1: Cursor ON, B=1: Character Blink ON
#define DISPLAY_ON							0b0000001100
#define CURSOR_ON								0b0000001010
#define CHARBLINK_ON						0b0000001001
#define CURSOR_DISPLAY_CONTROL  10 // Cursor position and display shift control; the content of
// DDRAM are not changed
#define FUNCTION_SET            0x20 // DL=1 8-bit interface, DL=0 4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_CGRAM_ADDRESS       0x40 // Set CGRAM address to address counter (AC)
// Make sure that in extended instruction SR=0
#define SET_DDRAM_ADDRESS       0x80 // Set DDRAM address to address counter (AC)
// AC6 is fixed to 0
 
// Instruction set 2: (RE=1: extended instruction)
#define STANDBY                 0x01 // Enter standby mode, any other instruction can terminate.
// COM1&#65533;c32 are halted
#define SCROLL_OR_RAM_ADDR_SEL  0x02 // SR=1: enable vertical scroll position
// SR=0: enable CGRAM address (basic instruction)
#define REVERSE_BY_LINE         0x04 // Select 1 out of 4 line (in DDRAM) and decide whether to
// reverse the display by toggling this instruction
// R1,R0 initial value is 0,0
#define EXTENDED_FUNCTION_SET   0x20 // DL=1 :8-bit interface, DL=0 :4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_SCROLL_ADDRESS      0x40 // G=1 :graphic display ON, G=0 :graphic display OFF
#define SET_GRAPHIC_RAM_ADDRESS 0x80 // Set GDRAM address to address counter (AC)
// Set the vertical address first and followed the horizontal
// address by consecutive writings
// Vertical address range: AC5&#65533;cAC0, Horizontal address range: AC3&#65533;cAC0
 
// Parameters regarding Instruction Sets 1 & 2
#define DISPLAY_SHIFT_S         0x01 // Set 1, ENTRY_MODE_SET
#define INCREASE_DECREASE_ID    0x02 // Set 1, ENTRY_MODE_SET
#define CURSOR_BLINK_ON_B       0x01 // Set 1, DISPLAY_CONTROL
#define CURSOR_ON_C             0x02 // Set 1, DISPLAY_CONTROL
#define DISPLAY_ON_D            0x04 // Set 1, DISPLAY_CONTROL
#define SHIFT_RL                0x04 // Set 1, CURSOR_DISPLAY_CONTROL
#define CURSOR_SC               0x08 // Set 1, CURSOR_DISPLAY_CONTROL
#define EXTENDED_INSTRUCTION_RE 0x04 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define DATA_LENGTH_DL          0x10 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define REVERSE_BY_LINE_R0      0x01 // Set 2, REVERSE_BY_LINE
#define REVERSE_BY_LINE_R1      0x02 // Set 2, REVERSE_BY_LINE
#define EN_VERTICAL_SCROLL_SR   0x01 // Set 2, SCROLL_OR_RAM_ADDR_SEL
#define GRAPHIC_ON_G            0x02 // Set 2, EXTENDED_FUNTION_SET
 
#define BUSY_FLAG_BF            0x80
//*****Endlist

#endif

