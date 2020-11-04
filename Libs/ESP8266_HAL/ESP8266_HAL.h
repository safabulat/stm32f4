/*
 * ESP8266_HAL.h
 *
 *  Created on: 6 agustos, 2020
 *      Author: Safafafafafa
 */

#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_

#define MOSI_PIN GPIO
void ESP_Init (char *SSID, char *PASSWD);

void Server_Start (void);


#endif /* INC_ESP8266_HAL_H_ */
