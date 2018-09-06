/**
 ******************************************************************************
 * @file    Repo/project/Project1/main.c
 * @author  Joshua Salecich - s4398172
 * @date   	20/04/2018
 * @brief   Solution to the tasks set in Project 1 up to DT4.1
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"
#include "stdio.h"
#include "string.h"
#include "s4398172_hal_ir.h"
#include "s4398172_hal_joystick.h"
#include "s4398172_hal_ledbar.h"
#include "s4398172_hal_pantilt.h"
#include "s4398172_proj1_helper.h"
#include "s4398172_hal_radio.h"
#include "s4398172_hal_hamming.h"
#include "s4398172_hal_ircomms.h"

/* Private define ------------------------------------------------------------*/

#define IR_PACKET_SIZE 22	/* IR default packet size */
#define RA_PACKET_SIZE 32	/* Radio default packet size */
#define POS 1			/* Move the servo in the positive direction */
#define NEG 0			/* Move the servo in the negative direction */
#define PAN 1			/* Pantilt pan mode */
#define TILT 0			/* Pantilt tilt mode */
#define NUM_STATES 7	/* Total number of attempted modes */
#define FIRST_LED 9 	/* First LED, initialised back to front */
#define HAMMING 0 		/* Hamming encoding mode */
#define MANCHESTER 1 	/* Manchester encoding mode */
#define STX_STATE 0 	/* IR state machine: STX mode */
#define STX_VAL 0x02 	/* Value of STX */
#define ETX_STATE 1 	/* IR state machine: ETX mode */
#define ETX_VAL 0x03 	/* Value of ETX */
#define ACK_STATE 2 	/* IR state machine: ACK mode */
#define ACK_VAL 0x06 	/* Value of ACK */
#define NACK_STATE 3 	/* IR state machine: NACK mode */
#define NACK_VAL 0x15 	/* Value of NAK */
#define PACKET 4 		/* IR state machine: packet mode */
#define DEFAULT 5 		/* IR state machine: default mode */
#define COM_LEN 13 		/* Maximum length of effective input */
#define BIT_DELAY 5 	/* The time between highs and lows in IR transmitting */
/* WAIT_FACTOR * 5ms = how long to wait between each sent IR packet */
#define WAIT_FACTOR 8

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef TIM_3_Init; /* Handler for TIMER3 */
TIM_HandleTypeDef TIM_1_Init; /* Handler for TIMER1 */

int ledbar; 			/* The ledbar display in bits */
int oldLoop; 			/* The previous main iteration time */
int sendIndicator;		/* The time a message was sent */
int recvIndicator;		/* The time a message was received */
int ackIndicator;		/* If message sent, true until ACK received */
int errIndicator;		/* True when ERR received, false upon next correct TX */
int consoleOn = 1;		/* Taking console input */
int interruptTime; 		/* Current program runtime (ms) */
int prevTime;			/* The previous edge in IR receive */
int pulseTime;			/* The difference between prevTime and currentTime */
char modeFlag;			/* If in a mode and want to change modes set flag */
char currentMode;		/* Hold the current mode */
char oldMode;			/* Last mode accessed */
char codeCommand[6];	/* Array to store command for mode: encode/decode */
char irCommand[COM_LEN]; 			/* Array to store command for mode: IR */
char irRxBuffer[COM_LEN - 2]; 		/* IR received buffer */
int codeIndex[1];		/* Tracks the code command index */
int irIndex[1]; 		/* Tracks the IR command index */
int irTxPacket = 0;		/* Current IR TX packet */
int irTxIndex = 0;		/* Tracks IR TX packet index */
int numChars;			/* Number of characters in any command */
int irRecvFlag = 0;		/* If true waiting to receive a response */
int irState = DEFAULT;	/* Current state in IR state machine*/
int arbIndex = 0;		/* Arbitrary index used for upward counting */
int irRecvPrint;		/* Flag to print the received packet */
int irRxPacket = 0;		/* Current packet received */
int irRxIndex = 21;		/* MSB -> LSB IR receive index */
int irWaitState = 0;	/* Set if waiting between IR TX packets */
int irPrintAck = 0;		/* Flag to print IR ACK */
int irPacketSent;		/* Time the last IR packet was sent */
int irPrintNACK = 0; 	/* Flag to print ACK */
uint8_t deIrRxData[COM_LEN - 2]; 	/* Decoded IR RX data */
uint16_t enIrTxData[COM_LEN - 2];	/* Encoded IR TX data */
uint16_t enIrRxData[COM_LEN - 2];	/* Encoded IR RX data */
uint16_t strIrData[4];	/* Store ETX, STX, ACK, NACK data */
/* Encoded IR TX packets with start/stop bits*/
uint32_t enIrTxPacks[COM_LEN - 2];
uint32_t strIrPacks[4]; /* ETX, STX, ACK, NACK with start/stop bits */
uint32_t enIrRxPacks[COM_LEN]; /* Encoded IR RX packets */
unsigned char raRxPacket[RA_PACKET_SIZE];	/* Radio RX packet */
int raIndex[1]; 		/* Tracks the radio command index */
int raPacketSent;		/* Time last radio packet was sent */
int raAckErrFlag = -1;  /* Radio: 0 if ACK detected, 1 if ERR, -1 if packet */
int raRecvFlag = 0;		/* If true waiting to receive response */
char raCommand[COM_LEN]; 		/* Array to store command for mode: radio */
char raPayload[COM_LEN - 2];	/* Radio set payload by user */
char radioErr[5];		/* Packet filled with ERR */
char radioAck[5];		/* Packet filled with ACK */
int dIndex[1];			/* Tracks the full duplex index */
char dCommand[COM_LEN];			/* Array to store command for mode: duplex */

