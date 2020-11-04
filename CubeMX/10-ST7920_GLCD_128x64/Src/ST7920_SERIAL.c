/*
 * ST7920_SERIAL.c
 *
 *  Created on: 07-Jun-2019
 *      Author: poe
 */

#include "stm32f4xx_hal.h"
#include "ST7920_SERIAL.h"
//#include "delay.h"

/* setup below is as follows
 * A5 ---------> SCLK (EN)	------stm32f4----> PB12
 * A6 ---------> CS (RS)		------stm32f4----> PB13
 * A7 ---------> SID (RW)		------stm32f4----> PB14
 * B0 ---------> RST (RST)	------stm32f4----> PB15
 *
 */

//serialwire defining
#define SCLK_PIN GPIO_PIN_12
#define SCLK_PORT GPIOB

#define SID_PIN GPIO_PIN_14
#define SID_PORT GPIOB

#define CS_PIN GPIO_PIN_13
#define CS_PORT GPIOA

#define RST_PIN GPIO_PIN_15
#define RST_PORT GPIOB
//end

//spi defining
static SPI_HandleTypeDef lcdSPIhandle;
////spi SCK
//static GPIO_TypeDef *SCK_GPIO;
//static uint16_t SCK_PIN;
////spi SID
//static GPIO_TypeDef *SID_GPIO;
//static uint16_t SID_PIN;
////CS pin
//static GPIO_TypeDef  *CS_PORT;
//static uint16_t CS_PIN;
////Reset pin
//static GPIO_TypeDef  *RESET_GPIO;
//static uint16_t RESET_PIN;

uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
uint8_t numRows = 64;
uint8_t numCols = 128;
uint8_t Graphic_Check = 0;
uint8_t image[(128 * 64)/8];




//Library initilation
void ST7920_Init (void)
{
	HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_RESET);  // RESET=0
	HAL_Delay(10);   // wait for 10ms
	HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_SET);  // RESET=1

	HAL_Delay(50);   //wait for >40 ms


	ST7920_SendCmd(0x30);  // 8bit mode
	//delay_us(110);  //  >100us delay
	HAL_Delay(110);

	ST7920_SendCmd(0x30);  // 8bit mode
	//delay_us(40);  // >37us delay
	HAL_Delay(40);

	ST7920_SendCmd(0x08);  // D=0, C=0, B=0
	//delay_us(110);  // >100us delay
	HAL_Delay(110);

	ST7920_SendCmd(0x01);  // clear screen
	HAL_Delay(12);  // >10 ms delay


	ST7920_SendCmd(0x06);  // cursor increment right no shift
	HAL_Delay(1);  // 1ms delay

	ST7920_SendCmd(0x0C);  // D=1, C=0, B=0
    HAL_Delay(1);  // 1ms delay

	ST7920_SendCmd(0x02);  // return to home
	HAL_Delay(1);  // 1ms delay

}

// A replacement for SPI_TRANSMIT
void SendByteSPI(uint8_t byte)
{
	for(int i=0;i<8;i++)
	{
		if((byte<<i)&0x80)
			{
				HAL_GPIO_WritePin(SID_PORT, SID_PIN, GPIO_PIN_SET);  // SID=1  OR MOSI
			}

		else HAL_GPIO_WritePin(SID_PORT, SID_PIN, GPIO_PIN_RESET);  // SID=0

		HAL_GPIO_WritePin(SCLK_PORT, SCLK_PIN, GPIO_PIN_RESET);  // SCLK =0  OR SCK

		HAL_GPIO_WritePin(SCLK_PORT, SCLK_PIN, GPIO_PIN_SET);  // SCLK=1

	}
}
void SendByte_SPI(uint8_t byyt)
{
	
				HAL_SPI_Transmit(&lcdSPIhandle, &byyt, 8, 5);
			
}



