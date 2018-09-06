/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_radio.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Radio CLI commands.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <s4398172_cli_sysmon.h>
#include "s4398172_os_sysmon.h"
#include "s4398172_os_printf.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private defines -----------------------------------------------------------*/
#define PAYLOAD_LEN 128

/* Private variables ---------------------------------------------------------*/

/* Get system time. */
CLI_Command_Definition_t xGetSys = {
	"getsys",
	"getsys: return current system time (ms)\n\r",
	prvGetSysCommand,
	0
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  	Get system time.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvGetSysCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	char *token;

	strcpy(inputBuffer, pcCommandString);
	token = strtok(inputBuffer, " ");

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(token, "getsys") == 0) {

		if (s4398172_SemaphoreGetSys != NULL) {
			xSemaphoreGive(s4398172_SemaphoreGetSys);
		}
	}

	return pdFALSE;
}

/**
  * @brief  Initialise cli radio commands
  * @param  None
  * @retval None
  */
void s4398172_cli_sysmon_init(void) {

	FreeRTOS_CLIRegisterCommand(&xGetSys);
}
