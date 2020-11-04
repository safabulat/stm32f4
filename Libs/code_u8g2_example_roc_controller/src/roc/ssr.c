/*
 * ssr.c
 *
 *  Created on: 16.03.2019
 *      Author: Johannes
 */

#include "roc/ssr.h"
#include "cmsis_os.h"
#include "stm32f0xx_hal.h"

uint8_t ssr_state = SSR_STATE_DISABLED;

void run_ssr_thread(void)
{
	for (;;)
	{
		HAL_GPIO_WritePin(SSR_GPIO_PORT, SSR_GPIO_PIN, ssr_state);
		osDelay(10);
	}
}

void ssr_toggle(void)
{
	ssr_state = !ssr_state;
}

void ssr_set_state(uint8_t state)
{
	ssr_state = state;
}

uint8_t ssr_get_state(void)
{
	return ssr_state;
}
