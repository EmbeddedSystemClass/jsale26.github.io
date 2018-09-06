/**
 ******************************************************************************
 * @file    mylib/s4398172_os_ir.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief	IR Control functionality and Task.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * void s4398172_os_ir_init(void);
 * void s4398172_os_ir_deinit(void);
 ******************************************************************************
 */

#ifndef s4398172_OS_IR_H_
#define s4398172_OS_IR_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIME 0
#define BIT 1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t s4398172_HandlerIR;
QueueHandle_t s4398172_QueueIR;

/* External function prototypes -----------------------------------------------*/
extern void s4398172_os_ir_init(void);
extern void s4398172_os_ir_deinit(void);

#endif

