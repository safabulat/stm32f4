/*
 * roctrl.c
 *
 *  Created on: 18.03.2019
 *      Author: Johannes Berndorfer
 *
 *  Main control code for the reflow oven.
 */

#include "roc/roctrl.h"
#include "roc/debug.h"
#include "cmsis_os.h"
#include "sys/_stdint.h"

//-------- VARIABLES --------

// Running flag for this control code.
static uint8_t roctrl_running = ROCTRL_NOT_RUNNING;

// The current control state of the reflow oven controller.
static uint8_t control_state = ROCTRL_STATE_IDLE; // Default in idle state.

// Flag indicating the main thread function that the control state has been changed.
static uint8_t control_state_changed_flag = ROCTRL_STATE_CHG_FLAG_RESET;

// Currently set reflow oven profile
static uint8_t current_reflow_profile = ROCTRL_REFLOW_PROFILE_HARDCODED; // Default to hardcoded reflow oven profile

//-------- END VARIABLES --------



//-------- PRIVATE FUNCTION DECLARATIONS --------

static void roctrl_switch_control_state(uint8_t switch_state);
static void roctrl_thread_main(void);

//-------- END PRIVATE FUNCTION DECLARATIONS --------



//-------- FUNCTIONS --------

/*
 * Init function of this file.
 */
void roctrl_init_thread(void)
{
	if (roctrl_running == ROCTRL_NOT_RUNNING) // Stop ROCTRL from running more than once (in several threads at the same time)
	{
		roctrl_running = ROCTRL_RUNNING;
		control_state_changed_flag = ROCTRL_STATE_CHG_FLAG_SET;
		roctrl_thread_main();
	}
	else
		return;
}

/*
 * Selects a new profile.
 */
uint8_t roctrl_set_reflow_profile(uint8_t profile)
{
	if (control_state == ROCTRL_STATE_IDLE)
	{
		current_reflow_profile = profile;
		return 1;
	}
	else
		return 0;
}

/*
 * Initiates the reflow process.
 */
uint8_t roctrl_init_reflow_proc(void)
{
	if (control_state == ROCTRL_STATE_IDLE)
	{
		roctrl_switch_control_state(ROCTRL_STATE_REFLOW);
		return 1;
	}
	else
		return 0;
}

//-------- END FUNCTIONS --------



//-------- PRIVATE FUNCTIONS --------

/*
 * The thread's main function.
 */
static void roctrl_thread_main(void)
{
	while (1)
	{
		switch (control_state)
		{
		case ROCTRL_STATE_IDLE:
			// IDLE CONTROL CODE

			break;
		case ROCTRL_STATE_REFLOW:
			// REFLOW CONTROL CODE

			break;
		case ROCTRL_STATE_PROGRAM:
			// PROGRAM CONTROL CODE

			break;
		case ROCTRL_STATE_DEBUG:
			// DEBUG CONTROL CODE

			break;
		default:
			debug_cons_println("[ROCTRL] Error: Current control state does not exist.");
			osDelay(500);
			break;
		}
	}
}

/*
 * Switches to a new control state.
 */
static void roctrl_switch_control_state(uint8_t switch_state)
{
	control_state = switch_state;
	control_state_changed_flag = ROCTRL_STATE_CHG_FLAG_SET;
}

//-------- END PRIVATE FUNCTIONS --------
