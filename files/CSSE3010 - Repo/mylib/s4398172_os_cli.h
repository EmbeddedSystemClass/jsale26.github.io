/**
 ******************************************************************************
 * @file    mylib/s4398172_os_cli.h
 * @author  Joshua Salecich � 43981722
 * @date    28/04/2018
 * @brief   Initialise CLI
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_os_cli_init(void) � CLI task init
 ******************************************************************************
 */

#ifndef s4398172_OS_CLI_H_
#define s4398172_OS_CLI_H_

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
TaskHandle_t s4398172_HandlerCLI;

/* External function prototypes ----------------------------------------------*/
void s4398172_os_cli_init(void);

#endif

