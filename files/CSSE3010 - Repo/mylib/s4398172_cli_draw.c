/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_draw.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Draw (Graphics) CLI commands.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_printf.h"
#include <s4398172_os_radio.h>
#include "s4398172_os_printf.h"
#include "s4398172_cli_draw.h"
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
#include <math.h>

/* Defines -------------------------------------------------------------------*/
#define LOW 0
#define HIGH 1
#define PI 3.14159265
#define ELLIPSE_RATIO 4

/* Private variables ---------------------------------------------------------*/

/* Draw a horizontal or vertical line, starting X, Y */
CLI_Command_Definition_t xLine = {
	"line",
	"line: <X> <Y> <type> <length>\n\r",
	prvLineCommand,
	4
};

/* Draw a square starting at X, Y with given sidelength. */
CLI_Command_Definition_t xSquare = {
	"square",
	"square: <X> <Y> <side length>\n\r",
	prvSquareCommand,
	3
};

/* Use Bresenham algorithm to draw line from X1, Y1 to X2, Y2. */
CLI_Command_Definition_t xBLine = {
	"bline",
	"bline: <X1> <Y1> <X2> <Y2> <step size>\n\r",
	prvBLineCommand,
	5
};

/* Draw a n sided polygon with given sidelength.*/
CLI_Command_Definition_t xPoly = {
	"poly",
	"poly: <X> <Y> <number of sides> <side length>\n\r",
	prvPolyCommand,
	4
};

/* Draw a n sided rose with given sidelength. */
CLI_Command_Definition_t xRose = {
	"rose",
	"rose: <number of sides> <side length>\n\r",
	prvRoseCommand,
	4
};


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  	Draw a horizontal or vertical line, given start point and length.
 * @param	int x0, y0: starting coordinates.
 * @param	int x1, y1: ending coordinates.
 * @param	int step: step size.
 * @param 	int slopeType: is the slope high or low.
 * @retval 	None.
 */
void bline(int x0, int y0, int x1, int y1, int step, int slopeType) {

	int dX = x1 - x0;
	int dY = y1 - y0;
	int increment, difference;
	int y, x;
	char buffer[3];
	s4398172_update_z("down");

	if (slopeType == LOW) {

		increment = 1;

		if (dY < 0) {
			increment = -1;
			dY = -1 * dY; //positive change
		}

		difference = 2 * dY - dX;
		y = y0;

		for (x = x0; x <= x1; x += step) {

			sprintf(buffer, "%d", x);
			s4398172_update_xy(buffer, X_COORD);

			sprintf(buffer, "%d", y);
			s4398172_update_xy(buffer, Y_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}

			if (difference > 0) {

				y = y + increment * step;
				difference += -2 * dX;
			}
			difference += 2 * dY;
		}

	} else if (slopeType == HIGH) {
		increment = 1;

		if (dX < 0) {
			increment = -1;
			dX = -1 * dX; //positive change
		}

		difference = 2 * dX - dY;
		x = x0;

		for (y = y0; y <= y1; y += step) {

			sprintf(buffer, "%d", x);
			s4398172_update_xy(buffer, X_COORD);

			sprintf(buffer, "%d", y);
			s4398172_update_xy(buffer, Y_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}

			if (difference > 0) {

				x = x + increment * step;
				difference += -2 * dY;
			}
			difference += 2 * dX;
		}
	}

	s4398172_update_z("up");

	if (s4398172_QueuePacket != NULL) {

		xQueueSendToBack(s4398172_QueuePacket, (void *) &s4398172_payload,
							(portTickType) 10);
	}
}

/**
 * @brief  	Decision matrix to determine what type of Bresenham line to draw.
 * @param	int x[2]: start and end x coord.
 * @param 	int y[2]: start and end y coord.
 * @param	int step: step size.
 * @retval 	None.
 */
void bresenham_approximation(int x[2], int y[2], int step) {

	int dX = x[1] - x[0];
	int dY = y[1] - y[0];

	if ((abs(dY) < abs(dX))) {

		if (x[0] > x[1]) {
			bline(x[1], y[1], x[0], y[0], step, LOW);
		} else {
			bline(x[0], y[0], x[1], y[1], step, LOW);
		}
	} else {

		if (y[0] > y[1]) {
			bline(x[1], y[1], x[0], y[0], step, HIGH);
		} else {
			bline(x[0], y[0], x[1], y[1], step, HIGH);
		}
	}
}