/* Private functions ---------------------------------------------------------*/

/*
 * @brief Simply takes the current mode and displays its binary value
 * (-1) on the three designated state LEDs.
 * @param  None
 * @retval None
 */
void ledbar_state(void) {

	int stateVal = (int)currentMode - (int)'0' - 1;
	int bitVal[3];
	int stateLedOne = FIRST_LED - 1;

	if (stateVal >= 0 && stateVal <= NUM_STATES) {

		for (int i = 2; i >= 0; i--) {

			/* Put bits into an array. Switch statement so LSB last */
			switch(i) {

				case 2:
					bitVal[0] = (stateVal >> i) & 1;
					break;
				case 1:
					bitVal[1] = (stateVal >> i) & 1;
					break;
				case 0:
					bitVal[2] = (stateVal >> i) & 1;
					break;
			}
		}
		for (int i = 0; i < 3; i++) {

			if (bitVal[i]) {
				ledbar |= 1 << (stateLedOne - i);
			} else {
				ledbar &= ~(1 << (stateLedOne - i));
			}
		}
	} else {

		for (int i = 2; i >= 0; i--) {
			ledbar &= ~(1 << (stateLedOne - i));
		}
	}
}

/*
 * @brief Function which essentially handles the LED display.
 * Takes flags set from other functions which indicate whether
 * a certain LED should be on or off.
 * @param  None
 * @retval None
 */
void ledbar_handler(void) {

	int newLoop;
	int sendLED = (FIRST_LED - 4), recvLED = (FIRST_LED - 5);
	int ackLED = (FIRST_LED - 6), errLED = (FIRST_LED - 7);

	/* Heartbeat LED */
	newLoop = HAL_GetTick();
	if ((newLoop - oldLoop) >= 200) {
		oldLoop = newLoop;
		/* Toggle the heartbeat */
		ledbar ^= 1 << FIRST_LED;
	}

	if (sendIndicator) {
		/* Turn send indicator on (5th LED) */
		ledbar |= 1 << sendLED;
	}
	if (HAL_GetTick() >= (sendIndicator + 250)) {
		ledbar &= ~(1 << sendLED);
		sendIndicator = 0;
	}

	if (recvIndicator) {
		/* Turn recv indicator on (6th LED) */
		ledbar |= 1 << recvLED;
	}
	if (HAL_GetTick() >= (recvIndicator + 250)) {
		ledbar &= ~(1 << recvLED);
		recvIndicator = 0;
	}

	ledbar ^= (-ackIndicator ^ ledbar) & (1 << ackLED);
	ledbar ^= (-errIndicator ^ ledbar) & (1 << errLED);

	ledbar_state();

	s4398172_ledbar_write(ledbar);

}

