/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_control.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   (Plotter) Control CLI commands.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *	 void s4398172_cli_control_init(void); //Initialise CLI Control commands
 ******************************************************************************
 */

#ifndef s4398172_CLI_CONTROL_H_
#define s4398172_CLI_CONTROL_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BaseType_t prvMovePenCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvJoinCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvPenDepthCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvOriginCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);

extern CLI_Command_Definition_t xMove;
extern CLI_Command_Definition_t xPen;
extern CLI_Command_Definition_t xPenDepth;
extern CLI_Command_Definition_t xJoin;
extern CLI_Command_Definition_t xOrigin;

/* External function prototypes ----------------------------------------------*/

void s4398172_cli_control_init(void);



#endif

