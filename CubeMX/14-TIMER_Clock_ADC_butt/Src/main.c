/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include <stdio.h>  	//printf
#include "string.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef __GNUC__						//Printf function
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
	#ifdef __GNUC__
#define GETCHAR_PROTOTYPE int __io_getchar (void)
#else
#define GETCHAR_PROTOTYPE int fgetc(FILE * f)
#endif /* __GNUC__ */
	
//find scanf imp
	
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//*************adc variables**********//
uint16_t adcval;

//*************clock variables********//
int count1=0;

int seconds=0;
int minutes=0;
int hours=0;

int day=1;
char* date="     Monday     ";
//*************clock set variables****//
int setSEC=0;
int setMIN=0;
int setHRS=0;
//*************alarm set variables****//
int alarmSEC=5;
int alarmMIN=0;
int alarmHRS=0;

//*************LCD menu variables****//
int time4clk=0;
int menu=0;
int setmenu=1;
//*************UART variables*******//
uint8_t dayBuff[1]={'\0'};
uint8_t UARTmenuBuff[1]={'0'};
uint8_t UARTSetmenuBuff[1]={'0'};
uint8_t UARTmenuScale=0;
uint8_t UARTdayScale=0;
uint8_t UARTSetmenuScale=0;
int UARTmenu;
int UARTSetmenu=0;

//*************Functions************//			
char *GetInputPrototype(FILE* i, size_t size){
	char *str;
  int ch;
	size_t len = 0;
  str = realloc(NULL, sizeof(char)*size);
	
	while(EOF!=(ch=fgetc(i)) && ch != '\n'){
		__HAL_UART_CLEAR_OREFLAG(&huart2);
		HAL_UART_Receive(&huart2, (uint8_t *)i, 1, 0xFFF);
		str[len++]=ch;
		if(len==size)
				{
				str = realloc(str, sizeof(char)*(size+=16));
        if(!str)return str;
        }
		
		memset(i, '\0', sizeof(&i));
	}
	str[len++]='\0';

	return realloc(str, sizeof(char)*len);
}			
int GetInput(uint8_t val){
	val= (uint8_t)GetInputPrototype(stdin, sizeof(val));
	return val;
}

void SetHours(uint16_t seHR){
	hours = seHR;
}
void SetMinutes(uint16_t seMI){
	minutes = seMI;
}
void SetSeconds(uint16_t seSE){
	seconds = seSE;
}

void ClearJunk(){
	lcd_setCursor(2,1);
	lcd_send_string("      ");
	lcd_setCursor(2,9);
	lcd_send_data (' ');
	lcd_setCursor(2,12);
	lcd_send_data (' ');
	lcd_setCursor(2,15);
	lcd_send_data (' ');
	lcd_setCursor(2,16);
	lcd_send_data (' ');
}

void Buzzer(){
		HAL_GPIO_WritePin(GPIOB, Buzzer_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOB, Buzzer_Pin, GPIO_PIN_RESET);
}
void Days(){
	switch (day){
		case 1:
			date = "     Monday     "; break;
		case 2:
			date = "     Tuesday    "; break;
		case 3:
			date = "    Wednesday   "; break;
		case 4: 
			date = "    Thursday    "; break;
		case 5:
			date = "     Friday     "; break;
		case 6:
			date = "    Saturday    "; break;
		case 7:
			date = "     Sunday     "; break;
		case 0: 
			date = "     Sunday     "; break; 
		}
	
	lcd_setCursor(1,1);
	lcd_send_string(date);
		
	if(day==7)
	{
		day=0; 
	}
	else if(day>7)
	{
		day=day%7;
	}
	else if(day == 0)
	{
		day++;
	}
}