/*
 * @brief Terminal input Servo control mode. Takes input from the user.
 * If entry is one of the expected simply respond by moving the servo
 * accordingly. Also print servo angles every second.
 * @param  None
 * @retval None
 */
void terminal_mode(void) {

	char dir;
	int panAngle, tiltAngle;

	if (oldMode != currentMode) {

		consoleOn = 0;
		debug_printf("Welcome to PAN/TILT terminal control mode!\n\r");
		s4398172_move_servo(TILT, NEG, 5);
		s4398172_move_servo(PAN, NEG, 5);
	}

	panAngle = s4398172_hal_pantilt_pan_read();
	tiltAngle = s4398172_hal_pantilt_tilt_read();
	dir = debug_getc();

	if (dir != '\0') {

		switch(dir) {

			case 'A':
				s4398172_move_servo(PAN, NEG, panAngle);
				break;

			case 'D':
				s4398172_move_servo(PAN, POS, panAngle);
				break;

			case 'W':
				s4398172_move_servo(TILT, POS, tiltAngle);
				break;

			case 'S':
				s4398172_move_servo(TILT, NEG, tiltAngle);
				break;

			default:
				if ((((dir <= '7') &&
				(dir >= '1')) || (dir == '?'))) {

					modeFlag = dir;
					consoleOn = 1;
				} else {
					debug_printf("Incorrect entry... Try again.\n\r");
				}
				break;
		}
	}

	if (interruptTime % 1000 == 0) {
		debug_printf("Pan: %d Tilt: %d\n\r", panAngle, tiltAngle);
	}

}


/*
 * @brief Joystick input Servo control mode. Takes input from the user
 * via the joystick. The corresponding adc value effects the angle
 * of the servo based on direction. Also print servo angles every second.
 * @param  None
 * @retval None
 */
void joystick_mode(void) {

	int xPos, yPos, tiltAngle, panAngle;
	char joyCheck;

	if (oldMode != currentMode) {

		consoleOn = 0;
		debug_printf("Welcome to PAN/TILT joystick control mode!\n\r");
		s4398172_move_servo(TILT, NEG, 5);
		s4398172_move_servo(PAN, NEG, 5);
	}

	joyCheck = debug_getc();

	xPos = s4398172_hal_joystick_x_read();
	yPos = s4398172_hal_joystick_y_read();
	panAngle = s4398172_hal_pantilt_pan_read();
	tiltAngle = s4398172_hal_pantilt_tilt_read();

	if (interruptTime % 50 == 0) {

		if (xPos < 1000) {
			s4398172_move_servo(PAN, NEG, panAngle);
		} else if (xPos > 3000){
			s4398172_move_servo(PAN, POS, panAngle);
		}
		if (yPos < 1000) {
			s4398172_move_servo(TILT, NEG, tiltAngle);
		} else if (yPos > 3000) {
			s4398172_move_servo(TILT, POS, tiltAngle);
		}
	}


	if (interruptTime % 1000 == 0) {
		debug_printf("Pan: %d Tilt: %d\n\r", panAngle, tiltAngle);
	}

	if (joyCheck != '\0') {

		modeFlag = joyCheck;
		debug_printf("%c\n\r", modeFlag);
		consoleOn = 1;
	}
}

/*
 * @brief Joystick input Servo control mode. Takes input from the user
 * via the joystick. The corresponding adc value effects the angle
 * of the servo based on direction. Also print servo angles every second.
 * @param  None
 * @retval None
 */
