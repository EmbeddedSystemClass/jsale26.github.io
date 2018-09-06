/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_radio.c
 * @author  Joshua Salecich � 43981722
 * @date    23/03/2018
 * @brief   Radio transceiver peripheral driver. Initialise FSM states.
 *	     REFERENCE: See the nrf9051plus datasheet.
 *
 *******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	s4398172_hal_radio_init(void);
 *	s4398172_hal_radio_getpacket(unsigned char *rxpacket);
 *	s4398172_hal_radio_fsmprocessing(int interruptTime);
 *	s4398172_hal_radio_getrxstatus(void);
 *	s4398172_hal_radio_setrxstatus(int status);
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <s4398172_hal_hamming.h>
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"
#include "s4398172_hal_radio.h"
#include "stdio.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/

#define S4398172_IDLE_STATE 	0x00	/* FSM IDLE MODE */
#define S4398172_TX_STATE 		0x01	/* FSM TRANSMIT MODE */
#define S4398172_RX_STATE 		0x02	/* FSM RECEIVE MODE */
#define S4398172_WAIT_STATE 	0x03	/* FSM WAITING MODE */

/* Private variables ---------------------------------------------------------*/

int s4398172_hal_radio_fsmcurrentstate; /* Current state of the FSM */
int s4398172_hal_radio_rxstatus; /* True if packet received */
uint8_t s4398172_rxBuffer[32]; /* Receive packet buffer */
unsigned char s4398172_txBuffer[32]; /* Transmit packet buffer */
/* Default payload - my name */
unsigned char s4398172_payLoad[22];
/* Comms board addr */
unsigned char s4398172_txAddr[BUFFERLEN] = {0x47, 0x33, 0x22, 0x11, 0x00};
/* My addr */
unsigned char s4398172_rxAddr[BUFFERLEN] = {0x22, 0x17, 0x98, 0x43, 0x00};
uint8_t s4398172_chan = 47; /* Channel set on comms board */
int newLoad = 0, packetSize;


/* Private functions ---------------------------------------------------------*/
void s4398172_hal_radio_setrxaddress(unsigned char *addr);
void s4398172_hal_radio_setchan(unsigned char chan);

/**
  * @brief 	Set the switch statement state.
  * @param  int state: desired state.
  * @retval None
  */
void set_state(int state) {
	s4398172_hal_radio_fsmcurrentstate = state;
}

/**
  * @brief 	Get the switch statement state.
  * @param
  * @retval int current state
  */
int get_state(void) {
	return s4398172_hal_radio_fsmcurrentstate;
}

/**
  * @brief 	Set the payload from external function
  * @param  char *payload: new desired payload
  * @retval None
  */
void s4398172_set_payload(unsigned char *payLoad) {

	uint16_t enPayload;
	packetSize = strlen((char *)payLoad);
	for (int i = 0; i < packetSize; i++) {
		enPayload = s4398172_ham_encode(payLoad[i]);
		s4398172_payLoad[i * 2 + 1] = enPayload;
		s4398172_payLoad[i * 2] = enPayload >> 8;
	}
	packetSize = packetSize * 2 + 10;
	newLoad = 1;
}

/**
  * @brief 	Initialise radio fsm and the transmitted packet
  * @param  None
  * @retval None
  */
void s4398172_hal_radio_init(void) {
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	/* Initialise radio FSM */
	radio_fsm_init();

	s4398172_txBuffer[0] = 0xA1;
	for (int i = 1; i < 5; i++) {
		s4398172_txBuffer[i] = s4398172_txAddr[i - 1];
	}
	for (int i = 5; i < 9; i++) {
		s4398172_txBuffer[i] = s4398172_rxAddr[i - 5];
	}
	s4398172_txBuffer[9] = 0x00;

	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	set_state(S4398172_IDLE_STATE);

	s4398172_hal_radio_setrxaddress(s4398172_rxAddr);
}

/**
  * @brief 	Set channel
  * @param  chan: desired channel
  * @retval None
  */
void s4398172_hal_radio_setchan(unsigned char chan) {
	uint8_t currentChannel = (uint8_t)(chan);

	s4398172_chan = currentChannel;

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_register_write(NRF24L01P_RF_CH, &currentChannel);
}

/**
  * @brief 	Return current channel.
  * @param  None
  * @retval currentChannel
  */
unsigned char s4398172_hal_radio_getchan(void) {

	unsigned char currentChannel;

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_register_read(NRF24L01P_RF_CH, &currentChannel);

	return currentChannel;
}

/**
  * @brief 	Set transmit address
  * @param  addr: txAddr
  * @retval None
  */
void s4398172_hal_radio_settxaddress(unsigned char *addr) {

	for (int i = 0; i < BUFFERLEN; i++) {
		s4398172_txAddr[i] = addr[i];
	}

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_buffer_write(NRF24L01P_TX_ADDR, addr, BUFFERLEN);
}

/**
  * @brief 	Set receive address
  * @param  addr: rxAddr
  * @retval None
  */
void s4398172_hal_radio_setrxaddress(unsigned char *addr) {

	for (int i = 0; i < BUFFERLEN; i++) {
		s4398172_rxAddr[i] = addr[i];
	}

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_buffer_write(NRF24L01P_RX_ADDR_P0, addr, BUFFERLEN);
}