void Alarm(){
	if(alarmSEC == seconds && alarmMIN == minutes && alarmHRS == hours)
	{
		time4clk=0;	
		HAL_GPIO_WritePin(GPIOB, Buzzer_Pin, GPIO_PIN_SET);
		
		lcd_setCursor(1,1);
		lcd_send_string("  !! ALARM. !!  ");
		printf("!! ALARM. !!  \n");
		HAL_Delay(1000);

	}
}
void UARTsetMenu(){
		if(UARTmenu==1){
			//***For entering the SetMenu(s)*****...gets UARTSetmenu input but doesn't wait........
			printf("\n\r Set Menu:\n\r1.setClock\n\r2.setAlarm\n\r3.setDay\n\r4.Exit\n\t");
			memset(&UARTSetmenu, '\0', sizeof(UARTSetmenu));
			HAL_UART_Receive(&huart2, (uint8_t *)&UARTSetmenuScale, 1, 0xFFFF);
			UARTSetmenu=UARTSetmenuScale-48; 
			//*************************************************************************************
			if(UARTSetmenu==1)//Set Clock
			{
				//***Menu for setting the Clock patameters (HRS:MIN)********************
				printf("\n\rClock Settings:\n\n");		
				printf("\r\tClock: __:__ \n\r\t\t");
				memset(&setHRS, '\0', sizeof(setHRS));
				scanf("%d",&setHRS);						
				printf("\r\tClock: %d:__ \n",setHRS);
				scanf("%d",&setMIN);	
				
				printf("\r\tClock: %d:%d \n",setHRS,setMIN);
				
				hours=setHRS%24;
				minutes=setMIN%60;
				printf("Set Clock to: %d:%d:00",hours, minutes);	//Prints setted clock
				UARTSetmenu++;																//Goes to the next menu automatically 
			}
			
			if(UARTSetmenu==2)//Set Alarm
			{
				//***Menu for setting the Alarm patameters (HRS:MIN:SEC)*****************

				printf("\n\rAlarm Settings:\n\n");
				
				printf("\r\tAlarm: __:__:__ \n\r\t\t");
				memset(&alarmHRS, '\0', sizeof(alarmHRS));
				scanf("%d",&alarmHRS);						
				printf("\r\tAlarm: %d:__:__ \n",alarmHRS);
				memset(&alarmMIN, '\0', sizeof(alarmMIN));
				scanf("%d",&alarmMIN);	
				printf("\r\tAlarm: %d:%d:__ \n",alarmHRS,alarmMIN);
				memset(&alarmSEC, '\0', sizeof(alarmSEC));
				scanf("%d",&alarmSEC);	
				printf("\r\tAlarm: %d:%d:%d \n",alarmHRS,alarmMIN,alarmSEC);
				
				alarmHRS=alarmHRS%24;
				alarmMIN=alarmMIN%60;
				alarmSEC=alarmSEC%60;
				printf("Alarm setted to: %d:%d:%d",alarmHRS, alarmMIN,alarmSEC);	//Prints setted clock
				UARTSetmenu++;																//Goes to the next menu automatically 
			}
			if(UARTSetmenu==3)//Set Day
			{
				//***Menu for setting the Day patameters (Day)********************
				printf("\n\rDay Settings:\n\n");
				scanf("%d",(int *)&dayBuff[0]);
				//....Need a scan function that waits........	
				day=dayBuff[0];
				Days();																	//Int to Day (switch case function)
				printf("Set Day to: %d - %s",day, date);//Prints setted day (int and day)
				
				UARTSetmenu++;																 //Goes to the next menu automatically 
			}
			if(UARTSetmenu==4)//Exit
			{
				//***Exit Menu for making the Clock ON********************
				UARTmenu=0;
				seconds=0;
				printf("\n\rExit Settings, Clock:ON\n\n");
			}

		}
		else{
		//If anything comes except 1 for UARTmenu, it becomes 0, and clock continues..
		UARTmenu=0;
		}
}
	