void coding_mode(void) {

	if (oldMode != currentMode) {

		consoleOn = 0;
		debug_printf("Welcome to Coding mode!\n\r");
	}

	if (s4398172_get_command(codeIndex, codeCommand)) {

		switch(codeCommand[0]) {

			case 'M':

				switch(codeCommand[1]) {

					case 'E':

						if (strlen(codeCommand) > 4) {
							debug_printf("Incorrect entry... Try again.\n\r");
						} else {
							s4398172_encoder(codeCommand, MANCHESTER);
						}
						break;

					case 'D':

						if (strlen(codeCommand) > 6) {
							debug_printf("Incorrect entry... Try again.\n\r");
						} else {
							s4398172_decoder(codeCommand, MANCHESTER);
						}
						break;

					default:

						debug_printf("Incorrect entry... Try again.\n\r");
						break;
				}
				break;

			case 'H':
				switch(codeCommand[1]) {
					case 'E':

						if (strlen(codeCommand) > 4) {
							debug_printf("Incorrect entry... Try again.\n\r");
						} else {
							s4398172_encoder(codeCommand, HAMMING);
						}
						break;

					case 'D':

						if (strlen(codeCommand) > 6) {
							debug_printf("Incorrect entry... Try again.\n\r");
						} else {
							s4398172_decoder(codeCommand, HAMMING);
						}
						break;

					default:
						debug_printf("Incorrect entry... Try again.\n\r");
						break;
				}
				break;

			default:

				if ((codeCommand[1] == '\0') && (((codeCommand[0] <= '7') &&
				(codeCommand[0] >= '1')) || (codeCommand[0] == '?'))) {

					modeFlag = codeCommand[0];
					consoleOn = 1;
				} else {
					debug_printf("Incorrect entry... Try again.\n\r");
				}
				break;
		}
	}
}

/*
 * @brief Fills the arbitrary packets required for most duplex
 * operations. These are later used for sending these values and
 * detecting them on receiving.
 * @param  None
 * @retval None
 */
void fill_arb_packets(void) {

	strIrData[STX_STATE] = s4398172_man_encode(STX_VAL);
	strIrData[ETX_STATE] = s4398172_man_encode(ETX_VAL);
	strIrData[ACK_STATE] = s4398172_man_encode(ACK_VAL);
	strIrData[NACK_STATE] = s4398172_man_encode(NACK_VAL);

	s4398172_amble_gen(STX_STATE, strIrData, strIrPacks);
	s4398172_amble_gen(ETX_STATE, strIrData, strIrPacks);
	s4398172_amble_gen(ACK_STATE, strIrData, strIrPacks);
	s4398172_amble_gen(NACK_STATE, strIrData, strIrPacks);

	radioErr[0] = 'E', radioErr[1] = ' ', radioErr[2] = 'R';
	radioErr[3] = ' ', radioErr[4] = 'R';
	radioAck[0] = 'A', radioAck[1] = ' ', radioAck[2] = 'C';
	radioAck[3] = ' ', radioAck[4] = 'K';

}

/*
 * @brief IR mode which takes an input and then encodes it into
 * Manchester form. This packet is later sent via interrupt.
 * @param  None
 * @retval None
 */
void ir_mode(void) {

	if (oldMode != currentMode) {

		consoleOn = 0;
		debug_printf("Welcome to IR Duplex mode!\n\r");
	}

	if (s4398172_get_command(irIndex, irCommand)) {

		if (irCommand[0] == 'I') {

			if (irCommand[1] == 'T') {

				irTxPacket = 0;
				numChars = strlen(irCommand);

				if (numChars > COM_LEN) {
					/* If too many chars are entered, cap at 11 */
					numChars = COM_LEN;
				}

				sendIndicator = HAL_GetTick();
				debug_printf("Sent from IR: ");
				for (int i = 2; i < numChars; i++) {
					debug_printf("%c", irCommand[i]);
				}
				debug_printf("\n\r");

				irPacketSent = HAL_GetTick();
				irRecvFlag = 1;
				irState = STX_STATE;
				ackIndicator = 1;

				for (int i = 2; i < numChars; i++) {

					enIrTxData[i - 2] = s4398172_man_encode(irCommand[i]);
					s4398172_amble_gen((i - 2), enIrTxData, enIrTxPacks);
				}

			} else {
				debug_printf("Incorrect entry... Try again.\n\r");
			}
		} else if ((irCommand[1] == '\0') && (((irCommand[0] <= '7') &&
				(irCommand[0] >= '1')) || (irCommand[0] == '?'))) {

			modeFlag = irCommand[0];
			consoleOn = 1;
		} else {
			debug_printf("Incorrect entry... Try again.\n\r");
		}
	}
}


