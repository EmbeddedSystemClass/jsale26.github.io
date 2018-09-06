/**
 ******************************************************************************
 * @file    mylib/s4398172_os_joystick.c
 * @author  Joshua Salecich � 43981722
 * @date    28/04/2018
 * @brief   Initialise joystick Z semaphore.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_os_joystick_init() � Initialise joystick Z semaphore.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_os_joystick.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialise joystick z semaphore.
  * @param  None
  * @retval None
  */
void s4398172_os_joystick_init(void) {
	s4398172_SemaphoreJoystickZ = xSemaphoreCreateBinary();
}



