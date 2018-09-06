/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_proj1_helper.h
 * @author  Joshua Salecich � s4398172
 * @date   	20/04/2018
 * @brief 	Project 1 helper functions
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	void s4398172_encoder(char *command, int type)
 * 	void s4398172_decoder(char *command, int type)
 * 	void s4398172_move_servo(int type, int dir, int angle)
 * 	int s4398172_get_command(int *index, char *command)
 * 	void s4398172_amble_gen(int packetNum, uint16_t *data, uint32_t *packet)
 ******************************************************************************
 */

#ifndef s4398172_PROJ1_HELPER_H
#define s4398172_PROJ1_HELPER_H

/* Private define ------------------------------------------------------------*/

#define HAMMING 0 		/* Hamming mode */
#define MANCHESTER 1	/* Manchester mode */
#define POS 1			/* Move in positive direction */
#define NEG 0			/* Move in negative direction */
#define PAN 1			/* Pan mode */
#define TILT 0			/* Tilt mode */

/* External function prototypes ----------------------------------------------*/

void s4398172_encoder(char *command, int type);
void s4398172_decoder(char *command, int type);
void s4398172_move_servo(int type, int dir, int angle);
int s4398172_get_command(int *index, char *command);
void s4398172_amble_gen(int packetNum, uint16_t *data, uint32_t *packet);

#endif

