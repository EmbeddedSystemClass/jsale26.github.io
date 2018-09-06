/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_sysmon.h
 * @author  Joshua Salecich � 43981722
 * @date    01/05/2018
 * @brief   LA System monitor driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_hal_sysmon_write(int pin, int value) � Write LA pin
 * s4398172_hal_sysmon_init(void) � Initialise LA pins
 ******************************************************************************
 */

#ifndef s4398172_HAL_SYSMON_H_
#define s4398172_HAL_SYSMON_H_

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CHAN0 10
#define CHAN1 11
#define CHAN2 12

#define ON 1
#define OFF 0

#define s4398172_hal_sysmon_chan0_set() s4398172_hal_sysmon_write(CHAN0, ON)
#define s4398172_hal_sysmon_chan0_clr() s4398172_hal_sysmon_write(CHAN0, OFF)

#define s4398172_hal_sysmon_chan1_set() s4398172_hal_sysmon_write(CHAN1, ON)
#define s4398172_hal_sysmon_chan1_clr() s4398172_hal_sysmon_write(CHAN1, OFF)

#define s4398172_hal_sysmon_chan2_set() s4398172_hal_sysmon_write(CHAN2, ON)
#define s4398172_hal_sysmon_chan2_clr() s4398172_hal_sysmon_write(CHAN2, OFF)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* External function prototypes -----------------------------------------------*/

void s4398172_hal_sysmon_write(int pin, int value);
void s4398172_hal_sysmon_init(void);

#endif

