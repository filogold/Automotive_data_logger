/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define R19overR17 1.6
#define Vdd 3.3
#define ParallelR18R17 909.1
#define R16 10000
#define R19 16000
#define ADCMAX 4095.0 // (2^12-1) Maximum value for 12-bit ADC
#define beta 3.85 // Coefficient of Pt1000 [Ω/°C]
#define R_0deg 1000 // Electrical nominal resistance at 0°C [Ω]

#define map(x, in_min, in_max, out_min, out_max)  ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t pippo = 0;
uint32_t adcValue = 0;

char stat1 = '0';
char stat2 = '0';
char switch_status = '0';
const char *switch_status_string;
const char *status_charger_strings[] = {
        "Charge completed",
        "Charging",
        "Recoverable fault",
        "Non-recoverable fault",
		"External power supply disconnected",
		"Battery not connected"
    };
int status_charger;

char buffer1 [150];
char buffer2 [150];
char timestamp [50];


ADC_ChannelConfTypeDef adcPT1000Channel = {0};
ADC_ChannelConfTypeDef adcVInChannel = {0};

uint16_t adcVin;
uint16_t adcPT1000;
uint16_t meanadcPT1000;
uint16_t sumadcPT1000;
int meancount = 0;

int battery_dis = 0;
static uint32_t millisStat = 0;

float RPt1000;
float meanPT1000;
float pt1000;
float vIn;


uint16_t pt1000_CAN = 0;
uint16_t vIn_CAN = 0;
uint8_t digIn_CAN = 0;

RTC_DateTypeDef getDate = {0};
RTC_TimeTypeDef getTime = {0};


CAN_TxHeaderTypeDef 	pHeader;
uint32_t 				TxMailbox;
uint8_t canTxBuff[8] = {0};

uint8_t uartStatus = 0;
uint8_t uartTxBuff[8];
uint8_t usartRxBuff[8];

uint8_t printMode = 0;
uint32_t interrupt = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

char Read_Save_Pin_Status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
float Convert_Pt1000_RawData(uint16_t Pt1000ADC);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  stat1 = Read_Save_Pin_Status(GPIOA, GPIO_PIN_8);
  stat2 = Read_Save_Pin_Status(GPIOB, GPIO_PIN_14);
  switch_status = Read_Save_Pin_Status(GPIOA, GPIO_PIN_10);

  pHeader.DLC = 8; 						//give message size of 1 byte
  pHeader.IDE = CAN_ID_STD; 				//set identifier to standard
  pHeader.RTR = CAN_RTR_DATA; 				//set data type to remote transmission request?
  pHeader.StdId = 0x300; 					//define a standard identifier, used for message identification by filters (switch this for the other microcontroller)

  HAL_CAN_Start(&hcan1);

  HAL_UART_Receive_IT(&huart1, usartRxBuff, 8);


  adcPT1000Channel.Channel = ADC_CHANNEL_8;
  adcPT1000Channel.Rank = ADC_REGULAR_RANK_1;
  adcPT1000Channel.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
  adcPT1000Channel.SingleDiff = ADC_SINGLE_ENDED;
  adcPT1000Channel.OffsetNumber = ADC_OFFSET_NONE;
  adcPT1000Channel.Offset = 0;
  HAL_ADC_ConfigChannel(&hadc1, &adcPT1000Channel);

  adcVInChannel.Channel = ADC_CHANNEL_10;
  adcVInChannel.Rank = ADC_REGULAR_RANK_1;
  adcVInChannel.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
  adcVInChannel.SingleDiff = ADC_SINGLE_ENDED;
  adcVInChannel.OffsetNumber = ADC_OFFSET_NONE;
  adcVInChannel.Offset = 0;
  HAL_ADC_ConfigChannel(&hadc1, &adcVInChannel);





  HAL_TIM_Base_Start_IT(&htim1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  static uint32_t millisLedBlink = 0;
//
//	  if(millis >= millisLedBlink + 100)
//	  {
//		  adcValue = HAL_ADC_GetValue(&hadc1);
//
//		  //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//		  pippo++;
//		  millisLedBlink = millis;
//
//		  HAL_ADC_Start(&hadc1);
//	  }


	  static uint32_t millisUart = 0;


	  if(printMode == 0)
	  {
		  if(millis >= millisUart + 1000)
		  {
			  // ASSOCIATE THE SOURCE INFORMATION TO THE SWITCH_STATUS
			  if (switch_status == '1') {
			          switch_status_string = "Battery";
			  }
			  else{
			          switch_status_string = "External Power";
			  }

			  // ASSOCIATE THE SOURCE INFORMATION TO THE BATTERY CHARGER
			  if (switch_status == '1') {
				  status_charger = 4;
			  }
			  else if(battery_dis == '1'){
				  status_charger = 5;
			  }
			  else {
				  if (stat1 == '1' && stat2 == '1') {
					  status_charger = 0; // Combination: 11
				  } else if (stat1 == '1' && stat2 == '0') {
					  status_charger = 2; // Combination: 10
				  } else if (stat1 == '0' && stat2 == '1') {
					  status_charger = 1; // Combination: 01
				  } else{
					  status_charger = 3; // Combination: 00
				  }
			  }


			  // GENERATE THE STRING TO PRINT TO UART

			  HAL_RTC_GetTime(&hrtc, &getTime, RTC_FORMAT_BIN);
			  HAL_RTC_GetDate(&hrtc, &getDate, RTC_FORMAT_BIN);
			  sprintf(timestamp,"DATE: %02d/%02d/%02d TIME: %02d:%02d:%02d\r\n", getDate.Date, getDate.Month, getDate.Year, getTime.Hours, getTime.Minutes, getTime.Seconds);
			  HAL_UART_Transmit(&huart1,(uint8_t *)timestamp, strlen(timestamp), 100);


			  sprintf(buffer1, "PT1000 READING: %.2f \r\nANALOG READING: %.2f \r\nDIGITAL READING: %d \r\n", meanPT1000, vIn, digIn_CAN);
			  HAL_UART_Transmit(&huart1,(uint8_t *)buffer1, strlen(buffer1), 100);
			  sprintf(buffer2, "CHARGER STATE: %s \r\nPOWER SUPPLY: %s \r\n\n", status_charger_strings[status_charger], switch_status_string);
			  HAL_UART_Transmit(&huart1,(uint8_t *)buffer2, strlen(buffer2), 100);

			  HAL_CAN_AddTxMessage(&hcan1, &pHeader, canTxBuff, &TxMailbox);


			  millisUart = millis;
		  }
	  }
	  else
	  {
		  if(millis >= millisUart + 100)
		  {
	  //		  uartTxBuff[0] = pt1000 >> 8;
	  //		  uartTxBuff[1] = pt1000;
	  //		  uartTxBuff[2] = vIn >> 8;
	  //		  uartTxBuff[3] = vIn;
	  //		  uartTxBuff[4] = digIn;
	  //		  uartTxBuff[5] = 0;
	  //		  uartTxBuff[6] = 0;
			  uartTxBuff[1] = '\n';

			  uartTxBuff[0] = 'b';

			  //HAL_CAN_AddTxMessage(&hcan1, &pHeader, canTxBuff, &TxMailbox);

			  uartStatus = HAL_UART_Transmit(&huart1, uartTxBuff, 8, 100);

			  millisUart = millis;
		  }
	  }

  }
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

