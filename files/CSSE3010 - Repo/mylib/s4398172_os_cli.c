/**
 ******************************************************************************
 * @file    mylib/s4398172_os_cli.c
 * @author  Joshua Salecich � 43981722
 * @date    7/05/2018
 * @brief   Initialise CLI.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_printf.h"
#include "s4398172_hal_pantilt.h"
#include "s4398172_os_cli.h"

#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define mainCLITASK_PRIORITY		(tskIDLE_PRIORITY + 2)

#define mainCLITASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

#define PAYLOAD_LEN 128


/* Function Prototypes -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CLI Receive Task.
  * @param  None
  * @retval None
  */
void s4398172_CLITask(void) {

	char cRxedChar;
	char cInputString[PAYLOAD_LEN];
	int inputIndex = 0;
	char *pcOutputString;
	BaseType_t xReturned;

	/* Initialise pointer to CLI output buffer. */
	memset(cInputString, 0, sizeof(cInputString));
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	for (;;) {

		/* Receive character from terminal */
		cRxedChar = debug_getc();

		/* Process if character if not Null */
		if (cRxedChar != '\0') {

			/* Echo character */
			//myprintf("%c", cRxedChar);
			debug_putc(cRxedChar);

			/* Process only if return is received. */
			if (cRxedChar == '\r') {

				//Put new line and transmit buffer
				debug_putc('\n');
				//myprintf("%c", "\n");
				debug_flush();

				/* Put null character in command input string. */
				cInputString[inputIndex] = '\0';

				xReturned = pdTRUE;

				/* Process command input string. */
				while (xReturned != pdFALSE) {

					/* Returns pdFALSE, when all strings have been returned */
					xReturned = FreeRTOS_CLIProcessCommand(cInputString,
							pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);
					myprintf("%s", pcOutputString);
				}

				memset(cInputString, 0, sizeof(cInputString));
				inputIndex = 0;

			} else {

				debug_flush();		//Transmit USB buffer

				if( cRxedChar == '\r' ) {

					/* Ignore the character. */
				} else if( cRxedChar == '\b' ) {

					/* Backspace was pressed.  Erase the last character in the
					 string - if any.*/
					if( inputIndex > 0 ) {
						inputIndex--;
						cInputString[ inputIndex ] = '\0';
					}

				} else {

					/* A character was entered.  Add it to the string
					   entered so far.  When a \n is entered the complete
					   string will be passed to the command interpreter. */
					if( inputIndex < 40 ) {
						cInputString[ inputIndex ] = cRxedChar;
						inputIndex++;
					}
				}
			}
		}

		vTaskDelay(50);
	}
}

/**
  * @brief  Create CLI task
  * @param  None
  * @retval None
  */
void s4398172_os_cli_init(void) {

	xTaskCreate((void *) &s4398172_CLITask, (const char *) "CLI",
			mainCLITASK_STACK_SIZE, NULL,
			mainCLITASK_PRIORITY, &s4398172_HandlerCLI);

}

/**
  * @brief  Delete CLI task
  * @param  None
  * @retval None
  */
void s4398172_os_cli_deinit(void) {

	vTaskDelete(s4398172_HandlerCLI);

}
