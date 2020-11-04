
#include <stdio.h>  	//printf
#include "string.h"
#include "stm32f4xx_hal.h"
//This variable is defined by other .c files.
extern UART_HandleTypeDef huart2;
uint8_t ch;
uint8_t ch_r;
// Rewrite this function, redirect printf function to the serial port, meaning that printf directly output to the serial port, its default output to the console




// Redirect scanf function to the serial port means to accept the data sent by the serial port, the default is to accept the console data
/*fgetc*/
int fgetc(FILE * F)    
{
         HAL_UART_Receive (&huart2,&ch_r,1,0xffff);//Receive
    return ch_r;
}