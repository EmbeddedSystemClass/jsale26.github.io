/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_ircomms.h
 * @author  Joshua Salecich � s4398172
 * @date   	20/04/2018
 * @brief 	Encode/Decode using manchester coding.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	uint16_t s4398172_man_encode(uint8_t data)
 * 	uint8_t s4398172_man_decode(uint16_t data)
 ******************************************************************************
 */

#ifndef s4398172_HAL_IRCOMMS_H
#define s4398172_HAL_IRCOMMS_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
uint16_t s4398172_man_encode(uint8_t data);
uint8_t s4398172_man_decode(uint16_t data);

#endif

