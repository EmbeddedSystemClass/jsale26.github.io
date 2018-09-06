/**
 ******************************************************************************
 * @file    mylib/s4398172_os_printf.c
 * @author  Joshua Salecich � 43981722
 * @date    5/05/2018
 * @brief   Make debug_printf safe.
 *
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_printf.h"

#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>

/* Defines -------------------------------------------------------------------*/
#define PAYLOAD_LEN 128

#define mainPrintTASK_PRIORITY			(tskIDLE_PRIORITY + 2)
#define mainPrintTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 4)

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Init print task, semaphore and queue
  * @param  None
  * @retval None
  */
void s4398172_os_myprintf_init(void) {

	xTaskCreate((void *) &s4398172_TaskPrintf, (const char *) "PRINT",
			mainPrintTASK_STACK_SIZE, NULL,
			mainPrintTASK_STACK_SIZE, &s4398172_HandlerPrintf);

	s4398172_PrintMutex = xSemaphoreCreateMutex();
	s4398172_QueuePrintf = xQueueCreate(10, sizeof(char) * PAYLOAD_LEN);
}

/**
  * @brief  Init print task, semaphore and queue
  * @param  None
  * @retval None
  */
void s4398172_os_myprintf_deinit(void) {

	vSemaphoreDelete(s4398172_PrintMutex);

	vQueueDelete(s4398172_QueuePrintf);

	vTaskDelete(s4398172_HandlerPrintf);
}

/**
  * @brief  Print Task. Used to print messages safely.
  * @param  None
  * @retval None
  */
void s4398172_TaskPrintf(void) {

	char recvPayload[PAYLOAD_LEN];

	for (;;) {

		BRD_LEDRedToggle();

		for (int i = 0; i < PAYLOAD_LEN; i++) {
			recvPayload[i] = 0;
		}

		if (s4398172_QueuePrintf != NULL) {	/* Check if queue exists */

			/* Check for item received - block atmost for 10 ticks */
			if (xQueueReceive(s4398172_QueuePrintf, &recvPayload, 10)) {

				if (xSemaphoreTake(s4398172_PrintMutex, 10) == pdTRUE) {

					/* display received item */
					debug_printf("%s", recvPayload);
					debug_flush();

					vTaskDelay(10);

					xSemaphoreGive(s4398172_PrintMutex);
				}
			}
		}

		/* Delay for 10ms */
		vTaskDelay(10);
	}
}

/**
  * @brief  Printf handler
  * @param  input is a given string
  * @param	variadic input
  * @retval None
  */
void my_printf(char *input, ...) {

	char payload[PAYLOAD_LEN];

	if (xSemaphoreTake(s4398172_PrintMutex, 10) == pdTRUE) {

		for (int i = 0; i < PAYLOAD_LEN; i++) {
			payload[i] = 0;
		}

		va_list info;

		va_start(info, input);
		vsprintf(payload, input, info);
		va_end(info);

		xQueueSendToBack(s4398172_QueuePrintf, (void *) &payload,
				(portTickType) 10);

		xSemaphoreGive(s4398172_PrintMutex);
	}
}



