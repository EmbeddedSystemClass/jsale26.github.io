/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_ircomms.c
 * @author  Joshua Salecich � 43981722
 * @date    20/04/2018
 * @brief   Encode/Decode using manchester coding.
 *
 *******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	uint16_t s4398172_man_encode(uint8_t data)
 * 	uint8_t s4398172_man_decode(uint16_t data)
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "math.h"
#include "s4398172_hal_ircomms.h"

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 * @brief Takes 8 bits and converts to 16 bits with manchester coding.
 * 0 -> 10, 1 -> 01.
 * @param  uint8_t data: decoded data
 * @retval uint16_t encodedData: encoded data
 */
uint16_t s4398172_man_encode(uint8_t data) {
	uint16_t encodedData;
	for (int i = 0; i < 8; i++) {
		if ((data >> i) & 1) {
			/* 1 -> 01 */
			encodedData &= ~(1 << (i * 2 + 1));
			encodedData |= 1 << (i * 2);
		} else {
			/* 0 -> 10 */
			encodedData |= 1 << (i * 2 + 1);
			encodedData &= ~(1 << (i * 2));
		}
	}
	return encodedData;
}

/*
 * @brief Takes 8 bits and converts to 16 bits with manchester coding.
 * 0 -> 10, 1 -> 01.
 * @param  uint16_t data: encoded data
 * @retval uint8_t decodedData: decoded data
 */
uint8_t s4398172_man_decode(uint16_t data) {
	uint8_t decodedData;
	for (int i = 0; i < 16; i++) {
		if ((i % 2) == 0) {
			if ((data >> i) & 1) {
				if ((data >> (i + 1)) & 1) {
					debug_printf("Manchester %X\n\r", data);
				} else {
					decodedData |= 1 << (i / 2);
				}
			} else {
				if ((data >> (i + 1)) & 1) {
					decodedData &= ~(1 << (i / 2));
				} else {
					debug_printf("Manchester %X\n\r", data);
				}
			}
		}
	}
	return decodedData;
}

