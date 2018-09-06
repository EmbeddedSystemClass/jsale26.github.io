/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_radio.h
 * @author  Joshua Salecich � s4398172
 * @date   	23/03/2018
 * @brief 	Radio transceiver peripheral driver.
 *	      REFERENCE: See the nrf9051plus datasheet.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	s4398172_hal_radio_init(void);
 *	s4398172_hal_radio_getpacket(unsigned char *rxpacket);
 *	s4398172_hal_radio_fsmprocessing(int interruptTime);
 *	s4398172_hal_radio_getrxstatus(void);
 *	s4398172_hal_radio_setrxstatus(int status);
 ******************************************************************************
 */

#ifndef s4398172_HAL_RADIO_H
#define s4398172_HAL_RADIO_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFERLEN 5
#define PAYLOAD_LEN 11
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
void s4398172_hal_radio_init(void);
void s4398172_hal_radio_getpacket(unsigned char *rxpacket);
void s4398172_hal_radio_fsmprocessing(void);
int s4398172_hal_radio_getrxstatus(void);
void s4398172_hal_radio_setrxstatus(int status);
void s4398172_set_payload(unsigned char *payload);
void s4398172_hal_radio_setchan(unsigned char chan);
unsigned char s4398172_hal_radio_getchan(void);
void s4398172_hal_radio_gettxaddress(unsigned char *addr);
void s4398172_hal_radio_getrxaddress(unsigned char *addr);
void s4398172_hal_radio_setrxaddress(unsigned char *addr);
void s4398172_hal_radio_settxaddress(unsigned char *addr);

#endif

