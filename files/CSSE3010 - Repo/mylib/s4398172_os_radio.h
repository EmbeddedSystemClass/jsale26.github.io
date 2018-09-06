/**
 ******************************************************************************
 * @file    mylib/s4398172_os_radio.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Radio control functionality and tasks.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_os_radio_init(void): init radio tasks, semaphores, queues
 * s4398172_os_radio_deinit(void):
 ******************************************************************************
 */

#ifndef s4398172_OS_RADIO_H_
#define s4398172_OS_RADIO_H_

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
TaskHandle_t s4398172_HandlerRadio;
TaskHandle_t s4398172_HandlerRadioFSM;

QueueHandle_t s4398172_QueueSetChan;
QueueHandle_t s4398172_QueueSetTxAddr;
QueueHandle_t s4398172_QueueSetRxAddr;
QueueHandle_t s4398172_QueuePacket;

SemaphoreHandle_t s4398172_SemaphoreGetChan;
SemaphoreHandle_t s4398172_SemaphoreGetTxAddr;
SemaphoreHandle_t s4398172_SemaphoreGetRxAddr;
SemaphoreHandle_t s4398172_SemaphoreJoin;

/* External function prototypes -----------------------------------------------*/
extern void s4398172_os_radio_init(void);
extern void s4398172_os_radio_deinit(void);


#endif

