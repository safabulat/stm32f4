/*
 * debug.h
 *
 *  Created on: 16.03.2019
 *      Author: Johannes
 */

#ifndef INC_ROC_DEBUG_H_
#define INC_ROC_DEBUG_H_

#include "stm32f0xx_hal.h"
#include "cmsis_os.h"

#define CONSOLE_CHARACTER_NEW_LINE "\n"
#define CONSOLE_RECIEVE_BUFFER_SIZE 64

#define ALLOC_WAIT_FUNC_TYPE 0x00 // 0x00: osDelay, 0x01: HAL_Delay;

void debug_cons_init_handle(UART_HandleTypeDef* huart, osMutexId mutex);
void debug_cons_print(char* msg);
void debug_cons_println(char* msg);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void debug_rcv_proc_cmd(char* cmd);

#endif /* INC_ROC_DEBUG_H_ */