/*
 * @brief Function to update the payload within radio.c
 * Also print the payload.
 * @param  char *payload: desired payload set via user input
 * in: radio_mode
 * @retval None
 */
void radio_send_packet(char *payload) {

	debug_printf("Sent from Radio: ");
	for (int i = 0; i < strlen(payload); i++) {
		debug_printf("%c", payload[i]);
	}
	debug_printf("\n\r");
	s4398172_set_payload(payload);
	sendIndicator = HAL_GetTick();
}

/*
 * @brief Radio mode which takes an input and then encodes it into
 * Hamming form. This packet is later sent by using the
 * radio_send_packet function - the radio fsm activated via interrupt.
 * @param  None
 * @retval None
 */
void radio_mode(void) {

	if (oldMode != currentMode) {

		consoleOn = 0;
		if (currentMode == '6') {
			debug_printf("Welcome to Radio Duplex mode!\n\r");
		} else if (currentMode == '7') {
			debug_printf("Welcome to Full Duplex mode!\n\r");
		}
	}

	if (s4398172_get_command(raIndex, raCommand)) {

		if (((raCommand[0] == 'R') && (currentMode == '6')) ||
				((raCommand[0] == 'D') && (currentMode == '7'))) {

			if (raCommand[1] == 'T') {

				numChars = strlen(raCommand);
				if (numChars > COM_LEN) {
					numChars = COM_LEN;
				}

				for (int i = 0; i < 11; i++) {
					/* Set payload to zero */
					raPayload[i] = 0;
				}

				for (int i = 2; i < numChars; i++) {
					raPayload[i - 2] = raCommand[i];
				}

				ackIndicator = 1;
				radio_send_packet(raPayload);
				raRecvFlag = 1;
				raPacketSent = HAL_GetTick();

			}
		} else if ((raCommand[1] == '\0') && (((raCommand[0] <= '7') &&
				(raCommand[0] >= '1')) || (raCommand[0] == '?'))) {

			consoleOn = 1;
			modeFlag = raCommand[0];
		} else {
			debug_printf("Incorrect entry... Try again.\n\r");
		}
	}
}

/*
 * @brief Function which takes input and decides which
 * mode to send the user to.
 * @param  None
 * @retval None
 */
void get_console(void) {

	char consoleMode;

	if (consoleOn) {

		if (modeFlag == '\0') {
			consoleMode = debug_getc();
		} else {

			consoleMode = modeFlag;
			modeFlag = '\0';
		}

		if (consoleMode != '\0') {
			currentMode = consoleMode;
		}
	}

	switch(currentMode) {
		case '?':

			debug_printf("CSSE3010 Project 1\n\r"
					"1 Idle\n\r"
					"2 P/T Terminal\n\r"
					"3 P/T Joystick\n\r"
					"4 Encode/Decode\n\r"
					"5 IR Duplex\n\r"
					"6 Radio Duplex\n\r"
					"7 Integration\n\r");
			currentMode = oldMode;
			oldMode = '?';
			break;

		case '1':

			if (oldMode != currentMode) {
				debug_printf("Welcome to IDLE mode!\n\r");
			}
			break;

		case '2':
			terminal_mode();
			break;

		case '3':
			joystick_mode();
			break;

		case '4':
			coding_mode();
			break;

		case '5':
			ir_mode();
			break;

		case '6':
			radio_mode();
			break;

		case '7':
			radio_mode();
			break;

	}
	oldMode = currentMode;
}

/*
 * @brief Simply initialise all the peripheral drivers
 * and also set the stdout buffer to NULL.
 * @param  None
 * @retval None
 */
void peripheral_init(void) {

	s4398172_hal_pantilt_init();
	s4398172_joystick_init();
	s4398172_ledbar_init();
	s4398172_ir_init();
	s4398172_hal_radio_init();
	setbuf(stdout, NULL);
}

/*
 * @brief Manipulates the received radio packet. Determines
 * whether a one or two bit error has occured an deals with
 * it appropriately - relative to the current mode.
 * @param  None
 * @retval None
 */
