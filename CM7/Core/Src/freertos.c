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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "share.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SH_MEM_SEM_ID 1
__attribute__((section(".shared_ram")))
SharedMemory_t s;
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
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* Internal Temp Sensor Calibration Addresses for H755 */
#define TS_CAL1_ADDR ((uint16_t*)0x1FF1E820) // 30°C
#define TS_CAL2_ADDR ((uint16_t*)0x1FF1E840) // 110°C
#define TS_VREF      3300.0f                 // Calibration Voltage in mV

float buff[61];
volatile int count = 0;


//  float findAvg(float *buff, int count);
//  void leftShiftAndAdd(float * buff,int loc_to_add,float val_to_add);

float findTemp(uint32_t adc_raw_value ){
	float temperature =0.0f;
    // Get factory calibration values from system memory
	uint16_t ts_cal1 = *TS_CAL1_ADDR;
    uint16_t ts_cal2 = *TS_CAL2_ADDR;
    // Linear interpolation formula
    temperature = (float)(110.0f - 30.0f) / (ts_cal2 - ts_cal1);
    temperature *= (adc_raw_value - ts_cal1);
    temperature += 30.0f;

    return temperature;
}


float findAvg(float *buff, int count)
{
  if (count == 0) return 0.0f;
  float sum = 0.0f;
  for (int i = 0; i < count; i++)
  {
    sum += buff[i];
  }
  return sum / count;
}

void leftShiftAndAdd(float * buff,int loc_to_add,float val_to_add)
{
  int i = 0;
  while (i < loc_to_add)
  {
    buff[i] = buff[i + 1];
    i++;
  }
  buff[loc_to_add] = val_to_add;
}


/* USER CODE END FunctionPrototypes */
void StartDefaultTask(void *argument);

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
    volatile float current_temperature=0.0f;
    uint32_t adc_raw_value = 0;
    int sample_arr[7]={500,1000,5000,10000,20000,30000,60000};
	  int length = (int)(60000 / sample_arr[s.sample_rate_index]);
	  float max=0.0f , min=999.0f;
	  float avg=0;


  /* Infinite loop */
  for(;;)
  {
	  HAL_ADC_Start(&hadc3);
	  if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK) {
	      adc_raw_value = HAL_ADC_GetValue(&hadc3);


	      while (HAL_HSEM_FastTake(SH_MEM_SEM_ID) != HAL_OK);

	      current_temperature = findTemp(adc_raw_value);


	      // Min, max calculation
	      if(current_temperature < min){
	    	  min = current_temperature;
	      }
	      if(current_temperature > max)
	      {
	      	    	  max = current_temperature;
	      }



	      s.min=min;
	      s.max=max;


	      // Storing it into a buffer

	      	      if (count < length)
	      	      {
	      	        buff[count] = current_temperature;
	      	        count++;
	      	        // continue;
	      	      }else{
	      	        // tempPrint(temperature);
	      	        leftShiftAndAdd(buff,length-1,current_temperature);
	      	      }
	      	      avg= findAvg(buff,count);
	      	      s.avg=avg;


	      	      // To place it in shared memory
/*
	              while (shared_data.cm7_updated == 1)
	              {
	                  HAL_Delay(10);  // wait for CM4 to read
	              }
*/

	    	      HAL_HSEM_Release(SH_MEM_SEM_ID, 0);
	    	      osDelay((sample_arr[s.sample_rate_index]));

	  }
	  HAL_ADC_Stop(&hadc3);
}

  }
  /* USER CODE END StartDefaultTask */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

