/*
Library:					SPI GLCD - ST7920
Written by:				Safa BULAT
Date Written:			21/08/2020
Last modified:		-/-
Description:			This is an STM32 device driver library for the ST7920 SPI gLCD display, using STM HAL libraries
										
*/
//Header files
#include "GLCD_MY.h"

//SPI handler
static SPI_HandleTypeDef lcdSPIhandle;
//Chip Select pin
static GPIO_TypeDef  *CS_GPIO;
static uint16_t CS_PIN;


//***** Functions *****//
//1. Write Command to LCD
void GLCD_SendCommand(uint8_t cmd)
{
	//*(__IO uint8_t *)(0x60000000) = cmd;
	uint8_t tmpCmd = cmd;
	//Bring CS HIGH
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpCmd, 1, 5);
	HAL_Delay(100);
	//Put CS LOW
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);

}

//2. Write data to LCD
void GLCD_SendData(uint8_t data)
{
	//*(__IO uint8_t *)(0x60040000) = data;
	uint8_t tmpData = data;
	//Bring CS HIGH
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpData, 1, 5);
	HAL_Delay(100);
	//Put CS LOW
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);
}
//3. Lib INIT
void GLCD_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN)
{
	//Copy SPI settings
	memcpy(&lcdSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	//CS pin
	CS_GPIO = csPORT;
	CS_PIN = csPIN;
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	
	GLCD_SendCommand(0x30);  // 8bit mode
	HAL_Delay(110);  //  >100us delay

	GLCD_SendCommand(0x01);  // clear screen
	HAL_Delay(12);  // >10 ms delay

	GLCD_SendCommand(0x06);  // cursor increment right no shift
	HAL_Delay(1);  // 1ms delay

	GLCD_SendCommand(0x0C);  // D=1, C=0, B=0
    HAL_Delay(1);  // 1ms delay

	GLCD_SendCommand(0x02);  // return to home
	HAL_Delay(1);  // 1ms delay
	
	GLCD_SendData(0x01);
	HAL_Delay(1); 
}
//4. Text printing functions
void GLCD_printText(char* string, int16_t col, int16_t row)
{
	switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0x90;
            break;
        case 2:
            col |= 0x88;
            break;
        case 3:
            col |= 0x98;
            break;
        default:
            col |= 0x80;
            break;
    }
		//col = row + col;
		GLCD_SendCommand(col);
		GLCD_SendData(*string);
		
    while (*string)
    	{
				GLCD_SendData(*string++);			
    	}
}

//5. Set Cursor Position
void GLCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  GLCD_SendCommand (0x80);
  GLCD_SendData(x1>>8);
  GLCD_SendData(x1 & 0xFF);
  GLCD_SendData(x2>>8);
  GLCD_SendData(x2 & 0xFF);

  GLCD_SendCommand (0x2B);
  GLCD_SendData(y1>>8);
  GLCD_SendData(y1 & 0xFF);
  GLCD_SendData(y2>>8);
  GLCD_SendData(y2 & 0xFF);
  //GLCD_SendCommand (SET_CGRAM_ADDRESS);
	GLCD_SendCommand (0x2C);
}

//6. Write data to a single pixel
void GLCD_LightPixel(uint16_t x, uint16_t y) 
{
	GLCD_SetCursorPosition(x, y, x, y);
	GLCD_SendData(0x09);
}
//6. LCD char print
void GLCD_print(char *chaer, int16_t add)
{
	GLCD_SendCommand(add);
	GLCD_SendData(*chaer);
}
//7. gLCD Clear
void GLCD_clear()
{
	GLCD_SendCommand (DISPLAY_CLEAR);
	HAL_Delay(5);
//	GLCD_SendData(DISPLAY_CLEAR);
//	HAL_Delay(5);
}

//*****Endlist