void radio_packet_manipulate(void) {
	uint16_t enBuffer;
	uint8_t deBuffer[COM_LEN - 2];
	char recvType[5];
	raAckErrFlag = -1;

	for (int i = 10; i <= RA_PACKET_SIZE; i += 2) {

		enBuffer = (raRxPacket[i] << 8) | (raRxPacket[i + 1] & 0xFF);
		deBuffer[(i - 10) / 2] = s4398172_ham_decode(enBuffer, 0);

		if (deBuffer[(i - 10) / 2] == 255) {

			debug_printf("Received from Radio: 2-bit ERROR\n\r");
			errIndicator = 1;
			if (currentMode == '7') {

				/* Duplex mode: send ir NACK */
				debug_printf("Sent from IR: NACK\n\r");
				irState = NACK_STATE;
			} else if (currentMode == '6') {

				/* Radio mode: send radio ERR */
				radio_send_packet(radioErr);
			}
			break;
		} else {

			errIndicator = 0;
		}

		if (raRxPacket[i] == 0x00) {
			for (int k = 0; k < 2; k++) {
				if (k == 0) {

					for (int l = 0; l < 5; l++) {
						recvType[l] = radioAck[l];
					}
				} else {

					for (int l = 0; l < 5; l++) {
						recvType[l] = radioErr[l];
					}
				}
				if (((i - 10) / 2) == 5) {
					for (int j = 0; j < 5; j++) {

						if (deBuffer[j] == recvType[j]) {

							if (j == 4) {

								raAckErrFlag = k;
								raRecvFlag = 0;

								if (k == 0) {

									/* Received an ACK */
									ackIndicator = 0;
								}
							}
						} else {

							break;
						}
					}
				}
			}

			debug_printf("Received from Radio: ");
			for (int j = 0; j < (i - 10) / 2; j++) {
				debug_printf("%c", deBuffer[j]);
			}
			debug_printf("\n\r");

			if (raAckErrFlag == -1) {

				if (currentMode == '6') {

					/* If in radio mode send an ACK over radio */
					radio_send_packet(radioAck);
				} else if (currentMode == '7') {

					/* If in duplex mdoe send an ACK over ir */
					sendIndicator = HAL_GetTick();
					debug_printf("Sent from IR: ACK\n\r");
					irState = ACK_STATE;
				}
			} else if (raAckErrFlag) {

				/* Resend payload if ERR */
				ackIndicator = 1;
				radio_send_packet(raPayload);
			}

			break;
		}
	}
}

/*
 * @brief Function which ultimately handles the printing
 * and resending operations for the radio.
 * @param  None
 * @retval None
 */
void radio_handler(void) {

	if (s4398172_hal_radio_getrxstatus() == 1) {

		recvIndicator = HAL_GetTick();
		s4398172_hal_radio_getpacket(raRxPacket);
		radio_packet_manipulate();
		s4398172_hal_radio_setrxstatus(0);
	} else if (raRecvFlag) {

		/* Resend 3 times if ACK not received */
		if ((HAL_GetTick() - raPacketSent + 1) % 3000 == 0) {

			if (raRecvFlag == 3) {

				debug_printf("After 3 transmits giving up.\n\r");
				ackIndicator = 0;
				raRecvFlag = 0;
			} else {

				ackIndicator = 1;
				radio_send_packet(raPayload);
				raRecvFlag++;
			}
		}
	}
}

/*
 * @brief Function which ultimately handles the printing
 * and resending operations for the IR transceiver.
 * @param  None
 * @retval None
 */
