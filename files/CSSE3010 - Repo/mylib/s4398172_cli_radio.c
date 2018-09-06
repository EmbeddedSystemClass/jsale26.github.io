/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_radio.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Radio CLI commands.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <s4398172_cli_radio.h>
#include <s4398172_os_radio.h>
#include "s4398172_os_printf.h"
#include "s4398172_hal_coords.h"
#include "s4398172_hal_radio.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define BUFFER_LEN 5

/* Private variables ---------------------------------------------------------*/

/* Set radio RF Channel */
CLI_Command_Definition_t xSetChan = {
	"setchan",
	"setchan: <radio channel>\n\r",
	prvSetChanCommand,
	1
};

/* Set radio tx address */
CLI_Command_Definition_t xSetTxAddr = {
	"settxaddr",
	"settxaddr: <tx addr>\n\r",
	prvSetAddrCommand,
	1
};

/* Set radio rx address */
CLI_Command_Definition_t xSetRxAddr = {
	"setrxaddr",
	"setrxaddr: <rx addr>\n\r",
	prvSetAddrCommand,
	1
};

/* Get radio RF Channel */
CLI_Command_Definition_t xGetChan = {
	"getchan",
	"getchan: return radio channel\n\r",
	prvGetCommand,
	0
};

/* Get radio tx address */
CLI_Command_Definition_t xGetTxAddr = {
	"gettxaddr",
	"gettxaddr: return tx addr\n\r",
	prvGetCommand,
	0
};

/* Get radio rx address */
CLI_Command_Definition_t xGetRxAddr = {
	"getrxaddr",
	"getrxaddr: return rx addr\n\r",
	prvGetCommand,
	0
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  	Set the radio tx and rx addresses.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvSetAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	char *token;
	long lParam_len;
	char addrBuffer[PAYLOAD_LEN];
	int hexAddr;

	strcpy(inputBuffer, pcCommandString);

	token = strtok(inputBuffer, " ");

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	strcpy(addrBuffer, param);

	hexAddr = s4398172_convert_string(addrBuffer, HEX);

	addrBuffer[BUFFER_LEN - 1] = 0x00;
	for (int i = 0; i < (BUFFER_LEN - 1); i++) {
		addrBuffer[i] = ((hexAddr >> (sizeof(int) * 2 * i)) & 0xFF);
	}

	if (strcmp(token, "settxaddr") == 0) {

		if (s4398172_QueueSetTxAddr != NULL) {

			xQueueSendToBack(s4398172_QueueSetTxAddr, (void *) &addrBuffer,
								(portTickType) 10);
		}

	} else if (strcmp(token, "setrxaddr") == 0) {

		if (s4398172_QueueSetRxAddr != NULL) {

			xQueueSendToBack(s4398172_QueueSetRxAddr, (void *) &addrBuffer,
								(portTickType) 10);
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Set the radio RF Channel.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvSetChanCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	long lParam_len;
	int channel;

	/* Get parameters from command string */
	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	strcpy(inputBuffer, pcCommandString);

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "setchan") == 0) {

		if (s4398172_QueueSetChan != NULL) {

			channel = (unsigned char)s4398172_convert_string(
					(char *)param, DEC);
			xQueueSendToBack(s4398172_QueueSetChan, (void *) &channel,
								(portTickType) 10);
		}

	}

	return pdFALSE;
}

/**
 * @brief  	Get radio RF Channel and tx/rx addresses.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvGetCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	char *token;

	strcpy(inputBuffer, pcCommandString);
	token = strtok(inputBuffer, " ");

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(token, "getchan") == 0) {

		if (s4398172_SemaphoreGetChan != NULL) {
			xSemaphoreGive(s4398172_SemaphoreGetChan);
		}

	} else if (strcmp(token, "gettxaddr") == 0) {

		if (s4398172_SemaphoreGetTxAddr != NULL) {
			xSemaphoreGive(s4398172_SemaphoreGetTxAddr);
		}

	} else if (strcmp(token, "getrxaddr") == 0) {

		if (s4398172_SemaphoreGetRxAddr != NULL) {
			xSemaphoreGive(s4398172_SemaphoreGetRxAddr);
		}
	}

	return pdFALSE;
}

/**
  * @brief  Initialise cli radio commands
  * @param  None
  * @retval None
  */
void s4398172_cli_radio_init(void) {

	FreeRTOS_CLIRegisterCommand(&xSetChan);
	FreeRTOS_CLIRegisterCommand(&xSetTxAddr);
	FreeRTOS_CLIRegisterCommand(&xSetRxAddr);

	FreeRTOS_CLIRegisterCommand(&xGetChan);
	FreeRTOS_CLIRegisterCommand(&xGetTxAddr);
	FreeRTOS_CLIRegisterCommand(&xGetRxAddr);
}
