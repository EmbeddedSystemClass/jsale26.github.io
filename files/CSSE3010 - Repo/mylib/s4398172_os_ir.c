/**
 ******************************************************************************
 * @file    mylib/s4398172_os_ir.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief	IR Control functionality and Task.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_ir.h"
#include "s4398172_os_printf.h"
#include "s4398172_os_radio.h"
#include "s4398172_hal_coords.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define mainIRTASK_PRIORITY		(tskIDLE_PRIORITY + 2)

#define mainIRTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

#define PAYLOAD_LEN 32
#define DECODED_LEN 16

/* Function Prototypes -------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int upKey[DECODED_LEN] = {0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1};
int downKey[DECODED_LEN] = {1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1};
int leftKey[DECODED_LEN] = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
int rightKey[DECODED_LEN] = {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief 	Compares the received signal to the ones seen for the registered
  * 		key strokes. Computes relevant command if a match is found.
  * @param  int *decodedBuffer: received impulses.
  * @retval None
  */
void execute_keypress(int *decodedBuffer) {

	for (int i = 0; i < DECODED_LEN; i++) {

		if (decodedBuffer[i] != upKey[i]) {
			break;
		} else if (i == DECODED_LEN - 1) {
			s4398172_update_xy_increment(10, Y_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}
			return;
		}
	}

	for (int i = 0; i < DECODED_LEN; i++) {

		if (decodedBuffer[i] != downKey[i]) {
			break;
		} else if (i == DECODED_LEN - 1) {
			s4398172_update_xy_increment(-10, Y_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}
			return;
		}
	}

	for (int i = 0; i < DECODED_LEN; i++) {

		if (decodedBuffer[i] != leftKey[i]) {
			break;
		} else if (i == DECODED_LEN - 1) {
			s4398172_update_xy_increment(-10, X_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}
			return;
		}
	}

	for (int i = 0; i < DECODED_LEN; i++) {

		if (decodedBuffer[i] != rightKey[i]) {
			break;
		} else if (i == DECODED_LEN - 1) {
			s4398172_update_xy_increment(10, X_COORD);

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToBack(s4398172_QueuePacket,
						(void *) &s4398172_payload, (portTickType) 10);
			}
			return;
		}
	}
}

/**
  * @brief 	IR Task. Handles IR signals sent by the NEC protocol remote.
  * @param  None
  * @retval None
  */
void s4398172_TaskIR(void) {

	uint32_t recvIR[2], previousTime = 0, startTime = 0;
	uint32_t timeDiff, timeBuffer[PAYLOAD_LEN];
	int bitBuffer[PAYLOAD_LEN], decodedBuffer[DECODED_LEN];
	int index = 0, edgeNum = 0;

	for (;;) {

		if (s4398172_QueueIR != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueIR, &recvIR, 10)) {

				if ((recvIR[TIME] - previousTime) <= 4000) {

					if (edgeNum == 0) {
						startTime = recvIR[TIME];
					}

					if ((recvIR[TIME] - startTime) > 27000) {

						if (index == PAYLOAD_LEN) {

							index = 0;

							for (int i = 0; i < PAYLOAD_LEN - 1; i++) {

								if (bitBuffer[i] == 0) {

									timeDiff = timeBuffer[i + 1]
														  - timeBuffer[i];

									if (timeDiff > 1000) {

										decodedBuffer[index++] = 1;
									} else {

										decodedBuffer[index++] = 0;
									}
								}
							}

							execute_keypress(decodedBuffer);

						} else {

							timeBuffer[index] = recvIR[TIME] - startTime;
							bitBuffer[index] = !(int)recvIR[BIT];
							index++;
						}
					}

					edgeNum++;
				} else {

					edgeNum = 0;
					index = 0;
				}
				previousTime = recvIR[TIME];
			}
		}

		vTaskDelay(10);
	}
}

/**
  * @brief  Initialise task, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_ir_init(void) {

	xTaskCreate((void *) &s4398172_TaskIR, (const char *) "IR",
			mainIRTASK_STACK_SIZE, NULL,
			mainIRTASK_PRIORITY, &s4398172_HandlerIR);

	s4398172_QueueIR = xQueueCreate(100, sizeof(uint32_t) * 2);
}

/**
  * @brief  Deinitialise task, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_ir_deinit(void) {

	vQueueDelete(s4398172_QueueIR);

	vTaskDelete(s4398172_HandlerIR);
}