void ir_handler(void) {

	if (irPrintAck) {

		recvIndicator = HAL_GetTick();
		ackIndicator = 0;
		debug_printf("Received from IR: ACK\n\r");
		irRecvFlag = 0;
		irPrintAck = 0;
	} else if (irPrintNACK) {

		recvIndicator = HAL_GetTick();
		debug_printf("Received from IR: NACK\n\r");
		irPrintNACK = 0;
	} else if (irRecvFlag) {

		if ((HAL_GetTick() - irPacketSent + 1) % 3000 == 0) {

			/* Resend the packet if an ACK/NACK was not received */
			if (irRecvFlag == 3) {

				debug_printf("After 3 transmits giving up.\n\r");
				ackIndicator = 0;
				irRecvFlag = 0;
			} else {

				sendIndicator = HAL_GetTick();
				debug_printf("Sent from IR: ");
				for (int i = 2; i < numChars; i++) {
					debug_printf("%c", irCommand[i]);
				}
				debug_printf("\n\r");

				irTxPacket = 0;
				/* Resend the same string */
				irState = STX_STATE;
				irRecvFlag++;
			}
		}
	}

	if (irRecvPrint) {

		recvIndicator = HAL_GetTick();

		debug_printf("Received from IR: ");
		for (int i = 0; i < irRecvPrint; i++) {
			debug_printf("%c", deIrRxData[i]);
		}
		debug_printf("\n\r");
		debug_printf("Sent from IR: ACK\n\r");

		/* Received properly so send an ACK */
		irState = ACK_STATE;
		irRecvPrint = 0;
	}
}

int main(void) {

	BRD_init();
	peripheral_init();
	fill_arb_packets();

	debug_printf("CSSE3010 Project 1 : Joshua Salecich : s43981722\n\r");

	while (1) {

		radio_handler();
		ir_handler();
		get_console();
		ledbar_handler();
	}
}

/**
 * @brief Simply toggle the bit relative to the current packet.
 * Also wait between the sending of each packet.
 * @param  None
 * @retval None
 */
void ir_send_packet(void) {

	if (irWaitState) {
		/* Wait an additional 5ms */
		irWaitState++;
		if (irWaitState == WAIT_FACTOR) {
			/* Wait 40ms in total */
			irWaitState = 0;
		}
	} else {
		switch (irState) {

			case STX_STATE:
				if (arbIndex < IR_PACKET_SIZE) {

					/* Send MSB first */
					if ((strIrPacks[STX_STATE] >> (21 - arbIndex)) & 1) {
						s4398172_hal_ir_datamodulation_set();
					} else {
						s4398172_hal_ir_datamodulation_clr();
					}
					arbIndex++;
				} else {

					irWaitState = 1;
					arbIndex = 0;
					irState = PACKET;
				}
				break;

			case PACKET:
				if (irTxPacket < (numChars - 2)) { // -2, IT

					if (irTxIndex < IR_PACKET_SIZE) {

						if ((enIrTxPacks[irTxPacket] >>
								(21 - irTxIndex)) & 1) { //MSB first
							s4398172_hal_ir_datamodulation_set();
						} else {
							s4398172_hal_ir_datamodulation_clr();
						}
						irTxIndex++;
					} else {

						/* Move onto next packet */
						irWaitState = 1;
						irTxIndex = 0;
						irTxPacket++;
					}
				} else {
					irState = ETX_STATE;
				}
				break;

			case ETX_STATE:
				if (arbIndex < IR_PACKET_SIZE) {

					if ((strIrPacks[ETX_STATE] >> (21 - arbIndex)) & 1) {
						s4398172_hal_ir_datamodulation_set();
					} else {
						s4398172_hal_ir_datamodulation_clr();
					}
					arbIndex++;
				} else {

					irWaitState = 1;
					arbIndex = 0;
					irState = DEFAULT;
				}
				break;

			case ACK_STATE:
				if (arbIndex < IR_PACKET_SIZE) {

					if ((strIrPacks[ACK_STATE] >> (21 - arbIndex)) & 1) {
						s4398172_hal_ir_datamodulation_set();
					} else {
						s4398172_hal_ir_datamodulation_clr();
					}
					arbIndex++;
				} else {

					sendIndicator = HAL_GetTick();
					irWaitState = 1;
					arbIndex = 0;
					irState = DEFAULT;
				}
				break;

			case NACK_STATE:
				if (arbIndex < IR_PACKET_SIZE) {

					if ((strIrPacks[NACK_STATE] >> (21 - arbIndex)) & 1) {
						s4398172_hal_ir_datamodulation_set();
					} else {
						s4398172_hal_ir_datamodulation_clr();
					}
					arbIndex++;
				} else {

					sendIndicator = HAL_GetTick();
					irWaitState = 1;
					arbIndex = 0;
					irState = DEFAULT;
				}
				break;

			default:
				break;
		}
	}
}

