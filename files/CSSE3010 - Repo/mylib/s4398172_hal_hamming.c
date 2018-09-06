/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_hamming.c
 * @author  Joshua Salecich � 43981722
 * @date    20/04/2018
 * @brief   Encode/decode using hamming (7, 4) coding.
 *
 *******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	uint16_t s4398172_hal_ham_encode(uint8_t data)
 * 	uint8_t s4398172_hal_ham_decode(uint16_t data)
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"
#include "s4398172_hal_radio.h"
#include "math.h"

/* Private define ------------------------------------------------------------*/
#define MSB_HIGH_BIT 15 /* Parity bit pass two */
#define MSB_LOW_BIT  7	/* Parity bit pass one */

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 * @brief Add a parity bit to the data.
 * @param  uint16_t data: encoded data without parity
 * int startBit: start of checking
 * @retval uint16_t data: encoded data with parity
 */
uint16_t parity_gen(uint16_t data, int startBit) {
	int oneCount = 0;
	/* Parity calculator */
	for (int i = (startBit - 1); i >= (startBit - 7); i--) {
		if ((data >> i) & 1) {
			oneCount++;
		}
	}
	/* If ones is odd add 1, even add 0 */
	if (oneCount % 2) {
		data |= 1 << (startBit);
	} else {
		data &= ~(1 << (startBit));
	}
	return data;
}

/*
 * @brief Complete the calculations required to encode data with
 * hamming (7, 4).
 * @param  uint8_t data: decoded data to be encoded
 * @retval	uint16_t encodedData: encoded data
 */
uint16_t s4398172_ham_encode(uint8_t data) {
	uint16_t encodedData = 0;
	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	for (int i = 0; i < 2; i++) {
		/* extract bits */
		data = data >> i * 4;
		d0 = !!(data & 0x1);
		d1 = !!(data & 0x2);
		d2 = !!(data & 0x4);
		d3 = !!(data & 0x8);

		/* calculate hamming parity bits */

		h0 = d0 ^ d1 ^ d2;
		h1 = d0 ^ d2 ^ d3;
		h2 = d0 ^ d1 ^ d3;

		/* generate out byte without parity bit P0 */
		out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
			(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

		/* calculate even parity bit */
		for (z = 1; z<8; z++)
			p0 = p0 ^ !!(out & (1 << z));

		out |= p0;
		if (i == 0) {
			for (int i = 0; i < 8; i++) {
				if ((out >> i) & 1) {
					encodedData |= 1 << i;
				} else {
					encodedData &= ~(1 << i);
				}
			}
		} else {
			for (int i = 8; i < 16; i++) {
				if ((out >> (i - 8)) & 1) {
					encodedData |= 1 << i;
				} else {
					encodedData &= ~(1 << i);
				}
			}
		}
	}
	return encodedData;
}

/*
 * @brief Complete the calculations required to decode data with
 * hamming (7, 4).
 * @param  uint16_t data: encoded data to be decoded
 * @retval	uint8_t decodedData: decoded data
 */
uint8_t s4398172_ham_decode(uint16_t data, int printStatus) {
	uint16_t fullData;
	uint8_t decodedData, empty = 0, H[3], errMask[4];
	int d[8], S[3], errorBit, parityCheck;
	int twoBitError = 0, msBits;

	H[0] = 0b1001110;
	H[1] = 0b0101011;
	H[2] = 0b0011101;

	fullData = data;

	for (int j = 0; j < 4; j++) {
		errMask[j] = 0;
	}

	/* Calculate Syndrome: S = H * y^T */
	/* Don't forget the backward format */

	for (int i = 0; i < 2; i++) {
		data =  data >> (i * 8);
		parityCheck = 0;

		/* Reset syndrome to zero (no errors, by default) too */
		for (int j = 0; j < 3; j++) {
			S[j] = 0;
		}

		/* Fill hamming and data bits */
		for (int j = 0; j < 8; j++) {
			empty |= 1 << j;
			d[7 - j] = !!(data & empty);
			empty = 0;
		}

		/* Check for a single error - find Syndrome */
		for (int j = 0; j < 3; j++) {
			for (int k = 6; k >= 0; k--) {
				S[j] = S[j] + (((H[j] >> k) & 1) * d[k]);
			}
			S[j] = S[j] % 2;
		}

		/* Match to H matrix */
		if ((S[0] == 0) && (S[1] == 0) && (S[2] == 0)) {
			errorBit = -1; //no error
		} else {
			for (int j = 0; j < 7; j++) {
				if (S[0] == ((H[0] >> j) & 1)) {
					if (S[1] == ((H[1] >> j) & 1)) {
						if (S[2] == ((H[2] >> j) & 1)) {
							errorBit = j;
						}
					}
				}
			}
		}

		/* Correct the first error */
		if (errorBit >= 0) {
			if (d[errorBit]) {
				d[errorBit] = 0;
			} else {
				d[errorBit] = 1;
			}
			fullData ^= 1 << (7 - errorBit + 8 * i);

			/* Lay this out with errMask */
			if (errorBit > 3) {
				errorBit = errorBit - 4;
			} else {
				msBits = 1;
			}
			errMask[msBits + 2 * i] |= 1 << (3 - errorBit);
		}

		/* Check if there's a second error */
		for (int j = 0; j < 8; j++) {
			if (d[j]) {
				parityCheck++;
			}
		}

		parityCheck = parityCheck % 2;

		if (parityCheck) {
			if (errorBit != -1) { //if already an error
				if (printStatus == 0) {
					return -1;
				}
				twoBitError = 1;
				break;
			} else {
				/* Flip the parity bit */
				if (d[7]) {
					d[7] = 0;
				} else {
					d[7] = 1;
				}
				fullData ^= 1 << (8 * i);
				errMask[2 * i] = 1;
			}
		}

		/* Store the data */
		for (int j = 3; j >= 0; j--) {
			decodedData ^= (-d[j] ^ decodedData) & (1 << ((3 - j) + (4 * i)));
		}
	}

	if (printStatus) {
		if (twoBitError) {
			debug_printf("2-bit ERROR\n\r");
		} else {
			debug_printf("%X (Full:%X ErrMask:", decodedData, fullData);
			for (int i = 3; i >= 0; i--) {
				debug_printf("%d", errMask[i]);
			}
			debug_printf(")\n\r");
		}
	}
	return decodedData;
}
