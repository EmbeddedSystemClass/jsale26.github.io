/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_pantilt.h
 * @author  Joshua Salecich � 43981722
 * @date    04/02/2018
 * @brief   Hektronik HXT900 peripheral driver
 *	     REFERENCE: HEKTRONICHXT900.pdf
 *
 *			NOTE: REPLACE s4398172 with your student login.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef s4398172_HAL_PANTILT_H
#define s4398172_HAL_PANTILT_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PAN 1
#define TILT 0
/* Access generic write function with type pan */
#define s4398172_hal_pantilt_pan_write(angle) pantilt_angle_write(PAN, angle)
/* Access generic write function with type pan */
#define s4398172_hal_pantilt_tilt_write(angle) pantilt_angle_write(TILT, angle)
/* Access generic read function with type pan */
#define s4398172_hal_pantilt_pan_read() pantilt_angle_read(PAN)
/* Access generic read function with type tilt */
#define s4398172_hal_pantilt_tilt_read() pantilt_angle_read(TILT)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
void s4398172_hal_pantilt_init(void);
int pantilt_angle_read(int type);
void pantilt_angle_write(int type, int angle);

#endif