char Read_Save_Pin_Status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	char status;
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET){
		status = '1';
	}
	else if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET){
		status = '0';
	}
	else {
		status = 'F';
	}
	return status;
}

uint16_t pt1000Counter = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	interrupt++;

	adcVin = HAL_ADC_GetValue(&hadc1);
	vIn = map((float) (adcVin), (float) (770), (float) (3993), (float) (1), (float) (5));
	vIn_CAN = (uint16_t)(vIn * 100);

	pt1000Counter++;

	if(pt1000Counter >= 100)
	{
		HAL_ADC_ConfigChannel(&hadc1, &adcPT1000Channel);
		HAL_ADC_Start(&hadc1);

		HAL_ADC_PollForConversion(&hadc1, 100);
		adcPT1000 = HAL_ADC_GetValue(&hadc1);

		pt1000 = Convert_Pt1000_RawData((float)(adcPT1000));
		pt1000_CAN = (uint16_t)(pt1000 * 100);

		if(meancount <9)
		{
			sumadcPT1000 += adcPT1000;
			meancount ++;
		}
		else
		{
			sumadcPT1000 += adcPT1000;
			meanPT1000 = sumadcPT1000/10;
			meanPT1000 = Convert_Pt1000_RawData((float)(meanPT1000));
			meancount = 0;
			sumadcPT1000 = 0;

		}


		HAL_ADC_ConfigChannel(&hadc1, &adcVInChannel);
		HAL_ADC_Start(&hadc1);

		pt1000Counter = 0;

		digIn_CAN = HAL_GPIO_ReadPin(DIGIN_GPIO_Port, DIGIN_Pin);
	}
	else
	{
		HAL_ADC_Start(&hadc1);
		/* Eseguiamo alla fine la lettura dell'ADC alla fine del timer, in questo modo il tempo perso per eseguire l'operazione di lettura
		 * dell'ADC viene fatto dopo aver letto e trasmesso i valori. Lo svantaggio è quello di avere un valore letto circa 1ms prima .*/
	}



	//CAN Tx message

	canTxBuff[0] = pt1000_CAN >> 8;
	canTxBuff[1] = pt1000_CAN;
	canTxBuff[2] = vIn_CAN >> 8;
	canTxBuff[3] = vIn_CAN;
	canTxBuff[4] = digIn_CAN;
	//canTxBuff[5] = 0;
	//canTxBuff[6] = 0;
	//canTxBuff[7] = 0;

	HAL_CAN_AddTxMessage(&hcan1, &pHeader, canTxBuff, &TxMailbox);



}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	stat1 = Read_Save_Pin_Status(GPIOA, GPIO_PIN_8);
	stat2 = Read_Save_Pin_Status(GPIOB, GPIO_PIN_14);
	switch_status = Read_Save_Pin_Status(GPIOA, GPIO_PIN_10);

	if(millis <= millisStat + 400){
		battery_dis = 1;
	}
	else{
		battery_dis = 0;
	}
	millisStat = millis;
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, usartRxBuff, 8);

	if(usartRxBuff[0] == '1')
		printMode = 1;
	else
		printMode = 0;
}


float Convert_Pt1000_RawData(uint16_t Pt1000ADC)
{
	float temperature;
	float Vout = (float) ((Vdd/ADCMAX)*Pt1000ADC);
	float alpha = (float) ((R19overR17 + (Vout/Vdd))*(ParallelR18R17/(ParallelR18R17+R19)));
	RPt1000 = (float) ((alpha*R16)/(1-alpha));

	temperature = (float) ((RPt1000-R_0deg)/beta);
	return temperature;
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
