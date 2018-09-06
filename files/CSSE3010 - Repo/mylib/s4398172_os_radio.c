/**
 ******************************************************************************
 * @file    mylib/s4398172_os_radio.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Radio control functionality and tasks.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <s4398172_cli_radio.h>
#include <s4398172_os_radio.h>
#include "s4398172_os_printf.h"
#include "s4398172_hal_radio.h"
#include "s4398172_hal_coords.h"
#include "s4398172_hal_hamming.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define mainRadioTASK_PRIORITY			(tskIDLE_PRIORITY + 2)

#define mainRadioTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 5)

#define mainRadioFSMTASK_PRIORITY		(tskIDLE_PRIORITY + 4)

#define mainRadioFSMTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 5)

#define BUFFER_LEN 5
#define PACKET_SIZE 12
#define JOIN_LEN 4
#define MAX_PACKET_LEN 32

/* Private variables ---------------------------------------------------------*/
unsigned char rxPacket[MAX_PACKET_LEN];
int ackFlag = 1, packetSent;
char currentPacket[PACKET_SIZE];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief 	Radio Task. Handles radio getting and setting functionality. Also
  * 		receiving packets from other libraries.
  * @param  None
  * @retval None
  */
void s4398172_TaskRadio(void) {

	unsigned char recvChan;
	unsigned char addrBuffer[BUFFER_LEN];
	char recvPacket[PACKET_MAX];

	for (int i = 0; i < BUFFER_LEN; i++) {
		addrBuffer[i] = 0;
	}

	for (;;) {

		if (s4398172_QueueSetChan != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueSetChan, &recvChan, 10)) {
				s4398172_hal_radio_setchan(recvChan);
			}
		}

		if (s4398172_QueueSetTxAddr != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueSetTxAddr, &addrBuffer, 10)) {
				s4398172_hal_radio_settxaddress(addrBuffer);
			}
		}

		if (s4398172_QueueSetRxAddr != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueSetRxAddr, &addrBuffer, 10)) {
				s4398172_hal_radio_setrxaddress(addrBuffer);
			}
		}

		if (ackFlag) {

			if (s4398172_QueuePacket != NULL) {	/* Check if queue exists. */

				/* Check for item received */
				if (xQueueReceive(s4398172_QueuePacket, &recvPacket, 10)) {

					myprintf("%s\n\r", recvPacket);
					strcpy(currentPacket, recvPacket);
					packetSent = xTaskGetTickCount();
					s4398172_set_payload((unsigned char *)recvPacket);
					ackFlag = 0;
				}
			}

		} else if (xTaskGetTickCount() >= packetSent + 3000) {

			if (s4398172_QueuePacket != NULL) {

				xQueueSendToFront(s4398172_QueuePacket,
						(void *) &currentPacket, (portTickType) 10);
			}
			packetSent = xTaskGetTickCount();
			ackFlag = 1;
		}

		if (s4398172_SemaphoreGetChan != NULL) {

			if (xSemaphoreTake(s4398172_SemaphoreGetChan, 10) == pdTRUE) {

				recvChan = s4398172_hal_radio_getchan();
				myprintf("%d\n\r", (uint8_t)recvChan);
			}
		}

		if (s4398172_SemaphoreGetTxAddr != NULL) {

			if (xSemaphoreTake(s4398172_SemaphoreGetTxAddr, 10) == pdTRUE) {

				s4398172_hal_radio_gettxaddress(addrBuffer);
				myprintf("%s", "0x");
				for (int i = (BUFFER_LEN - 2); i >= 0; i--) {

					if (addrBuffer[i] == 0x00) {
						myprintf("%X0", addrBuffer[i]);
					} else {
						myprintf("%X", addrBuffer[i]);
					}
				}
				myprintf("%s", "\n\r");
			}
		}

		if (s4398172_SemaphoreGetRxAddr != NULL) {

			if (xSemaphoreTake(s4398172_SemaphoreGetRxAddr, 10) == pdTRUE) {

				s4398172_hal_radio_getrxaddress(addrBuffer);

				myprintf("%s", "0x");
				for (int i = (BUFFER_LEN - 2); i >= 0; i--) {
					myprintf("%X", addrBuffer[i]);
				}
				myprintf("%s", "\n\r");
			}
		}
	}
}

/**
  * @brief 	Manipulate received packet, check for an ack and
  * 		retransmit if required.
  * @param  None
  * @retval None
  */
void radio_packet_manipulate(void) {
	uint16_t enBuffer;
	uint8_t deBuffer[PACKET_MAX];

	for (int i = 10; i <= MAX_PACKET_LEN; i += 2) {

		enBuffer = (rxPacket[i] << 8) | (rxPacket[i + 1] & 0xFF);
		deBuffer[(i - 10) / 2] = s4398172_ham_decode(enBuffer, 0);
	}

	if (strcmp((char *)deBuffer, "A C K") == 0) {
		ackFlag = 1;
	}
	myprintf("Received from Radio: %s\n\r", deBuffer);
}

/**
  * @brief 	Radio FSM Task. Handles sending packets via the fsm_proc.
  * @param  None
  * @retval None
  */
void s4398172_TaskRadioFSM(void) {

	for (;;) {

		if (s4398172_hal_radio_getrxstatus() == 1) {
			s4398172_hal_radio_getpacket(rxPacket);
			radio_packet_manipulate();
			s4398172_hal_radio_setrxstatus(0);
		}
		s4398172_hal_radio_fsmprocessing();
		vTaskDelay(50);
	}
}

/**
  * @brief  Initialise tasks, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_radio_init(void) {

	xTaskCreate((void *) &s4398172_TaskRadio, (const char *) "RadioTask",
			mainRadioTASK_STACK_SIZE, NULL,
			mainRadioTASK_PRIORITY, &s4398172_HandlerRadio);

	xTaskCreate((void *) &s4398172_TaskRadioFSM, (const char *) "RadioFSMTask",
			mainRadioFSMTASK_STACK_SIZE, NULL,
			mainRadioFSMTASK_PRIORITY, &s4398172_HandlerRadioFSM);

	s4398172_QueueSetChan = xQueueCreate(10, sizeof(int));
	s4398172_QueueSetTxAddr = xQueueCreate(10,
			sizeof(unsigned char) * BUFFER_LEN);
	s4398172_QueueSetRxAddr = xQueueCreate(10,
			sizeof(unsigned char) * BUFFER_LEN);
	s4398172_QueuePacket = xQueueCreate(200,
			sizeof(unsigned char) * PACKET_MAX);

	s4398172_SemaphoreGetChan = xSemaphoreCreateBinary();
	s4398172_SemaphoreGetTxAddr = xSemaphoreCreateBinary();
	s4398172_SemaphoreGetRxAddr = xSemaphoreCreateBinary();
	s4398172_SemaphoreJoin = xSemaphoreCreateBinary();
}

/**
  * @brief  Deinitialise tasks, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_radio_deinit(void) {

	vSemaphoreDelete(s4398172_SemaphoreGetChan);
	vSemaphoreDelete(s4398172_SemaphoreGetTxAddr);
	vSemaphoreDelete(s4398172_SemaphoreGetRxAddr);
	vSemaphoreDelete(s4398172_SemaphoreJoin);

	vQueueDelete(s4398172_QueueSetChan);
	vQueueDelete(s4398172_QueueSetTxAddr);
	vQueueDelete(s4398172_QueueSetRxAddr);
	vQueueDelete(s4398172_QueuePacket);

	vTaskDelete(s4398172_HandlerRadio);
	vTaskDelete(s4398172_HandlerRadioFSM);
}