void ScaleandSet(){
	UARTmenuScale=UARTmenuBuff[0]-33;
	UARTdayScale=dayBuff[0]-48;
	UARTSetmenuScale=UARTSetmenuBuff[0]-48;
	
	UARTmenu=UARTmenuScale;
	day=UARTdayScale;
	UARTSetmenu=UARTSetmenuScale;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	HAL_ADC_Start_IT(&hadc1);

	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	
	lcd_init();
//	lcd_clear();

	SetHours(23);
	SetMinutes(59);
	SetSeconds(55);
	
	printf("\r\nLets cook the clock..");
	UARTmenu=0;
	UARTmenuScale=48;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		//***********************CLOCK Stuff********************************//
		if(menu == 0)				 //Clock menu
		{
	
			if(seconds == 60 || seconds > 60)				//count minutes
			{	
				minutes++;
				seconds=0;
				HAL_GPIO_TogglePin(GPIOD, Led3_Pin);
				
			}
			if(minutes==60 || minutes > 60)				//count hours
			{	
				hours++;
				minutes=0;				//5mins = 1hour ( just for trying faster) ...for real hour; ==36000
				HAL_GPIO_TogglePin(GPIOD, Led4_Pin);
				
			}
			if(hours == 24)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_All, GPIO_PIN_SET);		
				hours=0;
				minutes=0;
				seconds=0;
				
				day++;
				Days();
				Buzzer();
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_All, GPIO_PIN_RESET);
			}
			//**********************************LCD i2c Stuff*************************//
			lcd_setCursor(1,1);
			lcd_send_string("Saat: Sa Dk Sn");
			
			if(hours<10){
				lcd_setCursor(2,7);
				lcd_PrintInt(0);
				lcd_setCursor(2,8);
				lcd_PrintInt(hours);
			}
			else
			{
				lcd_setCursor(2,7);
				lcd_PrintInt(hours);
			}
			if(minutes<10){
				lcd_setCursor(2,10);
				lcd_PrintInt(0);
				lcd_setCursor(2,11);
				lcd_PrintInt(minutes);
			}
			else
			{
				lcd_setCursor(2,10);
				lcd_PrintInt(minutes);
			}
			if(seconds<10){
				lcd_setCursor(2,13);
				lcd_PrintInt(0);
				lcd_setCursor(2,14);
				lcd_PrintInt(seconds);
			}
			else
			{
				lcd_setCursor(2,13);
				lcd_PrintInt(seconds);
			}
			
		}
		else if(menu%2 == 1) //Clock set menu
		{
			lcd_setCursor(1,1);
			lcd_send_string("Ayar: Sa Dk Sn");
			lcd_setCursor(2,1);
			lcd_send_string("S:");
			if(setmenu%3 == 1)
			{
				setmenu++;
			}
			else if(setmenu%3 == 2)
			{
				lcd_setCursor(2,3);
				lcd_send_string("MIN");
				if(setMIN<10)
				{
					lcd_setCursor(2,10);
					lcd_PrintInt(0);
					lcd_setCursor(2,11);
					lcd_PrintInt(setMIN);
				}
				else
				{
					lcd_setCursor(2,10);
					lcd_PrintInt(setMIN);
				}
				minutes=setMIN;
			}
			
			else if(setmenu%3 == 0)
			{
				lcd_setCursor(2,3);
				lcd_send_string("HRS");
				if(setHRS<10)
				{
					lcd_setCursor(2,7);
					lcd_PrintInt(0);
					lcd_setCursor(2,8);
					lcd_PrintInt(setHRS);
				}
				else
				{
					lcd_setCursor(2,7);
					lcd_PrintInt(setHRS);
				}
				hours=setHRS;
			}

			else
			{
				if(time4clk == 5)
				{
					menu=0;
					seconds=0;
				}
			}
			
			HAL_Delay(1000);
			seconds+=5;
		}
		else if(menu%2 == 0) //Alarm menu
		{
			lcd_setCursor(1,1);
			lcd_send_string("Alrm: Sa Dk Sn");
			lcd_setCursor(2,1);
			lcd_send_string("S:");
			
			if(setmenu%3 == 1)
			{
				lcd_setCursor(2,3);
				lcd_send_string("SEC");
				if(setSEC<10)
				{
					lcd_setCursor(2,13);
					lcd_PrintInt(0);
					lcd_setCursor(2,14);
					lcd_PrintInt(setSEC);
				}
				else
				{
					lcd_setCursor(2,13);
					lcd_PrintInt(setSEC);
				}
				alarmSEC=setSEC;
			}
			else if(setmenu%3 == 2)
			{
				lcd_setCursor(2,3);
				lcd_send_string("MIN");
				if(setMIN<10)
				{
					lcd_setCursor(2,10);
					lcd_PrintInt(0);
					lcd_setCursor(2,11);
					lcd_PrintInt(setMIN);
				}
				else
				{
					lcd_setCursor(2,10);
					lcd_PrintInt(setMIN);
				}
				alarmMIN=setMIN;
			}
			
			else if(setmenu%3 == 0)
			{
				lcd_setCursor(2,3);
				lcd_send_string("HRS");
				if(setHRS<10)
				{
					lcd_setCursor(2,7);
					lcd_PrintInt(0);
					lcd_setCursor(2,8);
					lcd_PrintInt(setHRS);
				}
				else
				{
					lcd_setCursor(2,7);
					lcd_PrintInt(setHRS);
				}
				alarmHRS=setHRS;
			}

			else
			{
				if(time4clk == 5)
				{
					menu=0;
					seconds=0;
				}
			}
			HAL_Delay(1000);
		}
		//***********************UART Stuff********************************//
	//	memset(&UARTmenuScale,'\0',1);
		HAL_UART_Receive(&huart2, (uint8_t *)&UARTmenuScale, 1, 50);
		UARTmenu=UARTmenuScale-48;
		if(UARTmenu==1){ goto jump; }
		
		jump2:
		//***********************Alarm Stuff********************************//
		Alarm();
		if(HAL_GPIO_ReadPin(GPIOB, Buzzer_Pin)==1) //Alarm mute
		{	
			if(setmenu%3 == 2 || time4clk == 5)
			{
				HAL_GPIO_WritePin(GPIOB, Buzzer_Pin, GPIO_PIN_RESET);
				printf("Alarm Muted.  \n");
				setmenu=1;
			}
		}

		
		ClearJunk(); //Clears junks or memory leaks from the lcd
		
  }
	jump:
	UARTsetMenu();
	printf("...Quitted...");
	if(UARTSetmenu==4){
	UARTmenuScale=48;
	goto jump2; }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 8400;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 100;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 8400;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8400;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 10000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ButtonB_Pin */
  GPIO_InitStruct.Pin = ButtonB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ButtonB_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Button_Pin */
  GPIO_InitStruct.Pin = Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Led1_Pin Led2_Pin Led3_Pin Led4_Pin */
  GPIO_InitStruct.Pin = Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE									//printf def to uart
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

