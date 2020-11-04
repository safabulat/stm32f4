/*
	Library: 				Motor
	Written by:  		Safa BULAT
	Date Written:		07/08/2020
	Description:		Motor Drive Stuff

*/


//***** Header files *****//
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "string.h"
#include "stdio.h"

//***** List of COMMANDS *****//
#define MOTOR_ENABLED				0x01
#define MOTOR_RETURNHOME 		0x02
#define MOTOR_DRIVETEST 		0x04
#define MOTOR_ROTATETEST 		0x08


//***** List of commands Bitfields *****//
/*
//1) Entry mode Bitfields
#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
//2) Entry mode Bitfields
#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
//3) Display control
#define LCD_DISPLAY_B						0x01
#define LCD_DISPLAY_C						0x02
#define LCD_DISPLAY_D						0x04
//4) Shift control
#define LCD_SHIFT_RL						0x04
#define LCD_SHIFT_SC						0x08
//5) Function set control
#define LCD_FUNCTION_F					0x04
#define LCD_FUNCTION_N					0x08
#define LCD_FUNCTION_DL					0x10
*/
//********** END **********//

//***** Functions prototypes *****//
//Private functions
//1) Enable EN pulse
static void MOTOR_EnablePulse(void);
//2) Reset control
static void MOTOR_Reset(bool state);
//3) Microsecond delay functions
static void MOTOR_TIM_Config(void);
static void MOTOR_TIM_MicroSecDelay(uint32_t uSecDelay);
//4) RUN
static void MOTOR_RUN(uint8_t byte);
//5) Run Command
static void MOTOR_RunCommand(uint8_t command);
//********** END **********//

//Public functions
//1) MOTOR begin 8 bits function
void MOTOR_Begin8BIT(GPIO_TypeDef* PORT_Rs_En, uint16_t Rs, uint16_t En, GPIO_TypeDef* PORT_MSs, uint16_t S1, uint16_t S2, uint16_t S3, GPIO_TypeDef* Func, uint16_t F1, uint16_t F2, uint16_t F3);
//2) MOTOR begin 4 bits function
void MOTOR_Begin2BIT(GPIO_TypeDef* Func, uint16_t F2, uint16_t F3);
//3) Go_to
void MOTOR_GO(int num);
//4) Stop
void MOTOR_Stop(void);
//5) Turn
void MOTOR_Turn(void);
//6) Full Step
void MOTOR_F_Step(void);
//7) Half Step
void MOTOR_H_Step(void);
//8) Quarter Step
void MOTOR_4_Step(void);
//9) Eighth Step
void MOTOR_8_Step(void);
//10) Sixteenth Step
void MOTOR_16_Step(void);

//********** END **********//

