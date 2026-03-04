/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "share.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
__attribute__((section(".shared_ram")))
SharedMemory_t s;
void Board1_M4_transmit(float , float , float , int );
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EncodeTransmit */
osThreadId_t EncodeTransmitHandle;
const osThreadAttr_t EncodeTransmit_attributes = {
  .name = "EncodeTransmit",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for timerBinarySem01 */
osSemaphoreId_t timerBinarySem01Handle;
const osSemaphoreAttr_t timerBinarySem01_attributes = {
  .name = "timerBinarySem01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Encodetransmit(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of timerBinarySem01 */
  timerBinarySem01Handle = osSemaphoreNew(1, 1, &timerBinarySem01_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of EncodeTransmit */
  EncodeTransmitHandle = osThreadNew(Encodetransmit, NULL, &EncodeTransmit_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Encodetransmit */
/**
* @brief Function implementing the EncodeTransmit thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Encodetransmit */
void Encodetransmit(void *argument)
{
  /* USER CODE BEGIN Encodetransmit */
  /* Infinite loop */
  for(;;)
  {
	  if(osSemaphoreAcquire(timerBinarySem01Handle, osWaitForever) == osOK){

			  Board1_M4_transmit(s.min,s.max,s.avg,0);
	  }
    osDelay(1);
  }
  /* USER CODE END Encodetransmit */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Board1_M4_transmit(float min, float max, float avg, int event){
	char msg[50];


	memset(msg, 0, sizeof(msg));

	snprintf(msg, sizeof(msg), "MIN:%.2f,MAX:%.2f,AVG:%.2f,EVT:%d\r\n", min, max, avg, event);


	if (HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg)-1, 1000) == HAL_OK){
		printf(msg);
	}


}


/* USER CODE END Application */

