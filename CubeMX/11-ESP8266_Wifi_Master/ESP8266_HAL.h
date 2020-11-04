/*
 * ESP8266_HAL.h
 *
 *  Created on: 6 agustos, 2020
 *      Author: Safafafafafa
 */

#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_

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



//***** Functions prototypes *****//
void ESP8266_SendCommand(uint8_t cmd);
void ESP8266_SendData(uint8_t data);
void ESP8266_print(char *chaer, int16_t add);
void ESP8266_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN);
void ESP_Wifi_(char *SSID, char *PASSWD);
void Server_Start (void);
int Look_for (char *str, char *buffertolookinto);
//***** Endlist *****//

#endif /* INC_ESP8266_HAL_H_ */