/**
 * @brief  	Draw a bresenham line with given starting and ending X, Y
 * 			with step size. Used to approximate lines which aren't vertical
 * 			and horizontal.
 * @param  	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvBLineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	char recv[PAYLOAD_LEN];
	long lParam_len;
	char *token;
	int step, yDec[2], xDec[2];

	/* Get parameters from command string */
	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	strcpy(recv, param);

	strcpy(inputBuffer, pcCommandString);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "bline") == 0) {

		/* Take pen to starting location */
		xDec[0] = s4398172_convert_string(strtok(NULL, " "), DEC);

		yDec[0] = s4398172_convert_string(strtok(NULL, " "), DEC);

		xDec[1] = s4398172_convert_string(strtok(NULL, " "), DEC);

		yDec[1] = s4398172_convert_string(strtok(NULL, " "), DEC);

		token = strtok((char *)pcCommandString, " ");
		for (int i = 0; i < 5; i++) {
			token = strtok(NULL, " ");
		}
		step = s4398172_convert_string(token, DEC);

		/* Error checking */
		for (int i = 0; i < 2; i++) {
			if ((yDec[i] < 0 || yDec[i] > 200) || (xDec[i] < 0 ||
					xDec[i] > 200)) {
				myprintf("%s", "Range is from 0 -> 200mm inclusive.\n\r");
				return pdFALSE;
			}
		}

		bresenham_approximation(xDec, yDec, step);

	}
	return pdFALSE;
}

/**
 * @brief  	Draw a rose with given sidelength, starting pos and number of sides.
 * @param  	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvRoseCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	char *token;
	int n, sideLength, xCentre, yCentre, radius, x[2], y[2];
	int petalWidth, xInit, yInit, spacingAngle;

	/* Get parameters from command string */
	strcpy(inputBuffer, pcCommandString);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp((token = strtok(inputBuffer, " ")), "rose") == 0) {

		token = strtok(NULL, " ");
		xInit = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		yInit = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		n = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		sideLength = s4398172_convert_string(token, DEC);

		petalWidth = sideLength / ELLIPSE_RATIO;
		radius = (int)(sideLength * cos(45 * PI / 180));
		spacingAngle = (int)(180 / n);

		for (int i = 180; i >= spacingAngle; i -= spacingAngle) {

			xCentre = xInit + (radius - petalWidth) * cos((i + 90) * PI / 180);
			yCentre = yInit + (radius - petalWidth) * sin((i + 90) * PI / 180);

			x[0] = (int)(xCentre + radius * cos((i - 45) * PI / 180));
			y[0] = (int)(yCentre + radius * sin((i - 45) * PI / 180));

			for (int j = (i - 45 - 9); j >= (i - 45 - 90); j -= 9) {

				x[1] = (int)(xCentre + radius * cos(j * PI / 180));
				y[1] = (int)(yCentre + radius * sin(j * PI / 180));

				bresenham_approximation(x, y, 1);

				x[0] = x[1];
				y[0] = y[1];
			}

			xCentre = xInit + (radius - petalWidth) * cos((i - 90) * PI / 180);
			yCentre = yInit + (radius - petalWidth) * sin((i - 90) * PI / 180);

			x[0] = (int)(xCentre + radius * cos((i + 45) * PI / 180));
			y[0] = (int)(yCentre + radius * sin((i + 45) * PI / 180));

			for (int j = (i + 45 + 9); j <= (i + 45 + 90); j += 9) {

				x[1] = (int)(xCentre + radius * cos(j * PI / 180));
				y[1] = (int)(yCentre + radius * sin(j * PI / 180));

				bresenham_approximation(x, y, 1);

				x[0] = x[1];
				y[0] = y[1];
			}
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Draw a polygon with given sidelength, starting pos and
 * 			number of sides.
 * @param  	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvPolyCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	char *token;
	int n, sideLength, xCentre, yCentre, radius, x[2], y[2];

	/* Get parameters from command string */
	strcpy(inputBuffer, pcCommandString);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp((token = strtok(inputBuffer, " ")), "poly") == 0) {

		token = strtok(NULL, " ");
		x[0] = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		y[0] = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		n = s4398172_convert_string(token, DEC);

		token = strtok(NULL, " ");
		sideLength = s4398172_convert_string(token, DEC);

		radius = sideLength / (2 * sin(PI / n));

		xCentre = x[0] - radius;
		yCentre = y[0];

		x[0] = (int)(xCentre + radius * cos(0));
		y[0] = (int)(yCentre + radius * sin(0));

		for (int i = 1; i <= n; i++) {

			x[1] = (int)(xCentre + radius * cos(i * 2 * PI / n));
			y[1] = (int)(yCentre + radius * sin(i * 2 * PI / n));

			bresenham_approximation(x, y, 1);

			x[0] = x[1];
			y[0] = y[1];
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Draw a square given starting X, Y and sidelength.
 * @param  	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvSquareCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	char recv[PAYLOAD_LEN];
	long lParam_len;
	char *token;
	int side, xDec, yDec;

	/* Get parameters from command string */
	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	strcpy(recv, param);

	strcpy(inputBuffer, pcCommandString);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "square") == 0) {

		token = strtok(recv, " ");
		xDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, X_COORD);

		token = strtok(NULL, " ");
		yDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, Y_COORD);

		token = strtok(NULL, " ");
		side = s4398172_convert_string(token, DEC);

		s4398172_update_z("down");

		/* Error checking */
		if ((yDec < 0 || yDec > 200) || (xDec < 0 || xDec > 200) ||
				((yDec + side) > 200) || ((xDec + side) > 200)) {

			myprintf("%s", "Range is from 0 -> 200mm inclusive.\n\r");
			return pdFALSE;
		}

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket,
					(void *) &s4398172_payload, (portTickType) 10);
		}

		sprintf(token, "%d", xDec + side);
		s4398172_update_xy(token, X_COORD);

		sprintf(token, "%d", yDec + side);
		s4398172_update_xy(token, Y_COORD);

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket,
					(void *) &s4398172_payload, (portTickType) 10);
		}

		sprintf(token, "%d", xDec);
		s4398172_update_xy(token, X_COORD);

		sprintf(token, "%d", yDec);
		s4398172_update_xy(token, Y_COORD);

		s4398172_update_z("up");

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket,
					(void *) &s4398172_payload, (portTickType) 10);
		}
	}

	return pdFALSE;
}

