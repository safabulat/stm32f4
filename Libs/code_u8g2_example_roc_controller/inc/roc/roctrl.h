/*
 * roctrl.h
 *
 *  Created on: 18.03.2019
 *      Author: Johannes
 */

#ifndef INC_ROC_ROCTRL_H_
#define INC_ROC_ROCTRL_H_

/*
 * ROCTRL running flag
 */
#define ROCTRL_NOT_RUNNING 0x00
#define ROCTRL_RUNNING 0x01

/*
 * Reflow oven profiles
 */
#define ROCTRL_REFLOW_PROFILE_HARDCODED 0x00
#define ROCTRL_REFLOW_PROFILE_0 0x01 // Customisable profiles in EEPROM
#define ROCTRL_REFLOW_PROFILE_1 0x02
#define ROCTRL_REFLOW_PROFILE_2 0x03
#define ROCTRL_REFLOW_PROFILE_3 0x03

/*
 * Control states
 */
#define ROCTRL_STATE_IDLE 0x00 // Controller is in idle mode - menu selection or just sitting around.
#define ROCTRL_STATE_PROGRAM 0x01 // Controller is currently in program mode and communicating over USB to program a new profile
#define ROCTRL_STATE_REFLOW 0x02 // A reflow process is currently going on.
#define ROCTRL_STATE_DEBUG 0xFF // Reserved debug state of the controller.

/*
 * Control state change flag
 */
#define ROCTRL_STATE_CHG_FLAG_RESET 0x00
#define ROCTRL_STATE_CHG_FLAG_SET 0x01

#endif /* INC_ROC_ROCTRL_H_ */
