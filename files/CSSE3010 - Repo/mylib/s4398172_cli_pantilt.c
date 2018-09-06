/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_pantilt.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Pantilt CLI commands.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_cli_pantilt.h"
#include "s4398172_os_printf.h"
#include "s4398172_hal_pantilt.h"
#include "s4398172_os_pantilt.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define mainPanTiltTASK_PRIORITY		(tskIDLE_PRIORITY + 2)

#define mainPanTiltTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

#define TYPE_PAN 1
#define TYPE_TILT 2
#define ANGLE_DIFF 5
#define MAX_ANGLE 70
#define PAYLOAD_LEN 128

/* Private variables ---------------------------------------------------------*/

/* Move the servo pan module by direction or angle. */
CLI_Command_Definition_t xPan = {
	"pan",
	"pan: <direction> or <angle>\n\r",
	prvPanTiltCommand,
	1
};

/* Move the servo tilt module by direction or angle. */
CLI_Command_Definition_t xTilt = {
	"tilt",
	"tilt: <direction> or <angle>\n\r",
	prvPanTiltCommand,
	1
};

/* Set the laser bottom left location. */
CLI_Command_Definition_t xSetMin = {
	"setmin",
	"setmin: set bottom left corner of the laser.\n\r",
	prvSetLaser,
	0
};

/* Set the laser top right location. */
CLI_Command_Definition_t xSetMax = {
	"setmax",
	"setmax: set top right corner of the laser.\n\r",
	prvSetLaser,
	0
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Convert string to integer
  * @param  *input is a string
  * @retval integer value of input
  */
int str_to_int(char *input) {

	char *badVal;
	int num = strtoul(input, &badVal, 10);

	if (badVal[0] != '\0') {
		return 420;
	}

	return num;
}

/**
 * @brief  	Set the laser bottom left and top right.
 * @param  	int type: pan or tilt.
 * @param 	char *parameter: CLI input parameter.
 * @retval 	None.
 */
void manipulate_cli_output(int type, char *parameter) {

	int angle = str_to_int(parameter);

	switch(type) {
		case TYPE_PAN:
			if (strcmp(parameter, "left") == 0) {

				xSemaphoreGive(s4398172_SemaphorePanLeft);
			} else if (strcmp(parameter, "right") == 0) {

				xSemaphoreGive(s4398172_SemaphorePanRight);
			} else if ((angle <= MAX_ANGLE) && (angle >= -MAX_ANGLE)) {

				xQueueSendToFront(s4398172_QueuePan, (void *) &angle,
									(portTickType) 10);
			}
			break;

		case TYPE_TILT:
			if (strcmp(parameter, "up") == 0) {

				xSemaphoreGive(s4398172_SemaphoreTiltUp);
			} else if (strcmp(parameter, "down") == 0) {

				xSemaphoreGive(s4398172_SemaphoreTiltDown);
			} else if ((angle <= MAX_ANGLE) && (angle >= -MAX_ANGLE)) {

				xQueueSendToFront(s4398172_QueueTilt, (void *) &angle,
									(portTickType) 10);
			}
			break;
	}
}

/**
 * @brief  	Set the laser bottom left and top right.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvSetLaser(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	strcpy(inputBuffer, pcCommandString);
	int angle[2];

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	angle[PAN] = s4398172_hal_pantilt_pan_read();
	angle[TILT] = s4398172_hal_pantilt_tilt_read();

	if (strcmp(inputBuffer, "setmin") == 0) {

		if (s4398172_QueueSetMin != NULL) {

			xQueueSendToBack(s4398172_QueueSetMin, (void *) &angle,
								(portTickType) 10);
		}
	} else if (strcmp(inputBuffer, "setmax") == 0) {

		if (s4398172_QueueSetMax != NULL) {

			xQueueSendToBack(s4398172_QueueSetMax, (void *) &angle,
								(portTickType) 10);
		}
	}
	return pdFALSE;
}


/**
 * @brief  	Move the Pan/Tilt module to given angle, or in specified direction.
 * @param  	char *pcWriteBuffer: CLI response
 * @param 	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvPanTiltCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	int type = 0;
	char inputBuffer[PAYLOAD_LEN], parameter[PAYLOAD_LEN];
	long lParam_len;
	const char *cCmd_string;
	char *token;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	strcpy(inputBuffer, pcCommandString);
	token = strtok(inputBuffer, " ");

	if (strcmp(token, "pan") == 0) {
		type = TYPE_PAN;
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');
	} else if (strcmp(token, "tilt") == 0) {
		type = TYPE_TILT;
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');
	} else {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer,
				"\n\r%s\n\r", cCmd_string);
		return pdFALSE;
	}

	strcpy(parameter, cCmd_string);
	manipulate_cli_output(type, parameter);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/**
  * @brief  Register Pantilt CLI commands.
  * @param  None
  * @retval None
  */
void s4398172_cli_pantilt_init(void) {

	FreeRTOS_CLIRegisterCommand(&xPan);
	FreeRTOS_CLIRegisterCommand(&xTilt);
	FreeRTOS_CLIRegisterCommand(&xSetMin);
	FreeRTOS_CLIRegisterCommand(&xSetMax);
}
