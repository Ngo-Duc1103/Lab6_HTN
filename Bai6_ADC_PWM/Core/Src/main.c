/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "uart.h"
#include "draw_line_chart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float info[2];
uint8_t minute[2];
uint8_t second[2];
char powerString[100];
char tempString[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_Buzzer();
void test_Adc();
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 lcd_Clear(BLACK);
 int dur_led = 20;
  while (1)
  {
	  while(!flag_timer2);
	  flag_timer2 = 0;
//	  button_Scan();
//	  test_LedDebug();
//	  test_Adc();
//	  test_Buzzer();
	  LineChart();

	  dur_led--;
	  if (dur_led == 0){
		  dur_led = 20;
		  check_led();
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  uart_init_rs232();
	  sensor_init();
	  buzzer_init();
	  setTimer2(50);
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}

uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}

uint8_t isButtonRight()
{
    if (button_count[11] == 1)
        return 1;
    else
        return 0;
}

uint8_t count_adc = 0;

void check_led(){
	led7_SetDigit(minute[0], 1, 0);
	led7_SetDigit(minute[1], 0, 0);
	led7_SetDigit(second[0], 3, 0);
	led7_SetDigit(second[1], 2, 0);

	second[0]++;
	if (second[0] > 9) {
		second[0] = 0;
		second[1]++;
		if (second[1] > 9) {
			second[1] = 0;
			minute[0]++;
			if (minute[0] > 9) {
				minute[0] = 0;
				minute[1]++;
				if (minute[1] > 9) minute[1] = 0;
			}
		}
	}
}

void test_Adc(){
	led7_SetDigit(minute[0], 0, 0);
	led7_SetDigit(minute[1], 1, 0);
	led7_SetDigit(second[0], 2, 0);
	led7_SetDigit(second[1], 3, 0);
	count_adc = (count_adc + 1)%20;
	if(count_adc == 0){
		second[1]++;
		check_led();
		sensor_Read();
		info[0] = sensor_GetVoltage() * sensor_GetCurrent();
		sprintf(powerString, "%.2f", info[0]);
		info[1] = sensor_GetTemperature();
		// Power consumption
		lcd_ShowStr(10, 100, "Power:", RED, BLACK, 16, 0);
		lcd_ShowStr(130, 100, powerString, RED, BLACK, 16, 0);
		lcd_ShowStr(200, 100, "mW", YELLOW, BLACK, 16, 0);

		// Light
		lcd_ShowStr(10, 120, "Light:", RED, BLACK, 16, 0);
		if (((float)sensor_GetLight() / 4095) > 0.5) {
			lcd_Fill(130, 120, 240, 140, BLACK);
			lcd_ShowStr(130, 120, "Weak", YELLOW, BLACK, 16, 0);
		} else {
			lcd_Fill(130, 120, 240, 140, BLACK);
			lcd_ShowStr(130, 120, "Strong", YELLOW, BLACK, 16, 0);
		}

		// Temperature
		lcd_ShowStr(10, 140, "Temperature:", RED, BLACK, 16, 0);
		lcd_ShowFloatNum(130, 140, info[1], 4, RED, BLACK, 16);
		lcd_ShowStr(200, 140, "oC", YELLOW, BLACK, 16, 0);

		// Humidity
		lcd_ShowStr(10, 160, "Humidity:", RED, BLACK, 16, 0);
		lcd_ShowIntNum(130, 160, sensor_GetPotentiometer() * 100 / 4095 , 2, RED, BLACK, 16);
		lcd_ShowStr(200, 160, "%", YELLOW, BLACK, 16, 0);

		int temp = (int) sensor_GetCurrent() * 100;
		sprintf(tempString, "%d", temp);
		lcd_ShowIntNum(10, 250, temp, 8, WHITE, BLACK, 16);
		lcd_ShowStr(10, 270, tempString, WHITE, BLACK, 16, 0);

//		lcd_ShowIntNum(130, 140, sensor_GetLight(), 4, RED, BLACK, 16);
//		lcd_ShowStr(10, 160, "Potentiometer:", RED, BLACK, 16, 0);
//		lcd_ShowIntNum(130, 160, sensor_GetPotentiometer(), 4, RED, BLACK, 16);
//		lcd_ShowStr(10, 180, "Temperature:", RED, BLACK, 16, 0);
//		lcd_ShowFloatNum(130, 180,sensor_GetTemperature(), 4, RED, BLACK, 16);
	}
}

int isBuzzerOn = 0;
int dur_buzzer = 20;

void test_Buzzer(){
/*	if(isButtonUp()){
		buzzer_SetVolume(50);
	}

	if(isButtonDown()){
		buzzer_SetVolume(0);
	}

	if(isButtonRight()){
		buzzer_SetVolume(25);
	}
*/
	dur_buzzer--;
	if (dur_buzzer == 0){
		dur_buzzer = 20;
		if (sensor_GetPotentiometer() * 100 / 4095 > 70){
			if (isBuzzerOn){
				buzzer_SetVolume(0);
				uart_Rs232SendString("Warning humidity\n");
			}
			else buzzer_SetVolume(25);
			isBuzzerOn = !isBuzzerOn;
		}
		else buzzer_SetVolume(0);
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
