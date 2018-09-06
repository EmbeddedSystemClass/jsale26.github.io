/**
 ******************************************************************************
 * @file    mylib/s4398172_os_printf.h
 * @author  Joshua Salecich � 43981722
 * @date    5/05/2018
 * @brief   Make debug_printf safe.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * void myprintf(char *input, ...);
 * void s4398172_TaskPrintf(void);
 * void s4398172_os_myprintf_init(void);
 ******************************************************************************
 */

#ifndef s4398172_OS_PRINTF_H_
#define s4398172_OS_PRINTF_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define myprintf(input, ...)		my_printf(input, __VA_ARGS__)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t s4398172_HandlerPrintf;

QueueHandle_t s4398172_QueuePrintf;

SemaphoreHandle_t s4398172_PrintMutex;

/* External function prototypes -----------------------------------------------*/
void my_printf(char *input, ...);
void s4398172_TaskPrintf(void);
void s4398172_os_myprintf_init(void);
void s4398172_os_myprintf_deinit(void);

#endif

