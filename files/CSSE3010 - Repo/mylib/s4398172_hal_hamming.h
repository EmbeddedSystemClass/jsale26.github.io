/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_hamming.h
 * @author  Joshua Salecich � s4398172
 * @date   	20/04/2018
 * @brief 	Encode/decode using hamming (7, 4) coding.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	uint16_t s4398172_hal_ham_encode(uint8_t data)
 * 	uint8_t s4398172_hal_ham_decode(uint16_t data)
 ******************************************************************************
 */

#ifndef s4398172_HAL_HAMMING_H
#define s4398172_HAL_HAMMING_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
uint16_t s4398172_ham_encode(uint8_t data);
uint8_t s4398172_ham_decode(uint16_t data, int printStatus);

#endif

