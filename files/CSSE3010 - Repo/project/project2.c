/**
  ******************************************************************************
  * @file    Project2/main.c
  * @author  Joshua Salecich - 43981722
  * @date    01/06/2018
  * @brief   Project 2 Demonstration
  *
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>

#include "s4398172_os_cli.h"
#include "s4398172_os_printf.h"
#include "s4398172_os_radio.h"
#include "s4398172_os_pantilt.h"
#include "s4398172_os_ir.h"
#include "s4398172_os_sysmon.h"

#include "s4398172_hal_ir.h"
#include "s4398172_hal_radio.h"

#include "s4398172_cli_radio.h"
#include "s4398172_cli_control.h"
#include "s4398172_cli_draw.h"
#include "s4398172_cli_pantilt.h"
#include "s4398172_cli_sysmon.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef TIM_1_Init; /* Base handler for TIM1. */
TIM_HandleTypeDef TIM_5_Init; /* Base handler for TIM12. */
uint32_t sendIR[2];

/* Private functions ---------------------------------------------------------*/


void us_cntr_init(void) {

	__TIM5_CLK_ENABLE();

	/* Initialise TIM5 as an interrupt */
	TIM_5_Init.Instance = TIM5;
  	TIM_5_Init.Init.Period = 0xFFFFFFFF;	\
  	/* us Precision */
  	TIM_5_Init.Init.Prescaler = (uint32_t)(SystemCoreClock / 1000000) -1;
  	TIM_5_Init.Init.ClockDivision = 0;
	TIM_5_Init.Init.RepetitionCounter = 0;
  	TIM_5_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_5_Init);
	HAL_TIM_Base_Start_IT(&TIM_5_Init);
}



/**
  * @brief  Various peripheral/board initialisation.
  * @param  None
  * @retval None
  */
void init(void) {

	portDISABLE_INTERRUPTS();	//Disable interrupts

	BRD_init();
	BRD_LEDInit();
	us_cntr_init();

	s4398172_os_myprintf_init();
	s4398172_os_cli_init();
	s4398172_os_radio_init();
	s4398172_os_pantilt_init();
	s4398172_os_ir_init();
	s4398172_os_sysmon_init();

	portENTER_CRITICAL();
	s4398172_hal_radio_init();
	portEXIT_CRITICAL();
	s4398172_hal_pantilt_init();
	s4398172_hal_ir_init();

	s4398172_cli_radio_init();
	s4398172_cli_control_init();
	s4398172_cli_draw_init();
	s4398172_cli_pantilt_init();
	s4398172_cli_sysmon_init();

	portENABLE_INTERRUPTS();	//Enable interrupts

}

int main(void) {

	init();

	myprintf("%s", "CSSE3010 Project 2 : Joshua Salecich : s43981722\n\r");

	/* Start the scheduler. */

	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
  	return 0;
}



/**
 * @brief  Sends time and bit value down a queue to the IR Task.
 * @param  Pointer to the base handler for the respective timer.
 * @retval None
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	sendIR[TIME] = TIM_5_Init.Instance->CNT;
	sendIR[BIT] = HAL_GPIO_ReadPin(BRD_D38_GPIO_PORT, BRD_D38_PIN);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE; //default false

	/* We have not woken a task at the start of the ISR. */
	xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendToBackFromISR(s4398172_QueueIR,
    		&sendIR, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Override default mapping of the TIM1_CC_IRQHandler to Default_Handler.
 * @param  None
 * @retval None
 */
void TIM1_CC_IRQHandler(void) {

	HAL_TIM_IRQHandler(&TIM_1_Init);
}

