/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_coords.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Control plotter coordinates.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * int s4398172_convert_string(char *input, int type);
 * void s4398172_update_xy(char *param, int type);
 * void s4398172_update_z(char *param);
 * void s4398172_update_xy_increment(int difference, int type);
 ******************************************************************************
 */

#ifndef s4398172_HAL_COORDS_H_
#define s4398172_HAL_COORDS_H_

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FALSE 0xFF
#define PACKET_MAX 11
#define DEC 10
#define HEX 16
#define X_COORD 0
#define Y_COORD 1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern char s4398172_depth[2];
extern char s4398172_payload[PACKET_MAX];

/* External function prototypes -----------------------------------------------*/
int s4398172_convert_string(char *input, int type);
void s4398172_update_xy(char *param, int type);
void s4398172_update_z(char *param);
void s4398172_update_xy_increment(int difference, int type);


#endif

