/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_proj1_helper.c
 * @author  Joshua Salecich � 43981722
 * @date    23/03/2018
 * @brief  Helper functions for project 1
 *
 *******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *  void s4398172_encoder(char *command, int type)
 * 	void s4398172_decoder(char *command, int type)
 * 	void s4398172_move_servo(int type, int dir, int angle)
 * 	int s4398172_get_command(int *index, char *command)
 * 	void s4398172_amble_gen(int packetNum, uint16_t *data, uint32_t *packet)
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_hal_hamming.h"
#include "s4398172_hal_ircomms.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_proj1_helper.h"
#include "s4398172_hal_ir.h"
#include "s4398172_hal_joystick.h"
#include "s4398172_hal_ledbar.h"
#include "s4398172_hal_pantilt.h"

/* Private define ------------------------------------------------------------*/
#define ANGLE_DIFF 5 	/* Angle change in specification */
#define SERVO_CAP_ANGLE 75 		/* Maximum angle */
#define IR_PACKET_SIZE 22		/* IR TX/RX packet size */

/* Private functions ---------------------------------------------------------*/

/*
 * @brief Generates pre and postamble and appends it to the given data.
 * @param  int packetNum: current packet
 * uint16_t *data: data without amble
 * uint32_t *packet: complete packet with amble
 * @retval None
 */
void s4398172_amble_gen(int packetNum, uint16_t *data, uint32_t *packet) {
	for (int j = (IR_PACKET_SIZE - 1); j >= 0; j--) {
		switch(j) { //set amble
			case 0:
				packet[packetNum] &= ~(1 << j);
				break;
			case 1:
				packet[packetNum] |= 1 << j;
				break;
			case 18:
				packet[packetNum] |= 1 << j;
				break;
			case 19:
				packet[packetNum] &= ~(1 << j);
				break;
			case 20:
				packet[packetNum] |= 1 << j;
				break;
			case 21:
				packet[packetNum] &= ~(1 << j);
				break;
		}
		if ((j > 1) && (j < 18)) { //fill with data
			if ((data[packetNum] >> (j - 2)) & 1) {
				packet[packetNum] |= 1 << j;
			} else {
				packet[packetNum] &= ~(1 << j);
			}
		}
	}
}

/*
 * @brief Looks at a character and relates it to its hex value
 * @param  char val: character which needs conversion
 * @retval int hexValue: the character converted to hex
 */
int char_to_hex(char val) {
	int hexValue = 0;

	if ((val >= '0') && (val <= '9')) {
		hexValue = val - '0';
	}
	if ((val >= 'A') && (val <= 'F')) {
		hexValue = val - 55;
	}
	if ((val >= 'a') && (val <= 'f')) {
		hexValue = val - 87;
	}

	return hexValue;
}

/*
 * @brief Takes user input and appends to an array.
 * @param  int *index: index of command
 * char *command: buffer that needs to be filled
 * @retval None
 */
int s4398172_get_command(int *index, char *command) {
	char buffer = 0;
	int commandFull = 0;

	buffer = debug_getc();
	if (buffer != '\0') {
		if (buffer != '\r') {;
			debug_printf("%c", buffer);
			command[index[0]] = buffer;
			index[0]++;
		} else {
			debug_printf("\n\r");
			command[index[0]] = '\0';
			commandFull = 1;
			index[0] = 0;
		}
	}
	return commandFull;
}

/*
 * @brief Moves the servo under specified settings.
 * @param  int type: Pan or tilt
 * int dir: positive or negative
 * int angle: current angle
 * @retval None
 */
void s4398172_move_servo(int type, int dir, int angle) {
	if (type == PAN && dir == POS) {
		if ((angle + ANGLE_DIFF) < SERVO_CAP_ANGLE) {
			s4398172_hal_pantilt_pan_write(angle + ANGLE_DIFF);
		} else {
			s4398172_hal_pantilt_pan_write(SERVO_CAP_ANGLE);
		}
	} else if (type == PAN && dir == NEG) {
		if ((angle - ANGLE_DIFF) > -1 * SERVO_CAP_ANGLE) {
			s4398172_hal_pantilt_pan_write(angle - ANGLE_DIFF);
		} else {
			s4398172_hal_pantilt_pan_write(-1 * SERVO_CAP_ANGLE);
		}
	} else if (type == TILT && dir == POS) {
		if ((angle + ANGLE_DIFF) < SERVO_CAP_ANGLE) {
			s4398172_hal_pantilt_tilt_write(angle + ANGLE_DIFF);
		} else {
			s4398172_hal_pantilt_tilt_write(SERVO_CAP_ANGLE);
		}
	} else if (type == TILT && dir == NEG) {
		if ((angle - ANGLE_DIFF) > -1 * SERVO_CAP_ANGLE) {
			s4398172_hal_pantilt_tilt_write(angle - ANGLE_DIFF);
		} else {
			s4398172_hal_pantilt_tilt_write(-1 * SERVO_CAP_ANGLE);
		}
	}
}

/*
 * @brief Decoder demonstration function, works with manchester and hamming
 * @param char *command: given commandline
 * int type: manchester or hamming
 * @retval None
 */
void s4398172_decoder(char *command, int type) {
	uint16_t encodedData = 0;
	uint8_t decodedData = 0;

	command[2] = char_to_hex(command[2]);
	command[3] = char_to_hex(command[3]);
	command[4] = char_to_hex(command[4]);
	command[5] = char_to_hex(command[5]);

	for (int i = 0; i < 4; i++) {
		if ((command[5] >> i) & 1) {
			encodedData |= 1 << i;
		} else {
			encodedData &= ~(1 << i);
		}
	}
	for (int i = 4; i < 8; i++) {
		if ((command[4] >> (i - 4)) & 1) {
			encodedData |= 1 << i;
		} else {
			encodedData &= ~(1 << i);
		}
	}
	for (int i = 8; i < 12; i++) {
		if ((command[3] >> (i - 8)) & 1) {
			encodedData |= 1 << i;
		} else {
			encodedData &= ~(1 << i);
		}
	}
	for (int i = 12; i < 16; i++) {
		if ((command[2] >> (i - 12)) & 1) {
			encodedData |= 1 << i;
		} else {
			encodedData &= ~(1 << i);
		}
	}

	if (type == MANCHESTER) {
		decodedData = s4398172_man_decode(encodedData);
		debug_printf("%X\n\r", decodedData);
	} else if (type == HAMMING) {
		decodedData = s4398172_ham_decode(encodedData, 1);
	}
}

/*
 * @brief Encoder demonstration function, works with manchester and hamming
 * @param char *command: given commandline
 * int type: manchester or hamming
 * @retval None
 */
void s4398172_encoder(char *command, int type) {
	uint16_t encodedData = 0;
	uint8_t decodedData = 0;

	command[2] = char_to_hex(command[2]);
	command[3] = char_to_hex(command[3]);

	for (int i = 4; i < 8; i++) {
		if ((command[2] >> (i - 4)) & 1) {
			decodedData |= 1 << i;
		} else {
			decodedData &= ~(1 << i);
		}
	}
	for (int i = 0; i < 4; i++) {
		if ((command[3] >> i) & 1) {
			decodedData |= 1 << i;
		} else {
			decodedData &= ~(1 << i);
		}
	}

	if (type == MANCHESTER) {
		encodedData = s4398172_man_encode(decodedData);
	} else if (type == HAMMING) {
		encodedData = s4398172_ham_encode(decodedData);
	}
	debug_printf("%X\n\r", encodedData);
}
