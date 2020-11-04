/*
 * debug.c
 *
 *  Created on: 16.03.2019
 *      Author: Johannes
 */

#include "roc/debug.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "string.h"
#include "stm32f0xx_hal_uart.h"

UART_HandleTypeDef* debug_cons_handle = 0x00;
osMutexId uart_mutex;
uint8_t uart_state = 0;

uint8_t uart_rcv_buf;
char uart_char_buf[CONSOLE_RECIEVE_BUFFER_SIZE];
uint8_t uart_char_buf_index = 0;

void debug_cons_init_handle(UART_HandleTypeDef* huart, osMutexId mutex)
{
	debug_cons_handle = huart;
	uart_mutex = mutex;
	HAL_UART_Receive_DMA(debug_cons_handle, &uart_rcv_buf, 1);
	if (CONSOLE_RECIEVE_BUFFER_SIZE < 1)
	{
		debug_cons_println("Internal Error: CONSOLE_RECIEVE_BUFFER_SIZE is 0.");
	}
	uart_char_buf[0] = 0x00;
}

void debug_cons_print(char* msg)
{
	if (debug_cons_handle == 0x00)
		return;
	while (uart_state == 1) // Wait for the current transfer to be complete
	{
		if (ALLOC_WAIT_FUNC_TYPE == 0x00) // Differentiable wait functions. When debugging is needed before the os kernel initializes, the osWait function never returns and the HAL_Delay function is needed for proper operation.
			osDelay(5); // Delay 5ms for next try and give the RTOS the option to halt the executing thread.
		else
			HAL_Delay(5);
	}
	uart_state = 1; // Occupy UART for transfer
	HAL_UART_Transmit_DMA(debug_cons_handle, (uint8_t*) msg, strlen(msg)); // Transfer the message using DMA.
}

void debug_cons_print_char(char ch)
{
	if (debug_cons_handle == 0x00)
		return;
	while (uart_state == 1) // Wait for the current transfer to be complete
	{
		osDelay(5); // Delay 5ms for next try and give the RTOS the option to halt the executing thread.
	}
	uart_state = 1; // Occupy UART for transfer
	HAL_UART_Transmit_DMA(debug_cons_handle, (uint8_t*) &ch, 1); // Transfer the message using DMA.
}

void debug_cons_println(char* msg)
{
	debug_cons_print(msg);
	debug_cons_print(CONSOLE_CHARACTER_NEW_LINE);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_state = 0; // Free the state to indicate that the current DMA transfer is completed.
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_DMA(debug_cons_handle, &uart_rcv_buf, 1);
	// 1 byte recieved in uart_rcv_buf.
	if (((char) uart_rcv_buf) == '\n')
	{
		// Enter sent
		uart_char_buf_index = 0;
		uart_char_buf[0] = 0x00;
		debug_rcv_proc_cmd(uart_char_buf);
	}
	else if (uart_char_buf_index < CONSOLE_RECIEVE_BUFFER_SIZE - 1) // Check if the new char fits in the char buffer (plus the size of the binary 0 at the end of the char buffer)
	{
		// Recieved a char and the recieve buffer is not full.
		uart_char_buf[uart_char_buf_index] = (char) uart_rcv_buf;
		uart_char_buf_index++;
		uart_char_buf[uart_char_buf_index] = 0;

		debug_cons_print_char((char) uart_rcv_buf); // Loopback the input of the user.
	}
}

void debug_rcv_proc_cmd(char* cmd)
{
	debug_cons_print("Processing command: ");
	debug_cons_println(cmd);
}