void SendCommand_SPI(uint8_t com)
{
	//*(__IO uint8_t *)(0x60000000) = com;
	uint8_t tmpCmd = com;
	//Set DC HIGH for COMMAND mode
//....	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_RESET);
	//Put CS HIGH//LOW
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpCmd, 8, 5);
	SendByte_SPI(0xf8+(0<<1));  // send the SYNC + RS(0)
	SendByte_SPI(tmpCmd&0xf0);  // send the higher nibble first
	SendByte_SPI((tmpCmd<<4)&0xf0);  // send the lower nibble
	//Bring CS LOW//HIGH
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void ST7920_SendCmd (uint8_t cmd)
{

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);  // PUll the CS high

	SendByteSPI(0xf8+(0<<1));  // send the SYNC + RS(0)
	SendByteSPI(cmd&0xf0);  // send the higher nibble first
	SendByteSPI((cmd<<4)&0xf0);  // send the lower nibble
//	delay_us(50);
	HAL_Delay(2);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);  // PUll the CS LOW

}

void ST7920_SendData (uint8_t data)
{

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);  // PUll the CS high

	SendByteSPI(0xf8+(1<<1));  // send the SYNC + RS(1)
	SendByteSPI(data&0xf0);  // send the higher nibble first
	SendByteSPI((data<<4)&0xf0);  // send the lower nibble
//	delay_us(50);
	HAL_Delay(2);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);  // PUll the CS LOW
}
void SendData_SPI(uint8_t data)
{
	//*(__IO uint8_t *)(0x60040000) = data;
	uint8_t tmpCmd = data;
	//Put CS LOW
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpCmd, 8, 5);
	SendByte_SPI(0xf8+(1<<1));  // send the SYNC + RS(1)
	SendByte_SPI(tmpCmd&0xf0);  // send the higher nibble first
	SendByte_SPI((tmpCmd<<4)&0xf0);  // send the lower nibble
	//Bring CS HIGH
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void ST7920_SendString(int row, int col, char* string)
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

    //ST7920_SendCmd(col);
		SendCommand_SPI(col);

    while (*string)
    	{
    	//	ST7920_SendData(*string++);
				SendData_SPI(*string++);
				
    	}
}



