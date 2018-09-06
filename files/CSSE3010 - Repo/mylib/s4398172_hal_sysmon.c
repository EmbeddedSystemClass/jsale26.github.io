/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_sysmon.c
 * @author  Joshua Salecich � 43981722
 * @date    28/04/2018
 * @brief   Initialise Logic Analyser Pins and toggling functionality.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_hal_sysmon_init() � intialise Logic Analyser Pins
 * s4398172_hal_sysmon_write(int val, int pin) �
 * set the LA pin high (val = 1) or low (val = 0) on respective pin
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_sysmon.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialise Logic Analyser Pins
  * @param  None
  * @retval None
  */
void s4398172_hal_sysmon_init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable D10 -> D12 clocks */
	__BRD_D10_GPIO_CLK();
	__BRD_D11_GPIO_CLK();
	__BRD_D12_GPIO_CLK();

	/* Configure the D10 -> D12 pins as outputs */
	GPIO_InitStructure.Pin = BRD_D10_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D10_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D11_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D11_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D12_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D12_GPIO_PORT, &GPIO_InitStructure);

}

/**
  * @brief  Set the respective pin based off given value.
  * @param  value: high or low
  * @param	pin: respective pin
  * @retval None
  */
void s4398172_hal_sysmon_write(int pin, int value) {
	switch (pin) {
		case 10:
			HAL_GPIO_WritePin(BRD_D10_GPIO_PORT, BRD_D10_PIN, value & 0x01);
			break;

		case 11:
			HAL_GPIO_WritePin(BRD_D11_GPIO_PORT, BRD_D11_PIN, value & 0x01);
			break;

		case 12:
			HAL_GPIO_WritePin(BRD_D12_GPIO_PORT, BRD_D12_PIN, value & 0x01);
			break;
	}
}