/**
  * @brief 	Get transmit address
  * @param  addr: buffer to hold tx address
  * @retval None
  */
void s4398172_hal_radio_gettxaddress(unsigned char *addr) {

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_buffer_read(NRF24L01P_TX_ADDR, addr, BUFFERLEN);
}

/**
  * @brief 	Get receive address
  * @param  addr: buffer to hold rx address
  * @retval None
  */
void s4398172_hal_radio_getrxaddress(unsigned char *addr) {

	set_state(S4398172_IDLE_STATE);
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	radio_fsm_buffer_read(NRF24L01P_RX_ADDR_P0, addr, BUFFERLEN);
}

/**
  * @brief 	Set channel, txaddr for TX mode. Write txpacket.
  * @param  chan, addr, txpacket
  * @retval None
  */
void s4398172_hal_radio_sendpacket(unsigned char chan, unsigned char *addr,
		unsigned char *txpacket) {

	s4398172_hal_radio_settxaddress(addr);
	s4398172_hal_radio_setchan(chan);

	for (int i = 1; i < 5; i++) {
		s4398172_txBuffer[i] = s4398172_txAddr[i - 1];
	}
	for (int i = 5; i < 9; i++) {
		s4398172_txBuffer[i] = s4398172_rxAddr[i - 5];
	}

	for (int i = 10; i < packetSize; i++) {
		txpacket[i] = s4398172_payLoad[i - 10];
	}

	for (int i = packetSize; i < 32; i++) {
		txpacket[i] = 0x00;
	}

	set_state(S4398172_TX_STATE);
}

/**
  * @brief 	Set the received packet to the buffer packet
  * @param  rxpacket: desired rxBuffer storage place
  * @retval
  */
void s4398172_hal_radio_getpacket(unsigned char *rxpacket) {
	for (int i = 0; i < 32; i++) {
		rxpacket[i] = s4398172_rxBuffer[i];
	}
}

/**
  * @brief 	Set state to receive
  * @param
  * @retval
  */
void s4398172_hal_radio_setfsmrx(void) {
	radio_fsm_setstate(RADIO_FSM_RX_STATE);
}

/**
  * @brief 	Get whether the packet is full or not
  * @param
  * @retval int rxstatus
  */
int s4398172_hal_radio_getrxstatus(void) {
	return s4398172_hal_radio_rxstatus;
}

/**
  * @brief 	Set whether the packet is full or not
  * @param  int status
  * @retval
  */
void s4398172_hal_radio_setrxstatus(int status) {
	s4398172_hal_radio_rxstatus = status;
}

/**
  * @brief 	Switch statement to control program flow of the implemented FSM
  * 		REFER: Workbook.
  * @param  int interruptTime: current time
  * @retval
  */
void s4398172_hal_radio_fsmprocessing(void) {
	switch(get_state()) {
		case S4398172_IDLE_STATE:
			if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {

				if (newLoad) {
					s4398172_hal_radio_sendpacket(s4398172_chan,
							s4398172_txAddr, s4398172_txBuffer);
					set_state(S4398172_TX_STATE);
				} else {
					set_state(S4398172_RX_STATE);
				}
			} else {
				// if error occurs set state back to IDLE state
				debug_printf("ERROR: Radio FSM not in IDLE state\r\n");
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		case S4398172_TX_STATE:	//TX state for writing packet to be sent.
			if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {
				if (radio_fsm_setstate(RADIO_FSM_TX_STATE) ==
						RADIO_FSM_INVALIDSTATE) {
					debug_printf("ERROR: Cannot set Radio FSM RX state\n\r");
				} else { /* send the packet, set to idle */
					radio_fsm_write(s4398172_txBuffer);
					newLoad = 0;
					set_state(S4398172_IDLE_STATE);
				}
			} else {
					/* if error occurs, set state back to IDLE state */
					debug_printf("ERROR: Radio FSM not in Idle state\n\r");
					radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		case S4398172_RX_STATE:
			if ((radio_fsm_getstate() == RADIO_FSM_IDLE_STATE)
					|| (radio_fsm_getstate() == RADIO_FSM_WAIT_STATE)) {

				if (radio_fsm_setstate(RADIO_FSM_RX_STATE)
						== RADIO_FSM_INVALIDSTATE) {
					debug_printf(
							"ERROR: Cannot set radio FSM to RX state\r\n");
				} else { /* Straight to wait state */
					set_state(S4398172_WAIT_STATE);
				}
			} else {
				// if error occurs set state back to IDLE state
				debug_printf("ERROR: Radio FSM no in IDLE state\r\n");
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		case S4398172_WAIT_STATE:
			if (radio_fsm_getstate() == RADIO_FSM_WAIT_STATE) {
				if (radio_fsm_read(s4398172_rxBuffer) == RADIO_FSM_DONE) {
					/* If read successful */
					s4398172_hal_radio_setrxstatus(1);
				}
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
				// Set radio FSM back to IDLE state
				set_state(S4398172_IDLE_STATE);
			}
			break;
	}
}



