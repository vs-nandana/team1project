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
void Board1_M4_transmit(float , float , float , int , int, int);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEBOUNCE_MS   0U

static uint32_t last_tick_sw1 = 0U;
static uint32_t last_tick_sw2 = 0U;
static uint32_t last_tick_sw3 = 0U;
static uint32_t last_tick_sw4 = 0U;



#define SW1_PIN   GPIO_PIN_9
#define SW1_PORT  GPIOE

#define SW2_PIN   GPIO_PIN_11
#define SW2_PORT  GPIOE

#define SW3_PIN   GPIO_PIN_13
#define SW3_PORT  GPIOE

#define SW4_PIN   GPIO_PIN_15
#define SW4_PORT  GPIOF

static void Notify_M7_via_HSEM(uint32_t hsem_id);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
int flag = 0;
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

			  Board1_M4_transmit(s.min,s.max,s.avg,0, s.sample_rate_index,s.is_run);
	  }
    osDelay(1);
  }
  /* USER CODE END Encodetransmit */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Board1_M4_transmit(float min, float max, float avg, int event , int smp, int run){
	char msg[50];


	memset(msg, 0, sizeof(msg));

	snprintf(msg, sizeof(msg), "MIN:%.2f,MAX:%.2f,AVG:%.2f,EVT:%d,SMP:%d,RUN:%d\r\n", min, max, avg, event, smp, run);

	printf(msg);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg)-1, 1000);
//	if (HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg)-1, 1000) == HAL_OK){
//		printf("Transmitted\r\n");
//	}


}

static void Notify_M7_via_HSEM(uint32_t hsem_id)
{

    while (HAL_HSEM_FastTake(hsem_id) != HAL_OK){}
    HAL_HSEM_Release(hsem_id,0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t now = HAL_GetTick();
    if (GPIO_Pin == SW1_PIN)
    {
        if ((now - last_tick_sw1) < DEBOUNCE_MS) return;
        last_tick_sw1 = now;

        s.is_run ^= 1U;
        //Notify_M7_via_HSEM(2);
        Board1_M4_transmit(s.min, s.max, s.avg, 1 ,s.sample_rate_index, s.is_run);
    }

    else if (GPIO_Pin == SW2_PIN)
    {
        if ((now - last_tick_sw2) < DEBOUNCE_MS) return;
        last_tick_sw2 = now;

        s.instant_tx= 1U;

        Notify_M7_via_HSEM(3);
        Board1_M4_transmit(s.min, s.max, s.avg, 3, s.sample_rate_index, s.is_run);
    }

    else if (GPIO_Pin == SW3_PIN)
    {
        if ((now - last_tick_sw3) < DEBOUNCE_MS) return;
        last_tick_sw3 = now;

        s.reset_stats = 1;

        Board1_M4_transmit(0.0f, 0.0f, 0.0f, 4,s.sample_rate_index, s.is_run);
        Notify_M7_via_HSEM(4);
    }

    else if (GPIO_Pin == SW4_PIN)
    {
        if ((now - last_tick_sw4) < DEBOUNCE_MS) return;
        last_tick_sw4 = now;
        s.sample_rate_index = (s.sample_rate_index + 1 ) % 7 ;
        Notify_M7_via_HSEM(5);
        Board1_M4_transmit(s.min, s.max, s.avg, 2,s.sample_rate_index, s.is_run);
    }
}
/* USER CODE END Application */

