/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "os.h"
#include "SEGGER_SYSVIEW.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_TASK_BLINK_STK_SIZE 128		// Task's Stack Size


#define APP_TASK_BLINK_BLUE_PRIO 5		// Blue LED Task priority
#define APP_TASK_BLINK_GREEN_PRIO 4		// Green LED Task priority. Lower value is higher priority.
#define APP_TASK_STARTER_PRIO 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Task Control Blocks */
static OS_TCB AppTask1_TCB;
static OS_TCB AppTask2_TCB;
static OS_TCB AppTaskStarter_TCB;

/* Task stacks. */
static CPU_STK AppTask1Stk[APP_TASK_BLINK_STK_SIZE];
static CPU_STK AppTask2Stk[APP_TASK_BLINK_STK_SIZE];
static CPU_STK AppTaskStarterStk[APP_TASK_BLINK_STK_SIZE];

/* A semaphore for broadcasting tasks to start. */
static OS_SEM StartSem;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void AppTask1(void *p_arg);
static void AppTask2(void *p_arg);
static void AppTaskStarter(void *p_arg);

static void checkError(OS_ERR *err);
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
	OS_ERR err;
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
  /* USER CODE BEGIN 2 */

  /* Initialize CPU */
  CPU_Init();

  /* Initialize the OS */
  OSInit(&err);
  checkError(&err);

  SEGGER_SYSVIEW_Conf();
  SEGGER_SYSVIEW_Start();

  SEGGER_SYSVIEW_Print("SystemView Trace started.");

  /* Create task starter broadcaster semaphore */
  OSSemCreate(&StartSem, "Start Semaphore", 0, &err);
  checkError(&err);


  /* Create starter wrapper task. */
  OSTaskCreate((OS_TCB*) &AppTaskStarter_TCB,
			(CPU_CHAR*) "App Task Start",
			(OS_TASK_PTR) AppTaskStarter,
			(void*) 0,
			(OS_PRIO) APP_TASK_STARTER_PRIO,
			(CPU_STK*) &AppTaskStarterStk[0],
			(CPU_STK_SIZE) 0,
			(CPU_STK_SIZE) APP_TASK_BLINK_STK_SIZE,
			(OS_MSG_QTY) 0,
			(OS_TICK) 0,
			(void*) 0,
			(OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
			(OS_ERR*) &err);
	checkError(&err);






  	/* Start OS */
  	OSStart(&err);
  	checkError(&err);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_14|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB14 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_14|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* A main wrapper task for starting the other task.
 * This task initializes Systick and creates all other tasks. */
static void AppTaskStarter(void *p_arg){
	OS_ERR err;

	SEGGER_SYSVIEW_Print("AppTaskStarter is started.");


	/* Initialize Systick. This function has to be called after OSStart() call. */
	OS_CPU_SysTickInit(SystemCoreClock / OS_CFG_TICK_RATE_HZ);


	/* Create blink tasks. */
	OSTaskCreate((OS_TCB*) &AppTask1_TCB,
	  			(CPU_CHAR*) "App Task 1",
	  			(OS_TASK_PTR) AppTask1,
	  			(void*) 0,
	  			(OS_PRIO) APP_TASK_BLINK_GREEN_PRIO,
	  			(CPU_STK*) &AppTask1Stk[0],
	  			(CPU_STK_SIZE) 0,
	  			(CPU_STK_SIZE) APP_TASK_BLINK_STK_SIZE,
	  			(OS_MSG_QTY) 0,
	  			(OS_TICK) 0,
	  			(void*) 0,
	  			(OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
	  			(OS_ERR*) &err);
	checkError(&err);




	OSTaskCreate((OS_TCB*) &AppTask2_TCB,
			(CPU_CHAR*) "App Task 2",
			(OS_TASK_PTR) AppTask2,
			(void*) 0,
			(OS_PRIO) APP_TASK_BLINK_BLUE_PRIO,
			(CPU_STK*) &AppTask2Stk[0],
			(CPU_STK_SIZE) 0,
			(CPU_STK_SIZE) APP_TASK_BLINK_STK_SIZE,
			(OS_MSG_QTY) 0,
			(OS_TICK) 0,
			(void*) 0,
			(OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
			(OS_ERR*) &err);
	checkError(&err);



	SEGGER_SYSVIEW_Print("Tasks are created.");

	/* Broadcast all the tasks to start working. */
	OSSemPost(&StartSem, OS_OPT_POST_ALL, &err);
	checkError(&err);


	while(1){

	}
}

/* A high priority task that toggles Green LED.
 * It is suspended for 5 seconds, preempts the other task when
 * ready to run and keep the Green LED on for 3 seconds*/
static void AppTask1(void *p_arg) {
	OS_ERR err;
	CPU_TS ts;

	SEGGER_SYSVIEW_Print("AppTask1 is started.");


	SEGGER_SYSVIEW_Print("Task1: Waiting on semaphore.");
	/* Waiting for all tasks to be created. */
	OSSemPend(&StartSem, 0, OS_OPT_PEND_BLOCKING, &ts, &err);
	checkError(&err);											// Check error

	SEGGER_SYSVIEW_Print("Task1: Ready.");


	while (1){
		SEGGER_SYSVIEW_Print("Task1: Going to suspend.");
		OSTimeDlyHMSM(0, 0, 5, 0, OS_OPT_TIME_HMSM_STRICT, &err);	// Sleep for 5 seconds.
		checkError(&err);											// Check error

		SEGGER_SYSVIEW_Print("Task1: Ready.");

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);						// Turn on the Green LED
		SEGGER_SYSVIEW_Print("Task1: Green LED turned on.");

		HAL_Delay(3000);											// Keep the LED on.

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);						// Turn off the Green LED
		SEGGER_SYSVIEW_Print("Task1: Green LED turned off.");
	}
}

/* A low priority task that toggles Blue LED.
 * This task is always ready to run and blinks the blue LED.*/
static void AppTask2(void *p_arg) {
	OS_ERR err;
	CPU_TS ts;

	SEGGER_SYSVIEW_Print("AppTask2 is started.");


	SEGGER_SYSVIEW_Print("Task2: Waiting on semaphore.");
	/* Waiting for all tasks to be created. */
	OSSemPend(&StartSem, 0, OS_OPT_PEND_BLOCKING, &ts, &err);
	checkError(&err);											// Check error

	SEGGER_SYSVIEW_Print("Task2: Ready.");

	while (1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);			// Toggle Blue LED
		SEGGER_SYSVIEW_Print("Task2: Blue LED is toggled..");

		HAL_Delay(500);

	}
}


/* This function checks the error code returned by OS services.*/
/* If there is an error, the procedure doesn't continue and the red LED is blinking*/
static void checkError(OS_ERR *err){
	if (*err != OS_ERR_NONE) {
		while (1) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
			HAL_Delay(200);

		}
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
