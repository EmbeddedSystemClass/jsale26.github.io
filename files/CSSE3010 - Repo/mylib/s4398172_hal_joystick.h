/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_joystick.h
 * @author  Joshua Salecich � 43981722
 * @date    10/03/2018
 * @brief  	Joystick peripheral driver
 *	     REFERENCE: joystick_datasheet.pdf
 *
 *			NOTE: REPLACE s4398172 with your student login.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_hal_joystick_x_read()
 * s4398172_hal_joystick_y_read()
 ******************************************************************************
 */

#ifndef s4398172_HAL_JOYSTICK_H
#define s4398172_HAL_JOYSTICK_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define X_MODE 0
#define Y_MODE 1
/* Joystic x read */
#define s4398172_hal_joystick_x_read() joystick_read(X_MODE)
/* Joystick y read */
#define s4398172_hal_joystick_y_read() joystick_read(Y_MODE)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
int joystick_read(int type);
void s4398172_joystick_init(void);

#endif