/**
 * @brief When a rising edge on pin 38 is noticed this function is computed.
 * 		  Compare this rising edge to the previous rising edge to determine
 * 		  the period. From this bits are appended to create the received
 * 		  character. Must be called within a bothedge capture interrupt.
 * @param  None
 * @retval None
 */
void ir_recv_packet(void) {

	/* Fill Packet MSB first*/
	int writeVal;
	pulseTime = interruptTime - prevTime;

	prevTime = interruptTime;

	if (HAL_GPIO_ReadPin(BRD_D38_GPIO_PORT, BRD_D38_PIN)) {
		writeVal = 1; //needs to be inverted because of MOSFET
	} else {
		writeVal = 0;
	}

	if (pulseTime == (2 * BIT_DELAY) || pulseTime == (2 * BIT_DELAY + 1)) {
		enIrRxPacks[irRxPacket] ^= (-writeVal ^ enIrRxPacks[irRxPacket])
				& (1 << irRxIndex--);
		enIrRxPacks[irRxPacket] ^= (-writeVal ^ enIrRxPacks[irRxPacket])
				& (1 << irRxIndex--);
	} else {
		enIrRxPacks[irRxPacket] ^= (-writeVal ^ enIrRxPacks[irRxPacket])
				& (1 << irRxIndex--);
	}

	if (irRxIndex == 0) {
		enIrRxPacks[irRxPacket] &= ~(1 << irRxIndex); //append last 0

		/* Decode the packet and analyse its contents */

		for (int i = 17; i > 1; i--) {
			if ((enIrRxPacks[irRxPacket] >> i) & 1) {
				enIrRxData[irRxPacket] |= 1 << (i - 2);
			} else {
				enIrRxData[irRxPacket] &= ~(1 << (i - 2));
			}
		}

		deIrRxData[irRxPacket] = s4398172_man_decode(enIrRxData[irRxPacket]);

		switch(deIrRxData[irRxPacket]) {
			case STX_VAL:
				irRxPacket = 0; //so STX isn't printed
				break;

			case ETX_VAL:
				irRecvPrint = irRxPacket;
				irRxPacket = 0;
				break;

			case ACK_VAL:
				irPrintAck = 1;
				irRxPacket = 0;

				/* If in duplex mode, this indicates radio packet
				 * was received. So don't resend. */
				if (currentMode == '7') {
					raRecvFlag = 0;
				}
				break;

			case NACK_VAL:
				irPrintNACK = 1;
				/* If IR receives NACK we are in Full Duplex
				 * Respond with resending the radio packet */
				radio_send_packet(raPayload);
				irRxPacket = 0;
				break;

			default:
				irRxPacket++;
				break;
		}

		irRxIndex = 21;
	}
}

/**
 * @brief Goes into the ir_recv_packet(); function whenever a rising
 * or falling edge is triggered.
 * @param  Pointer to the base handler for the respective timer (TIM1).
 * @retval None
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	ir_recv_packet();
}

/**
 * @brief  Call the FSM switch statement on a 5ms interval.
 * 		   Activate the ir_send function based on the bit delay (5ms).
 * @param  Pointer to the base handler for the respective timer (TIM3).
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	/* Every increment is equal to 1ms */
	interruptTime++;

	/* Radio process */
	if (interruptTime % 5 == 0) {
		/* We don't want the radio fsm running every 1ms */
		s4398172_hal_radio_fsmprocessing();
	}

	/* IR process */
	if (interruptTime % BIT_DELAY == 0) {
		ir_send_packet();
	}
}

/*
 * @brief Override default mapping of the TIM3_IRQHandler to Default_Handler.
 * @param  None
 * @retval None
 */
void TIM3_IRQHandler(void) {

	HAL_TIM_IRQHandler(&TIM_3_Init);
}


/**
 * @brief Override default mapping of the TIM1_CC_IRQHandler to Default_Handler.
 * @param  None
 * @retval None
 */
void TIM1_CC_IRQHandler(void) {

	HAL_TIM_IRQHandler(&TIM_1_Init);
}
