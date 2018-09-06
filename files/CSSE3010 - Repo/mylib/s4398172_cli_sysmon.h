/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_radio.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Radio CLI commands.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * void s4398172_cli_pantilt_init(void); init task and semaphores.
 ******************************************************************************
 */

#ifndef s4398172_CLI_RADIO_H_
#define s4398172_CLI_RADIO_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* External function prototypes -----------------------------------------------*/
BaseType_t prvGetSysCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);

void s4398172_cli_sysmon_init(void);

/* Private variables ---------------------------------------------------------*/

extern CLI_Command_Definition_t xGetSys;


#endif