//Library SPI initilation
void ST7920_SPI_Init(SPI_HandleTypeDef *spiLcdHandle)
{
 
	 //Copy SPI settings
	 memcpy(&lcdSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	 //CS pins
//	 CS_PORT = csPORT;
//	 CS_PIN = csPIN;

	 HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	 //RESET pin
//	 RESET_GPIO = resetPORT;
//	 RESET_PIN = resetPIN;
	 HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_SET);  //Turn LCD ON
	 
   SendCommand_SPI (ILI9341_RESET); // software reset comand
   HAL_Delay(100);
   SendCommand_SPI (ILI9341_DISPLAY_OFF); // display off
   //------------power control------------------------------
   SendCommand_SPI (ILI9341_POWER1); // power control
   SendData_SPI(0x26); // GVDD = 4.75v
   SendCommand_SPI (ILI9341_POWER2); // power control
   SendData_SPI   (0x11); // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3
   //--------------VCOM-------------------------------------
   SendCommand_SPI (ILI9341_VCOM1); // vcom control
   SendData_SPI   (0x35); // Set the VCOMH voltage (0x35 = 4.025v)
   SendData_SPI   (0x3e); // Set the VCOML voltage (0x3E = -0.950v)
   SendCommand_SPI (ILI9341_VCOM2); // vcom control
   SendData_SPI   (0xbe);

   //------------memory access control------------------------
   SendCommand_SPI (ILI9341_MAC); // memory access control
   SendData_SPI(0x48);

   SendCommand_SPI (ILI9341_PIXEL_FORMAT); // pixel format set
   SendData_SPI   (0x55); // 16bit /pixel

	 SendCommand_SPI(ILI9341_FRC);
   SendData_SPI(0);
   SendData_SPI(0x1F);
   //-------------ddram ----------------------------
   SendCommand_SPI (ILI9341_COLUMN_ADDR); // column set
   SendData_SPI   (0x00); // x0_HIGH---0
   SendData_SPI   (0x00); // x0_LOW----0
   SendData_SPI   (0x00); // x1_HIGH---240
   SendData_SPI   (0xEF); // x1_LOW----240
   SendCommand_SPI (ILI9341_PAGE_ADDR); // page address set
   SendData_SPI   (0x00); // y0_HIGH---0
   SendData_SPI   (0x00); // y0_LOW----0
   SendData_SPI   (0x01); // y1_HIGH---320
   SendData_SPI   (0x3F); // y1_LOW----320

   SendCommand_SPI (ILI9341_TEARING_OFF); // tearing effect off
   //LCD_write_cmd(ILI9341_TEARING_ON); // tearing effect on
   //LCD_write_cmd(ILI9341_DISPLAY_INVERSION); // display inversion
   SendCommand_SPI (ILI9341_Entry_Mode_Set); // entry mode set
   // Deep Standby Mode: OFF
   // Set the output level of gate driver G1-G320: Normal display
   // Low voltage detection: Disable
   SendData_SPI   (0x07);
   //-----------------display------------------------
   SendCommand_SPI (ILI9341_DFC); // display function control
   //Set the scan mode in non-display area
   //Determine source/VCOM output in a non-display area in the partial display mode
   SendData_SPI   (0x0a);
   //Select whether the liquid crystal type is normally white type or normally black type
   //Sets the direction of scan by the gate driver in the range determined by SCN and NL
   //Select the shift direction of outputs from the source driver
   //Sets the gate driver pin arrangement in combination with the GS bit to select the optimal scan mode for the module
   //Specify the scan cycle interval of gate driver in non-display area when PTG to select interval scan
   SendData_SPI   (0x82);
   // Sets the number of lines to drive the LCD at an interval of 8 lines
   SendData_SPI   (0x27);
   SendData_SPI   (0x00); // clock divisor

   SendCommand_SPI (ILI9341_SLEEP_OUT); // sleep out
   HAL_Delay(100);
   SendCommand_SPI (ILI9341_DISPLAY_ON); // display on
   HAL_Delay(100);
   SendCommand_SPI (ILI9341_GRAM); // memory write
   HAL_Delay(5);
	
}
// switch to graphic mode or normal mode::: enable = 1 -> graphic mode enable = 0 -> normal mode
void ST7920_GraphicMode (int enable)   // 1-enable, 0-disable
{
	
	
	if (enable == 1)
	{
		SendCommand_SPI(0x30);
		//ST7920_SendCmd(0x30);  // 8 bit mode
		HAL_Delay (1);
		SendCommand_SPI(0x34);
		//ST7920_SendCmd(0x34);  // switch to Extended instructions
		HAL_Delay (1);
		SendCommand_SPI(0x36);
		//ST7920_SendCmd(0x36);  // enable graphics
		HAL_Delay (1);
		Graphic_Check = 1;  // update the variable
	}

	else if (enable == 0)
	{
		SendCommand_SPI(0x30);
		//ST7920_SendCmd(0x30);  // 8 bit mode
		HAL_Delay (1);
		Graphic_Check = 0;  // update the variable
	}
}

void ST7920_DrawBitmap(const unsigned char* graphic)
{

	uint8_t x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)							// Draws top half of the screen.
			{												// In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				SendCommand_SPI(0x80 | y);
			//	ST7920_SendCmd(0x80 | y);				// Vertical coordinate of the screen is specified first. (0-31)
				SendCommand_SPI(0x80 | x);
			//	ST7920_SendCmd(0x80 | x);				// Then horizontal coordinate of the screen is specified. (0-8)
				SendData_SPI(graphic[2*x + 16*y]);
			//	ST7920_SendData(graphic[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				SendData_SPI(graphic[2*x+1 + 16*y]);
			//	ST7920_SendData(graphic[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)							// Draws bottom half of the screen.
			{												// Actions performed as same as the upper half screen.
				SendCommand_SPI(0x80 | (y-32));
				//ST7920_SendCmd(0x80 | (y-32));			// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				SendCommand_SPI(0x88 | x);
				//ST7920_SendCmd(0x88 | x);
				SendData_SPI(graphic[2*x + 16*y]);
				//ST7920_SendData(graphic[2*x + 16*y]);
				SendData_SPI(graphic[2*x+1 + 16*y]);
				//ST7920_SendData(graphic[2*x+1 + 16*y]);
			}
		}

	}
}


// Update the display with the selected graphics
void ST7920_Update(void)
{
	ST7920_DrawBitmap(image);
}



