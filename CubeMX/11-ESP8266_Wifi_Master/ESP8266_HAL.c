/*
 * ESP8266_HAL.c
 *
 *  Created on: 6 agustos, 2020
 *      Author: Safafafafafa
 */

//Header files
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"
//SPI handler
static SPI_HandleTypeDef wifiSPIhandle;
//Chip Select pin
static GPIO_TypeDef  *CS_GPIO;
static uint16_t CS_PIN;


/*****************************************************************************************************************************************/

char buffer[20];

char *Basic_inclusion = "<!DOCTYPE html> <html>\n<head><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
		<title>LED CONTROL</title>\n<style>html { font-family: Helvetica; \
		display: inline-block; margin: 0px auto; text-align: center;}\n\
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
		width: 80px;background-color: #1abc9c;border: none;color: white;\
		padding: 13px 30px;text-decoration: none;font-size: 25px;\
		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
		.button-on {background-color: #1abc9c;}\n.button-on:active \
		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
		</style>\n</head>\n<body>\n<h1>ESP8266 LED CONTROL</h1>\n";

char *LED_ON = "<p>LED Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>";
char *LED_OFF = "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>";
char *Terminate = "</body></html>";



/*****************************************************************************************************************************************/


//***** Functions *****//
//1. Write Command to ESP8266
void ESP8266_SendCommand(uint8_t cmd)
{
	//*(__IO uint8_t *)(0x60000000) = cmd;
	uint8_t tmpCmd = cmd;
	//Bring CS HIGH
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	//Write byte using SPI
	HAL_SPI_Transmit(&wifiSPIhandle, &tmpCmd, 1, 5);
	HAL_Delay(100);
	//Put CS LOW
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);

}
//2. Write data to ESP8266
void ESP8266_SendData(uint8_t data)
{
	//*(__IO uint8_t *)(0x60040000) = data;
	uint8_t tmpData = data;
	//Bring CS HIGH
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	//Write byte using SPI
	HAL_SPI_Transmit(&wifiSPIhandle, &tmpData, 1, 5);
	HAL_Delay(100);
	//Put CS LOW
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);
}
//3. Send String to ESP8266
void ESP8266_print(char *chaer, int16_t add)
{
	ESP8266_SendCommand(add);
	ESP8266_SendData(*chaer);
}
//4. Lib INIT
void ESP8266_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN)
{
	//Copy SPI settings
	memcpy(&wifiSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	//CS pin
	CS_GPIO = csPORT;
	CS_PIN = csPIN;
	HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);
	HAL_Delay(100);
}




//5. Look For
int Look_for (char *str, char *buffertolookinto)
{
	int stringlength = strlen (str);
	int bufferlength = strlen (buffertolookinto);
	int so_far = 0;
	int indx = 0;
repeat:
	while (str[so_far] != buffertolookinto[indx]) indx++;
	if (str[so_far] == buffertolookinto[indx]){
	while (str[so_far] == buffertolookinto[indx])
	{
		so_far++;
		indx++;
	}
	}

	else
		{
			so_far =0;
			if (indx >= bufferlength) return -1;
			goto repeat;
		}

	if (so_far == stringlength) return 1;
	else return -1;
}
//6. ESP wifi
void ESP_Wifi_(char *SSID, char *PASSWD)
{
	char data[80];



	/********* AT+CWJAP="SSID","PASSWD" **********/
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	HAL_SPI_Transmit(&wifiSPIhandle, (uint8_t *)data, 8, 10);
	sprintf (data, "Connected to,\"%s\"\n\n", SSID);
	HAL_SPI_Transmit(&wifiSPIhandle, (uint8_t *)data, 8, 10);
}
//7. Server Send
int Server_Send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	HAL_SPI_Transmit(&wifiSPIhandle, (uint8_t *)data, 8, 10);
	sprintf (data, "AT+CIPCLOSE=5\r\n");
	HAL_SPI_Transmit(&wifiSPIhandle, (uint8_t *)data, 8, 10);
	return 1;
}
//8. Get After
int Get_after (char *string, uint8_t numberofchars, char *buffertosave, SPI_HandleTypeDef *spiLcdHandle)
{

	for (int indx=0; indx<numberofchars; indx++)
	{
		HAL_SPI_Receive(spiLcdHandle, (uint8_t *)buffertosave, 1, 10);
	}
	return 1;
}

//11. Server Handle
void Server_Handle (char *str, int Link_ID)
{
	char datatosend[1024] = {0};
	if (!(strcmp (str, "/ledon")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_ON);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/ledoff")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

}

void Server_Start (void)
{
	char buftocopyinto[64] = {0};
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID, &wifiSPIhandle)));
	Link_ID -= 48;
//	while (!(Copy_upto(" HTTP/1.1", buftocopyinto, &wifiSPIhandle)));
	if (Look_for("/ledon", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
		Server_Handle("/ledon",Link_ID);
	}

	else if (Look_for("/ledoff", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		Server_Handle("/ledoff",Link_ID);
	}

	else if (Look_for("/favicon.ico", buftocopyinto) == 1);

	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		Server_Handle("/ ", Link_ID);
	}
}