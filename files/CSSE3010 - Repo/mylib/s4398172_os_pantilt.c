/**
 ******************************************************************************
 * @file    mylib/s4398172_os_pantilt.c
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Pantilt Control functionality and task.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_pantilt.h"
#include "s4398172_os_printf.h"
#include "s4398172_hal_pantilt.h"

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
#define PLOTTER_VELOCITY 20
#define X 0
#define Y 1

/* Function Prototypes -------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int maxAngle[2] = {0, -30}, minAngle[2] = {-40, -50};


/* Private functions ---------------------------------------------------------*/

/**
  * @brief 	Convert coordinate to angle via scaling.
  * @param  int coord: given coordinate.
  * @param 	int type: coordinate type (x, y)
  * @retval None
  */
int coord_to_angle(int coord, int type) {

	double outEnd = maxAngle[type], outStart = minAngle[type],
			inEnd = 200, inStart = 0, gradient, scaled;

	/* (y2 - y1)/(x2-x1) then convert to double */
	gradient = 1.0 * (outEnd - outStart) / (inEnd - inStart);
	/* y = mx+c */
	scaled = gradient * (coord - inStart) + outStart;

	return (int)scaled;
}

/**
  * @brief 	Pantilt Task. Handles pantilt movement based on given coordinates.
  * @param  None
  * @retval None
  */
void s4398172_TaskPanTilt(void) {
	int currentAngle, recvAngle, boundAngle[2];
	int recvCoord[2], prevCoord[2] ={0, 0};
	int msDelay = 0;

	for (;;) {

		BRD_LEDRedToggle();
		vTaskDelay(100);

		currentAngle = s4398172_hal_pantilt_pan_read();

		if (xSemaphoreTake(s4398172_SemaphorePanLeft, 1) == pdTRUE) {

			if ((currentAngle - ANGLE_DIFF) >= -MAX_ANGLE) {

				s4398172_hal_pantilt_pan_write(currentAngle - ANGLE_DIFF);
			}
		}

		if (xSemaphoreTake(s4398172_SemaphorePanRight, 1) == pdTRUE) {

			if ((currentAngle + ANGLE_DIFF) <= MAX_ANGLE) {
				s4398172_hal_pantilt_pan_write(currentAngle + ANGLE_DIFF);
			}
		}

		currentAngle = s4398172_hal_pantilt_tilt_read();
		if (xSemaphoreTake(s4398172_SemaphoreTiltDown, 1) == pdTRUE) {

			if ((currentAngle - ANGLE_DIFF) >= -MAX_ANGLE) {

				s4398172_hal_pantilt_tilt_write(currentAngle - ANGLE_DIFF);
			}
		}

		if (xSemaphoreTake(s4398172_SemaphoreTiltUp, 1) == pdTRUE) {

			if ((currentAngle + ANGLE_DIFF) <= MAX_ANGLE) {
				s4398172_hal_pantilt_tilt_write(currentAngle + ANGLE_DIFF);
			}
		}

		if (s4398172_QueuePan != NULL) {	/* Check if queue exists. */

			/* Check for item received. */
			if (xQueueReceive(s4398172_QueuePan, &recvAngle, 10)) {

				s4398172_hal_pantilt_pan_write(recvAngle);
			}
		}

		if (s4398172_QueueCoord != NULL) {	/* Check if queue exists. */

			/* Check for item received. */
			if (xQueueReceive(s4398172_QueueCoord, &recvCoord, 10)) {

				recvAngle = coord_to_angle(recvCoord[0], recvCoord[1]);

				if (recvCoord[1] == PAN) {

					currentAngle = s4398172_hal_pantilt_pan_read();
					msDelay = ((abs(recvCoord[0] - prevCoord[X])
							* PLOTTER_VELOCITY) / abs(recvAngle - currentAngle));

					if (recvAngle > currentAngle) {

						for (int i = currentAngle; i <= recvAngle; i++) {

							s4398172_hal_pantilt_pan_write(i);
							vTaskDelay(msDelay);
						}
					} else {

						for (int i = currentAngle; i >= recvAngle; i--) {

							s4398172_hal_pantilt_pan_write(i);
							vTaskDelay(msDelay);
						}
					}

					prevCoord[X] = recvCoord[0];
				} else {

					currentAngle = s4398172_hal_pantilt_tilt_read();
					msDelay = ((abs(recvCoord[0] - prevCoord[Y])
							* PLOTTER_VELOCITY)
							/ abs(recvAngle - currentAngle));

					if (recvAngle > currentAngle) {

						for (int i = currentAngle; i <= recvAngle; i++) {

							s4398172_hal_pantilt_tilt_write(i);
							vTaskDelay(msDelay);
						}
					} else {

						for (int i = currentAngle; i >= recvAngle; i--) {

							s4398172_hal_pantilt_tilt_write(i);
							vTaskDelay(msDelay);
						}
					}

					prevCoord[Y] = recvCoord[0];
				}
				vTaskDelay(500);
			}
		}

		if (s4398172_QueueTilt != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueTilt, &recvAngle, 10)) {

				s4398172_hal_pantilt_tilt_write(recvAngle);
			}
		}

		if (s4398172_QueueSetMin != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueSetMin, &boundAngle, 10)) {

				minAngle[TILT] = boundAngle[TILT];
				minAngle[PAN] = boundAngle[PAN];
			}
		}

		if (s4398172_QueueSetMax != NULL) {	/* Check if queue exists. */

			/* Check for item received */
			if (xQueueReceive(s4398172_QueueSetMax, &boundAngle, 10)) {

				maxAngle[TILT] = boundAngle[TILT];
				maxAngle[PAN] = boundAngle[PAN];
			}
		}


	}
}

/**
  * @brief  Initialise task, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_pantilt_init(void) {

	xTaskCreate((void *) &s4398172_TaskPanTilt, (const char *) "PanTilt",
			mainPanTiltTASK_STACK_SIZE, NULL,
			mainPanTiltTASK_PRIORITY, &s4398172_HandlerPanTilt);

	s4398172_QueuePan = xQueueCreate(10, sizeof(int));
	s4398172_QueueTilt = xQueueCreate(10, sizeof(int));
	s4398172_QueueCoord = xQueueCreate(100, sizeof(int) * 2);
	s4398172_QueueSetMin = xQueueCreate(10, sizeof(int) * 2);
	s4398172_QueueSetMax = xQueueCreate(10, sizeof(int) * 2);

	s4398172_SemaphorePanLeft = xSemaphoreCreateBinary();
	s4398172_SemaphorePanRight = xSemaphoreCreateBinary();
	s4398172_SemaphoreTiltUp = xSemaphoreCreateBinary();
	s4398172_SemaphoreTiltDown = xSemaphoreCreateBinary();
}

/**
  * @brief  Denitialise task, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_pantilt_deinit(void) {

	vQueueDelete(s4398172_QueuePan);
	vQueueDelete(s4398172_QueueTilt);
	vQueueDelete(s4398172_QueueCoord);
	vQueueDelete(s4398172_QueueSetMin);
	vQueueDelete(s4398172_QueueSetMax);

	vSemaphoreDelete(s4398172_SemaphorePanLeft);
	vSemaphoreDelete(s4398172_SemaphorePanRight);
	vSemaphoreDelete(s4398172_SemaphoreTiltUp);
	vSemaphoreDelete(s4398172_SemaphoreTiltDown);

	vTaskDelete(s4398172_HandlerPanTilt);
}
