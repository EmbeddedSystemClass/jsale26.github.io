/**
 ******************************************************************************
 * @file    mylib/s4398172_ledbar.h
 * @author  Joshua Salecich � 43981722
 * @date    03032015
 * @brief   LED Light Bar peripheral driver
 *	     REFERENCE: LEDLightBar_datasheet.pdf
 *
 *			NOTE: REPLACE s4398172 with your student login.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_ledbar_init() � intialise LED Light BAR
 * s4398172_ledbar_write() � set LED Light BAR value
 ******************************************************************************
 */

#ifndef s4398172_LEDBAR_H
#define s4398172_LEDBAR_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* External function prototypes -----------------------------------------------*/

extern void s4398172_ledbar_init(void);
extern void s4398172_ledbar_write(unsigned short value);
#endif

