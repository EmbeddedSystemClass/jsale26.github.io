/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_ledbar.c
 * @author  Joshua Salecich � 43981722
 * @date    17022018
 * @brief   LED Light Bar peripheral driver
 *	     REFERENCE: LEDLightBar_datasheet.pdf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_lightbar_init() � intialise LED Light BAR
 * s4398172_lightbar_write() � set LED Light BAR value
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_ledbar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ON 1
#define OFF 0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

void ledbar_seg_set(int segment, unsigned char segment_value) {
	/* Turn segment on (segment_value = 1) or off (segment_value = 0) */
	switch (segment) {
		case 0:
			HAL_GPIO_WritePin(BRD_D16_GPIO_PORT, BRD_D16_PIN, segment_value & 0x01);
			break;
		case 1:
			HAL_GPIO_WritePin(BRD_D17_GPIO_PORT, BRD_D17_PIN, segment_value & 0x01);
			break;
		case 2:
			HAL_GPIO_WritePin(BRD_D18_GPIO_PORT, BRD_D18_PIN, segment_value & 0x01);
			break;
		case 3:
			HAL_GPIO_WritePin(BRD_D13_GPIO_PORT, BRD_D13_PIN, segment_value & 0x01);
			break;
		case 4:
			HAL_GPIO_WritePin(BRD_D20_GPIO_PORT, BRD_D20_PIN, segment_value & 0x01);
			break;
		case 5:
			HAL_GPIO_WritePin(BRD_D21_GPIO_PORT, BRD_D21_PIN, segment_value & 0x01);
			break;
		case 6:
			HAL_GPIO_WritePin(BRD_D22_GPIO_PORT, BRD_D22_PIN, segment_value & 0x01);
			break;
		case 7:
			HAL_GPIO_WritePin(BRD_D23_GPIO_PORT, BRD_D23_PIN, segment_value & 0x01);
			break;
		case 8:
			HAL_GPIO_WritePin(BRD_D24_GPIO_PORT, BRD_D24_PIN, segment_value & 0x01);
			break;
		case 9:
			HAL_GPIO_WritePin(BRD_D25_GPIO_PORT, BRD_D25_PIN, segment_value & 0x01);
			break;
		default:
			debug_printf("cooked\n");
	}
}

/**
  * @brief  Initialise LEDBar.
  * @param  None
  * @retval None
  */
extern void s4398172_ledbar_init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable D0 -> D9 clocks */
	__BRD_D16_GPIO_CLK();
	__BRD_D17_GPIO_CLK();
	__BRD_D18_GPIO_CLK();
	__BRD_D13_GPIO_CLK();
	__BRD_D20_GPIO_CLK();
	__BRD_D21_GPIO_CLK();
	__BRD_D22_GPIO_CLK();
	__BRD_D23_GPIO_CLK();
	__BRD_D24_GPIO_CLK();
	__BRD_D25_GPIO_CLK();

	/* Configure the D16 -> D25 pins as outputs */
	GPIO_InitStructure.Pin = BRD_D16_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D16_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D17_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D17_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D18_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D18_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D13_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D13_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D20_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D20_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D21_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D21_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D22_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D22_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D23_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D23_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D24_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D24_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_D25_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D25_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Set the LED Bar GPIO pins high or low, depending on the bit of �value�
  *         i.e. value bit 0 is 1 � LED Bar 0 on
  *          value bit 1 is 1 � LED BAR 1 on
  *
  * @param  value
  * @retval None
  */
extern void s4398172_ledbar_write(unsigned short value) {

	/* Use bit shifts (<< or >>) and bit masks (1 << bit_index) to determine if a bit is set

	   e.g. The following pseudo code checks if bit 0 of value is 1.
			if ((value & (1 << 0)) == (1 << 0))	{
				Turn on LED BAR Segment 0.
			}
	*/
	for (int i = 0; i < 10; i++) {
		if ((value & (1 << i)) == (1 << i)) {
			ledbar_seg_set(i, ON);
		} else {
			ledbar_seg_set(i, OFF);
		}
	}
}

