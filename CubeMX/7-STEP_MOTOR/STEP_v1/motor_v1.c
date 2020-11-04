/*
	Library: 				Motor
	Written by:  		Safa BULAT
	Date Written:		07/08/2020
	Description:		Motor Drive Stuff

*/

//***** Header files *****//
#include "motor_v1.h"

//***** Variables *****//
static const uint32_t writeTimeConstant = 10;
static uint8_t mode_8_2_I2C = 1;
static GPIO_TypeDef* PORT_Reset_and_En;							  // Reset and Enable PORTs
static uint16_t PIN_Reset, PIN_En;									  // Reset and Enable pins

static GPIO_TypeDef* PORT_MS;													// MSs 		S1, S2, S3 PORT
static uint16_t S1_PIN, S2_PIN, S3_PIN;								// MSs 		S1, S2, S3 pins

static GPIO_TypeDef* PORT_Func;												// Func	Sleep, Step, Dir PORT
static uint16_t F1_PIN, F2_PIN, F3_PIN;								// Func	Sleep, Step, Dir pins

static uint8_t FunctionSet = 0x38;										// Function Set
//********** END **********//


//***** Functions definitions *****//
//Private functions
//1) Enable EN pulse
static void MOTOR_EnablePulse(void)
{
	HAL_GPIO_WritePin(PORT_Reset_and_En, PIN_En, GPIO_PIN_SET);
	MOTOR_TIM_MicroSecDelay(writeTimeConstant);
	HAL_GPIO_WritePin(PORT_Reset_and_En, PIN_En, GPIO_PIN_RESET);
	MOTOR_TIM_MicroSecDelay(60);
}
//2) Reset control
static void MOTOR_Reset(bool state)
{
	if(state) HAL_GPIO_WritePin(PORT_Reset_and_En, PIN_Reset, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(PORT_Reset_and_En, PIN_Reset, GPIO_PIN_RESET);
}
//3) Microsecond delay functions
static void MOTOR_TIM_Config(void)
{
	RCC_ClkInitTypeDef myCLKtypeDef;
	uint32_t clockSpeed;
	uint32_t flashLatencyVar;
	HAL_RCC_GetClockConfig(&myCLKtypeDef, &flashLatencyVar);
	if(myCLKtypeDef.APB1CLKDivider == RCC_HCLK_DIV1)
	{
		clockSpeed = HAL_RCC_GetPCLK1Freq();
	}
	else
	{
		clockSpeed = HAL_RCC_GetPCLK1Freq()*2;
	}
	clockSpeed *= 0.000001;
	
	//Enable clock for TIM2 timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // 0x1
	//Set the mode to Count up
	TIM3->CR1 &= ~(0x0010);
	//Enable Update Event
	TIM3->CR1 &= ~(0x0001);
	//Update request source 
	TIM3->CR1 &= ~(1UL << 2);
	// Set bit 3 High to enable One-Pulse mode
	TIM3->CR1 |= (1UL << 3);				  
	//Set the Prescalar
	TIM3->PSC = clockSpeed-1;
	//Set and Auto-Reload Value to delay the timer 1 sec
	TIM3->ARR = 10-1; 								// The Flag sets when overflows
	//Event generation handling to reset the counter
	TIM3->EGR = 1; 					//Update generate auto
	TIM3->SR &= ~(0x0001);	//Clear Update interrupt flag
}
static void MOTOR_TIM_MicroSecDelay(uint32_t uSecDelay)
{
	TIM3->ARR = uSecDelay-1;
	TIM3->SR &= ~(0x0001);  // Clear UEV flag
	TIM3->CR1 |= 1UL;
	while((TIM3->SR&0x0001) != 1);
}
//4) Run
static void MOTOR_RUN(uint8_t byte)
{
	uint8_t MS_nibble = byte&0xF, Func_nibble = (byte>>4)&0xF;
	 
	if(mode_8_2_I2C == 1)		//8bits mode
	{
		//write data to output pins
		//MSs data
		HAL_GPIO_WritePin(PORT_MS, S1_PIN, (GPIO_PinState)(MS_nibble&0x1));
		HAL_GPIO_WritePin(PORT_MS, S2_PIN, (GPIO_PinState)(MS_nibble&0x2));
		HAL_GPIO_WritePin(PORT_MS, S3_PIN, (GPIO_PinState)(MS_nibble&0x4));
		//Func data
		HAL_GPIO_WritePin(PORT_Func, F1_PIN, (GPIO_PinState)(Func_nibble&0x1));
		HAL_GPIO_WritePin(PORT_Func, F2_PIN, (GPIO_PinState)(Func_nibble&0x2));
		HAL_GPIO_WritePin(PORT_Func, F3_PIN, (GPIO_PinState)(Func_nibble&0x4));
		//Write the Enable pulse
		MOTOR_EnablePulse();
	}
	else if(mode_8_2_I2C == 2)	//2 bits mode
	{
		//write data to output pins
		//Func data
		HAL_GPIO_WritePin(PORT_Func, F1_PIN, (GPIO_PinState)(Func_nibble&0x1));
		HAL_GPIO_WritePin(PORT_Func, F2_PIN, (GPIO_PinState)(Func_nibble&0x2));
		HAL_GPIO_WritePin(PORT_Func, F3_PIN, (GPIO_PinState)(Func_nibble&0x4));
		//Write the Enable pulse
		MOTOR_EnablePulse();
		
		//MSs data
		HAL_GPIO_WritePin(PORT_MS, S1_PIN, (GPIO_PinState)(MS_nibble&0x1));
		HAL_GPIO_WritePin(PORT_MS, S2_PIN, (GPIO_PinState)(MS_nibble&0x2));
		HAL_GPIO_WritePin(PORT_MS, S3_PIN, (GPIO_PinState)(MS_nibble&0x4));
		//Write the Enable pulse
		MOTOR_EnablePulse();
	}
}

//5) Run Command
static void MOTOR_RunCommand(uint8_t command)
{
	//Set RS to 0
	MOTOR_Reset(false);
	//Call low level write parallel function
	MOTOR_RUN(command);
}
//********** END **********//


//Public functions
//1) LCD begin 8 bits function
void MOTOR_Begin8BIT(GPIO_TypeDef* PORT_Rs_En, uint16_t Rs, uint16_t En, GPIO_TypeDef* MSlave, uint16_t S1, uint16_t S2, uint16_t S3, GPIO_TypeDef* Func, uint16_t F1, uint16_t F2, uint16_t F3)
{
	//Set GPIO Ports and Pins data
	PORT_Reset_and_En = PORT_Rs_En;
	PIN_Reset = Rs;
	PIN_En = En;
	PORT_MS = MSlave;
	S1_PIN = S1;
	S2_PIN = S2;
	S3_PIN = S3;
	
	PORT_Func = Func;
	F1_PIN = F1;
	F2_PIN = F2;
	F3_PIN = F3;
	//Initialise microsecond timer
	MOTOR_TIM_Config();
	//Set the mode to 8 bits
	mode_8_2_I2C = 1;
	//Function set variable to 8 bits mode
	FunctionSet = 0x38;
	
	//Initialise MOTOR
	//1. Wait at least 15ms
	HAL_Delay(20);
}
//2) MOTOR begin 2 bits function
void MOTOR_Begin2BIT(GPIO_TypeDef* Func, uint16_t F2, uint16_t F3)
{
	//Set GPIO Ports and Pins data.......... Only Step(2) & Dir(3) Functions.
	PORT_Func = Func;
	F2_PIN = F2;
	F3_PIN = F3;
	//Initialise microsecond timer
	MOTOR_TIM_Config();
	//Set the mode to 8 bits
	mode_8_2_I2C = 1;
	//Function set variable to 8 bits mode
	FunctionSet = 0x38;
	
	//Initialise MOTOR
	//1. Wait at least 15ms
	HAL_Delay(20);
}

//3) Go_to
void MOTOR_GO(int num)
{
	for(int i=0; i<num; i++)
	{
	
		HAL_GPIO_WritePin(PORT_Func, F2_PIN, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(PORT_Func, F2_PIN, GPIO_PIN_RESET);
		HAL_Delay(5);
	}
	
}
//4) Stop
void MOTOR_Stop(void)
{
	MOTOR_GO(1);
	HAL_Delay(100);
}
//5) Turn
void MOTOR_Turn(void)
{
	HAL_GPIO_TogglePin(PORT_Func, F3_PIN);
	HAL_Delay(100);
}
//6) Full Step
void MOTOR_F_Step(void)
{
	HAL_GPIO_WritePin(PORT_MS, S1_PIN | S2_PIN | S3_PIN, GPIO_PIN_RESET);
}
//7) Half Step
void MOTOR_H_Step(void)
{
	HAL_GPIO_WritePin(PORT_MS, S1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PORT_MS, S2_PIN | S3_PIN, GPIO_PIN_RESET);
}
//8) Quarter Step
void MOTOR_4_Step(void)
{
	HAL_GPIO_WritePin(PORT_MS, S2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PORT_MS, S1_PIN | S3_PIN, GPIO_PIN_RESET);
}
//9) Eighth Step
void MOTOR_8_Step(void)
{
	HAL_GPIO_WritePin(PORT_MS, S3_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PORT_MS, S1_PIN | S2_PIN, GPIO_PIN_SET);
}
//10) Sixteenth Step
void MOTOR_16_Step(void)
{
	HAL_GPIO_WritePin(PORT_MS, S1_PIN | S2_PIN | S3_PIN, GPIO_PIN_SET);
}
//********** END **********//