GETCHAR_PROTOTYPE
{
  uint8_t ch = 0;
  // Clear the Overrun flag just before receiving the first character
  __HAL_UART_CLEAR_OREFLAG(&huart2);
 
  HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
	
	if(htim->Instance == TIM2) //01. sec
  {
		HAL_GPIO_TogglePin(GPIOD, Led1_Pin);
	
		count1++;
		
		if(count1 == 10)
		{
			HAL_GPIO_TogglePin(GPIOD, Led2_Pin);
			seconds++;
			count1=0;	
	
			if(UARTmenu==0)
			{
				printf("\n\r Day: %s	Saat: %d:%d:%d 	Alarm: %d:%d:%d\t",date,hours%24,minutes%60,seconds%60 , alarmHRS, alarmMIN, alarmSEC);	
			}
		}
	}
	
	
	if(htim->Instance == TIM4)
  {
		time4clk++;
	}
	
	
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ConvCpltCallback could be implemented in the user file
   */
	//adcval= HAL_ADC_GetValue(&hadc1);
	if(setmenu%3 == 1)
	{
		adcval= HAL_ADC_GetValue(&hadc1);
		setSEC=(adcval+1)/68.26666666666666666666666666666;
	}
	else if(setmenu%3 == 2)
	{
		adcval= HAL_ADC_GetValue(&hadc1);
		setMIN=(adcval+1)/68.26666666666666666666666666666;
	}
	else	if(setmenu%3 == 0)
	{
		adcval= HAL_ADC_GetValue(&hadc1);
		setHRS=(adcval+1)/170.6666666666666666666666666666;
	}
	else
		adcval=0;
	
	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  //	UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
	TIM4->CNT=0;
	if(HAL_GPIO_ReadPin(GPIOA, ButtonB_Pin))
	{
	menu++;
	time4clk=0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB, Button_Pin))
	{
	//while(HAL_GPIO_ReadPin(GPIOB, Button_Pin));
	setmenu++;
	time4clk=0;
	}
	
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance== USART2)
	{
		printf("cum");
		
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
