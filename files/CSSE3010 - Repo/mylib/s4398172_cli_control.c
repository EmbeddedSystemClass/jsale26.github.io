/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_control.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   (Plotter) Control CLI commands.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_printf.h"
#include <s4398172_os_radio.h>
#include "s4398172_os_printf.h"
#include "s4398172_hal_coords.h"
#include "s4398172_cli_control.h"
#include "s4398172_hal_radio.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/

/* Move plotter to X, Y command. */
CLI_Command_Definition_t xMove = {
	"move",
	"move: <X> <Y>\n\r",
	prvMovePenCommand,
	2
};

/* Raise/Lower pen command */
CLI_Command_Definition_t xPen = {
	"pen",
	"pen: <up or down>\n\r",
	prvMovePenCommand,
	1
};

/* Raise/Lower pen via input */
CLI_Command_Definition_t xPenDepth = {
	"pendepth",
	"pendepth: <height (mm)>\n\r",
	prvPenDepthCommand,
	1
};

/* Send join packet to initialise plotter communication. */
CLI_Command_Definition_t xJoin = {
	"join",
	"join: join to the CNC plotter.\n\r",
	prvJoinCommand,
	0
};

/* Send the plotter to X0 Y0 Z0. */
CLI_Command_Definition_t xOrigin = {
	"origin",
	"origin: Move the plotter to the origin.\n\r",
	prvOriginCommand,
	0
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  	Set the z axis coordinate on the CNC plotter based off user input.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvPenDepthCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	const char *param;
	long lParam_len;

	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strlen((char *)param) == 1) {

		s4398172_depth[0] = '0';
		s4398172_depth[1] = param[0];
	} else if (strlen((char *)param) == 2) {

		for (int i = 0; i < 2; i++) {
			s4398172_depth[i] = param[i];
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Set the plotter x, y and z coordinate to zero.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvOriginCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];

	strcpy(inputBuffer, pcCommandString);

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "origin") == 0) {

		s4398172_update_xy("0", X_COORD);
		s4398172_update_xy("0", Y_COORD);
		s4398172_update_z("up");

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket, (void *) &s4398172_payload,
								(portTickType) 10);
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Send a "JOIN" radio packet to join to the plotter - create
 * 			a communication link with the plotter.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvJoinCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];

	strcpy(inputBuffer, pcCommandString);

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "join") == 0) {

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket, (void *) &("JOIN"),
								(portTickType) 10);
		}
	}

	return pdFALSE;
}

/**
 * @brief	Move the pen in the x, y and z directions based off user input.
 * @param	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvMovePenCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	char recvCoords[6];
	long lParam_len;
	char *token;
	int xDec, yDec;

	/* Get parameters from command string */
	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	strcpy(recvCoords, param);

	strcpy(inputBuffer, pcCommandString);

	/* NULL Response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "move") == 0) {

		token = strtok(recvCoords, " ");
		xDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, X_COORD);

		token = strtok(NULL, " ");
		yDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, Y_COORD);

		/* Error checking */
		if ((yDec < 0 || yDec > 200) || (xDec < 0 || xDec > 200)) {
			myprintf("%d %d\n\r", yDec, xDec);
			myprintf("%s", "Range is from 0 -> 200mm inclusive.\n\r");
			return pdFALSE;
		}

	} else if (strcmp(strtok(inputBuffer, " "), "pen") == 0) {

		s4398172_update_z((char *)param);
	}

	if (s4398172_QueuePacket != NULL) {

		xQueueSendToBack(s4398172_QueuePacket, (void *) &s4398172_payload,
							(portTickType) 10);
	}

	return pdFALSE;
}


/**
  * @brief	Initialise CLI Control commands
  * @param  None
  * @retval None
  */
void s4398172_cli_control_init(void) {

	FreeRTOS_CLIRegisterCommand(&xMove);
	FreeRTOS_CLIRegisterCommand(&xPen);
	FreeRTOS_CLIRegisterCommand(&xJoin);
	FreeRTOS_CLIRegisterCommand(&xOrigin);
	FreeRTOS_CLIRegisterCommand(&xPenDepth);
}
