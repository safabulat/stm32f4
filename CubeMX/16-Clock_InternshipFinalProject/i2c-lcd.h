#include "stm32f4xx_hal.h"

#include "stdio.h"

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_PrintInt(int number);

void lcd_PrintFloat(float number, int decimalPoints);

void LCD1602_noCursor(void);

void LCD1602_cursor(void);

void lcd_setCursor(uint8_t row, uint8_t col);

void lcd_clear2 (void);
	

	


