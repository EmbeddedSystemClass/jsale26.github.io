/**
 ******************************************************************************
 * @file    mylib/s4398172_cli_draw.h
 * @author  Joshua Salecich � 43981722
 * @date    30/05/2018
 * @brief   Draw (Graphics) CLI commands.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * void s4398172_cli_draw_init(void); //Initialise CLI Draw Commands
 ******************************************************************************
 */

#ifndef s4398172_CLI_DRAW_H_
#define s4398172_CLI_DRAW_H_

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
BaseType_t prvBLineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvSquareCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvLineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvPolyCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvRoseCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString);

extern CLI_Command_Definition_t xLine;
extern CLI_Command_Definition_t xSquare;
extern CLI_Command_Definition_t xBLine;
extern CLI_Command_Definition_t xPoly;
extern CLI_Command_Definition_t xRose;


/* External function prototypes ----------------------------------------------*/

void s4398172_cli_draw_init(void);



#endif

