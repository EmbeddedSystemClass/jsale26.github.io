/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_pantilt.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Pantilt CLI commands.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * void s4398172_cli_pantilt_init(void); init task and semaphores.
 ******************************************************************************
 */

#ifndef s4398172_CLI_PANTILT_H_
#define s4398172_CLI_PANTILT_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
BaseType_t prvPanTiltCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvSetLaser(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);

void s4398172_cli_pantilt_init(void);

/* Private variables ---------------------------------------------------------*/
extern CLI_Command_Definition_t xPan;
extern CLI_Command_Definition_t xTilt;
extern CLI_Command_Definition_t xSetMin;
extern CLI_Command_Definition_t xSetMax;

#endif

