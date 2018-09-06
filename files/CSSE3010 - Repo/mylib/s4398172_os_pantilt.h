/**
 ******************************************************************************
 * @file    mylib/s4398172_os_pantilt.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Pantilt Control functionality and task.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_os_pantilt_init(void): init pantilt task, semaphores, queues
 * s4398172_os_pantilt_deinit(void):
 ******************************************************************************
 */

#ifndef s4398172_OS_PANTILT_H_
#define s4398172_OS_PANTILT_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t s4398172_HandlerPanTilt;

QueueHandle_t s4398172_QueuePan;
QueueHandle_t s4398172_QueueTilt;
QueueHandle_t s4398172_QueueSetMin;
QueueHandle_t s4398172_QueueSetMax;
QueueHandle_t s4398172_QueueCoord;

SemaphoreHandle_t s4398172_SemaphorePanLeft;
SemaphoreHandle_t s4398172_SemaphorePanRight;
SemaphoreHandle_t s4398172_SemaphoreTiltUp;
SemaphoreHandle_t s4398172_SemaphoreTiltDown;

/* External function prototypes -----------------------------------------------*/
extern void s4398172_os_pantilt_init(void);
extern void s4398172_os_pantilt_deinit(void);

#endif

