/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "can.h"
#include "GM6020.h"
#include "PID.h"
#include "math.h"
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
osThreadId CAN_DIAN_JIHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void CAN_DJ(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of CAN_DIAN_JI */
  osThreadDef(CAN_DIAN_JI, CAN_DJ, osPriorityNormal, 0, 128);
  CAN_DIAN_JIHandle = osThreadCreate(osThread(CAN_DIAN_JI), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_CAN_DJ */
/**
  * @brief  Function implementing the CAN_DIAN_JI thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_CAN_DJ */
void CAN_DJ(void const * argument)
{
  /* USER CODE BEGIN CAN_DJ */
  /* Infinite loop */	
  extern GM6020_Statu GM_Statu[4];
 static int16_t obVoltage[4];//相对值电�?? -15000 ~ +15000
//test:
  obVoltage[0] = 0;
  //速度环
  static float TargetSpeed = 0;//rpm:每分钟转速
	PID_Data PID_Speed;
/*
  PID_Speed.Kp = 5;
  PID_Speed.Ki = 0.0001;
	PID_Speed.Kd = 13;
  PID_Speed.errPlus = 0;
  PID_Speed.LastTime = 0;
  PID_Speed.LastData = 0;
  PID_Speed.Limit_Max = 1000;
  PID_Speed.Limit_Min = -1000;
  PID_Speed.Put_Max =  15000;
  PID_Speed.Put_Min = -15000;
*/
  PID_Init(&PID_Speed,5,0.0001,13,1000,-1000,15000,-15000);
	
  //位置环
  static float TargetAngle = 0; //目标角度(单位°)
  static int16_t MechanicalAngle =  0 ;
  PID_Data PID_Angle;
/*
  PID_Angle.Kp = 0.25;
  PID_Angle.Ki = 0;
  PID_Angle.Kd = 0;
  PID_Angle.errPlus = 0;
  PID_Angle.LastTime = 0;
  PID_Angle.LastData = 0;
  PID_Angle.Limit_Max = 10000;
  PID_Angle.Limit_Min = -10000;
  PID_Angle.Put_Max = 300;
  PID_Angle.Put_Min = -300;
*/
	PID_Init(&PID_Angle,0.25,0,0,10000,-10000,300,-300);
  
  for(;;)
  {
    //测试
    TargetAngle = (180 * sin(2 * 3.14f * HAL_GetTick() / 1000.0f ) + 180);

    if(TargetAngle < 0)TargetAngle += 360;
    MechanicalAngle = TargetAngle * 8191 / 360 ;
    //过零保护
    // 改进的过零保护
    int32_t error_mech = MechanicalAngle - GM_Statu[0].Mechanical_Angle;
    if(error_mech > 4096) {
            error_mech -= 8191;
    }
     else if(error_mech < -4096) {
              error_mech += 8191;
    }
    float corrected_target = GM_Statu[0].Mechanical_Angle + error_mech;
// 使用修正后的误差进行计算
    //if(TargetAngle == 0)corrected_target = 5;
    TargetSpeed = PID_Set(&PID_Angle,GM_Statu[0].Mechanical_Angle,corrected_target);
    obVoltage[0] += PID_Set(&PID_Speed,GM_Statu[0].Speed_rpm,TargetSpeed);
    GM6020_SetVoltageAll(obVoltage);
    osDelay(1);
  }
  /* USER CODE END CAN_DJ */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
