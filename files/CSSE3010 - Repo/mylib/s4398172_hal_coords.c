/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_sysmon.c
 * @author  Joshua Salecich � 43981722
 * @date    28/04/2018
 * @brief   Initialise Logic Analyser Pins and toggling functionality.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_hal_sysmon_init() � intialise Logic Analyser Pins
 * s4398172_hal_sysmon_write(int val, int pin) �
 * set the LA pin high (val = 1) or low (val = 0) on respective pin
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_coords.h"
#include "s4398172_hal_pantilt.h"
#include "s4398172_os_printf.h"
#include "s4398172_os_pantilt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private define ------------------------------------------------------------*/
#define PEN 9

/* Private functions ---------------------------------------------------------*/
char s4398172_depth[2] = "50";
char s4398172_payload[PACKET_MAX] = "XYZ00000000";

/**
  * @brief  Convert string to integer
  * @param  *input is a string
  * @retval integer value of input
  */
int s4398172_convert_string(char *input, int type) {

	char *badVal;
	int num = strtoul(input, &badVal, type);

	if (badVal[0] != '\0') {
		return FALSE;
	}

	return num;
}

/**
 * @brief  	Set the z axis coordinate on the CNC plotter based off user input.
 * @param  	char *param: either sets plotter up or down.
 * @retval 	None
 */
void s4398172_update_z(char *param) {

	if (strcmp(param, "up") == 0) {

		for (int i = 0; i < 2; i++) {
			s4398172_payload[PEN + i] = '0';
		}
	} else if (strcmp(param, "down") == 0) {

		for (int i = 0; i < 2; i++) {
			s4398172_payload[PEN + i] = s4398172_depth[i];
		}
	} else {
		myprintf("%s", "Pen direction must be up or down\n\r");
	}
}

/**
 * @brief  	Update X or Y by incrementing with given value.
 * @param  	int difference: value to increment by.
 * @param	int type: x or y coord.
 * @retval 	None
 */
void s4398172_update_xy_increment(int difference, int type) {

	char buffer[4] = {0, 0, 0, '\0'};
	int decimalValue;

	for (int i = 0; i < 3; i++) {

		buffer[i] = s4398172_payload[i + 3 * (type + 1)];
	}

	decimalValue = s4398172_convert_string(buffer, DEC);
	decimalValue += difference;

	sprintf(buffer, "%d", decimalValue);
	s4398172_update_xy(buffer, type);
}

/**
 * @brief  	Update X or Y to given value - also update laser location.
 * @param  	char *param: given XYZ packet.
 * @param	int type: x or y coord.
 * @retval 	None
 */
void s4398172_update_xy(char *param, int type) {

	int value[2];
	value[0] = s4398172_convert_string(param, DEC);

	if (type == X_COORD) {

		value[1] = PAN;

		if (s4398172_QueueCoord != NULL) {

			xQueueSendToBack(s4398172_QueueCoord, (void *) &value,
								(portTickType) 10);
		}

	} else {

		value[1] = TILT;

		if (s4398172_QueueCoord != NULL) {

			xQueueSendToBack(s4398172_QueueCoord, (void *) &value,
								(portTickType) 10);
		}
	}

	switch (strlen(param)) {

		case 3:

			for (int i = 3; i < 6; i++) {
				s4398172_payload[i + type * 3] = param[i - 3];
			}
			break;

		case 2:

			s4398172_payload[3 + 3 * type] = '0';
			for (int i = 4; i < 6; i++) {
				s4398172_payload[i + 3 * type] = param[i - 4];
			}
			break;

		case 1:

			s4398172_payload[3 + 3 * type] = '0';
			s4398172_payload[4 + 3 * type] = '0';
			s4398172_payload[5 + 3 * type] = param[0];
			break;
	}

}


