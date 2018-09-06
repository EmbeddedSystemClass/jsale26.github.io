/**
 ******************************************************************************
 * @file    mylib/s4398172_os_sysmon.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   System Monitor functionality and task.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_os_sysmon_init(void): init radio tasks, semaphores, queues
 * s4398172_os_sysmon_deinit(void):
 ******************************************************************************
 */

#ifndef s4398172_OS_SYSMON_H_
#define s4398172_OS_SYSMON_H_

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
TaskHandle_t s4398172_HandlerSysmon;

SemaphoreHandle_t s4398172_SemaphoreGetSys;

/* External function prototypes -----------------------------------------------*/
extern void s4398172_os_sysmon_init(void);
extern void s4398172_os_sysmon_deinit(void);


#endif

