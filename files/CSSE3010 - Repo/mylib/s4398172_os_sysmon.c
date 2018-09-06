/**
 ******************************************************************************
 * @file    mylib/s4398172_os_sysmon.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   System Monitor functionality and task.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "s4398172_os_sysmon.h"
#include "s4398172_os_printf.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"

/* Defines -------------------------------------------------------------------*/
#define mainSysmonTASK_PRIORITY			(tskIDLE_PRIORITY + 2)

#define mainSysmonTASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 2)

/* Private functions ---------------------------------------------------------*/

/**
  * @brief 	Sysmon Task.
  * @param  None
  * @retval None
  */
void s4398172_TaskSysmon(void) {

	for (;;) {
		BRD_LEDBlueToggle();
		if (s4398172_SemaphoreGetSys != NULL) {

			if (xSemaphoreTake(s4398172_SemaphoreGetSys, 10) == pdTRUE) {
				myprintf("%d\n\r", xTaskGetTickCount());
			}
		}

		vTaskDelay(20);
	}
}

/**
  * @brief  Initialise tasks, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_sysmon_init(void) {

	xTaskCreate((void *) &s4398172_TaskSysmon, (const char *) "SysmonTask",
			mainSysmonTASK_STACK_SIZE, NULL,
			mainSysmonTASK_PRIORITY, &s4398172_HandlerSysmon);
	s4398172_SemaphoreGetSys = xSemaphoreCreateBinary();
}

/**
  * @brief  Deinitialise tasks, semaphores and queues.
  * @param  None
  * @retval None
  */
void s4398172_os_sysmon_deinit(void) {

	vSemaphoreDelete(s4398172_SemaphoreGetSys);

	vTaskDelete(s4398172_HandlerSysmon);
}