/**
 * @brief  	Draw a horizontal or vertical line, given start point and length.
 * @param  	char *pcWriteBuffer: CLI response
 * @param	size_t xWriteBufferLen: Size of CLI response
 * @param 	const char *pcCommandString: Command entered by user
 * @retval 	BaseType_t: Machine Type
 */
BaseType_t prvLineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString) {

	char inputBuffer[PAYLOAD_LEN];
	const char *param;
	char recv[PAYLOAD_LEN];
	long lParam_len;
	char *token;
	int length, yDec, xDec;
	int boundFlag = 0;

	/* Get parameters from command string */
	param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	strcpy(recv, param);
	param = FreeRTOS_CLIGetParameter(pcCommandString, 4, &lParam_len);

	strcpy(inputBuffer, pcCommandString);

	/* Null response. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%c", '\0');

	if (strcmp(strtok(inputBuffer, " "), "line") == 0) {

		/* Take pen to starting location */
		token = strtok(recv, " ");
		xDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, X_COORD);

		token = strtok(NULL, " ");
		yDec = s4398172_convert_string(token, DEC);
		s4398172_update_xy(token, Y_COORD);

		s4398172_update_z("down");

		token = strtok(NULL, " ");

		length = s4398172_convert_string((char *)param, DEC);

		/* Error checking */
		if ((yDec < 0 || yDec > 200) || (xDec < 0 || xDec > 200)) {
			boundFlag = 1;
		}

		if (strcmp(token, "h") == 0) {

			if ((xDec + length) > 200) {
				boundFlag = 1;
			}

		} else if (strcmp(token, "v") == 0) {

			if ((yDec + length) > 200) {
				boundFlag = 1;
			}

		} else {

			myprintf("%s", "Direction must be h or v\n\r");
			return pdFALSE;
		}

		if (boundFlag) {

			myprintf("%s", "Range is from 0 -> 200mm inclusive.\n\r");
			return pdFALSE;
		}

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket,
					(void *) &s4398172_payload, (portTickType) 10);
		}

		if (strcmp(token, "h") == 0) {

			xDec = xDec + length;
			sprintf(token, "%d", xDec);
			s4398172_update_xy(token, X_COORD);

		} else if (strcmp(token, "v") == 0) {

			yDec = yDec + length;
			sprintf(token, "%d", yDec);
			s4398172_update_xy(token, Y_COORD);

		}

		s4398172_update_z("up");

		if (s4398172_QueuePacket != NULL) {

			xQueueSendToBack(s4398172_QueuePacket,
					(void *) &s4398172_payload, (portTickType) 10);
		}
	}

	return pdFALSE;
}

/**
  * @brief  Initialise cli radio commands
  * @param  None
  * @retval None
  */
void s4398172_cli_draw_init(void) {

	FreeRTOS_CLIRegisterCommand(&xLine);
	FreeRTOS_CLIRegisterCommand(&xSquare);
	FreeRTOS_CLIRegisterCommand(&xBLine);
	FreeRTOS_CLIRegisterCommand(&xPoly);
	FreeRTOS_CLIRegisterCommand(&xRose);
}
