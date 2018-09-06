/**
 ******************************************************************************
 * @file    mylib/s4398172_os_joystick.h
 * @author  Joshua Salecich � 43981722
 * @date    28/04/2018
 * @brief   Initialise joystick Z semaphore.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_ledbar_init() � Initialise joystick Z semaphore.
 ******************************************************************************
 */

#ifndef s4398172_OS_JOYSTICK_H_
#define s4398172_OS_JOYSTICK_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t s4398172_SemaphoreJoystickZ;

/* External function prototypes -----------------------------------------------*/
void s4398172_os_joystick_init(void);

#endif