void ST7920_Clear(void)
{
	if (Graphic_Check == 1)  // if the graphic mode is set
	{
		uint8_t x, y;
		for(y = 0; y < 64; y++)
		{
			if(y < 32)
			{
				SendCommand_SPI(0x80 | y);
				//ST7920_SendCmd(0x80 | y);
				SendCommand_SPI(0x80);
				//ST7920_SendCmd(0x80);
				
			}
			else
			{
				SendCommand_SPI(0x80 | (y-32));
				//ST7920_SendCmd(0x80 | (y-32));
				SendCommand_SPI(0x88);
				//ST7920_SendCmd(0x88);
			}
			for(x = 0; x < 8; x++)
			{
				SendData_SPI(0);
				//ST7920_SendData(0);
				SendData_SPI(0);
				//ST7920_SendData(0);
			}
		}
	}

	else
	{
		SendData_SPI(0);
		//ST7920_SendData(0);
		HAL_Delay(2); // delay >1.6 ms
		SendCommand_SPI(0x01);
		//ST7920_SendCmd(0x01);   // clear the display using command
		HAL_Delay(2); // delay >1.6 ms
	}
}



// set Pixel
void SetPixel(uint8_t x, uint8_t y)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    *p |= 0x80u >> (x%8);

    *image = *p;

    // Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
    if (startRow > y) { startRow = y; }
    if (endRow <= y)  { endRow = y + 1; }
    if (startCol > x) { startCol = x; }
    if (endCol <= x)  { endCol = x + 1; }


  }

}

/* draw a line
 * start point (X0, Y0)
 * end point (X1, Y1)
 */
void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
  int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  for (;;)
  {
    SetPixel(x0, y0);
    if (x0 == x1 && y0 == y1) break;
    int e2 = err + err;
    if (e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}




/* Draw rectangle
 * start point (x,y)
 * w -> width
 * h -> height
 */
void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	/* Check input parameters */
	if (
		x >= numCols ||
		y >= numRows
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= numCols) {
		w = numCols - x;
	}
	if ((y + h) >= numRows) {
		h = numRows - y;
	}

	/* Draw 4 lines */
	DrawLine(x, y, x + w, y);         /* Top line */
	DrawLine(x, y + h, x + w, y + h); /* Bottom line */
	DrawLine(x, y, x, y + h);         /* Left line */
	DrawLine(x + w, y, x + w, y + h); /* Right line */
}




/* Draw filled rectangle
 * Start point (x,y)
 * w -> width
 * h -> height
 */
void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint8_t i;

	/* Check input parameters */
	if (
		x >= numCols ||
		y >= numRows
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= numCols) {
		w = numCols - x;
	}
	if ((y + h) >= numRows) {
		h = numRows - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		DrawLine(x, y + i, x + w, y + i);
	}
}




/* draw circle
 * centre (x0,y0)
 * radius = radius
 */
void DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius)
{
  int f = 1 - (int)radius;
  int ddF_x = 1;

  int ddF_y = -2 * (int)radius;
  int x = 0;

  SetPixel(x0, y0 + radius);
  SetPixel(x0, y0 - radius);
  SetPixel(x0 + radius, y0);
  SetPixel(x0 - radius, y0);

  int y = radius;
  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    SetPixel(x0 + x, y0 + y);
    SetPixel(x0 - x, y0 + y);
    SetPixel(x0 + x, y0 - y);
    SetPixel(x0 - x, y0 - y);
    SetPixel(x0 + y, y0 + x);
    SetPixel(x0 - y, y0 + x);
    SetPixel(x0 + y, y0 - x);
    SetPixel(x0 - y, y0 - x);
  }
}


// Draw Filled Circle

void DrawFilledCircle(int16_t x0, int16_t y0, int16_t r)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SetPixel(x0, y0 + r);
    SetPixel(x0, y0 - r);
    SetPixel(x0 + r, y0);
    SetPixel(x0 - r, y0);
    DrawLine(x0 - r, y0, x0 + r, y0);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        DrawLine(x0 - x, y0 + y, x0 + x, y0 + y);
        DrawLine(x0 + x, y0 - y, x0 - x, y0 - y);

        DrawLine(x0 + y, y0 + x, x0 - y, y0 + x);
        DrawLine(x0 + y, y0 - x, x0 - y, y0 - x);
    }
}



// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
	/* Draw lines */
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x3, y3, x1, y1);
}



// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

#define ABS(x)   ((x) > 0 ? (x) : -(x))

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		DrawLine(x, y, x3, y3);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
