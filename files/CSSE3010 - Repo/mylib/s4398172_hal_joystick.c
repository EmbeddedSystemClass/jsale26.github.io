/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_joystick.c
 * @author  Joshua Salecich � 43981722
 * @date    10/03/2018
 * @brief   Joystick peripheral driver
 *	     REFERENCE: joystick_datasheet.pdf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_joystick_init() � intialise joystick
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_joystick.h"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef AdcHandleX;
ADC_HandleTypeDef AdcHandleY;
ADC_ChannelConfTypeDef AdcChanConfigX;
ADC_ChannelConfTypeDef AdcChanConfigY;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Read the joystick value via analogue to digital conversion.
 * @param  int type: whether converting the x or y joystick.
 * @retval int adcValue: the digital converted value.
 */
int joystick_read(int type) {
	uint16_t adcValue;
	if (type == X_MODE) {
		HAL_ADC_Start(&AdcHandleX);
		while (HAL_ADC_PollForConversion(&AdcHandleX, 10) != HAL_OK);
		adcValue = (uint16_t)(HAL_ADC_GetValue(&AdcHandleX));
	} else if (type == Y_MODE) {
		HAL_ADC_Start(&AdcHandleY);
		while (HAL_ADC_PollForConversion(&AdcHandleY, 10) != HAL_OK);
		adcValue = (uint16_t)(HAL_ADC_GetValue(&AdcHandleY));
	}
	return adcValue;
}

/**
 * @brief  Initialise the pin, clock and adc to convert the joystick analogue
 * 		   signal to digital.
 * @param  None
 * @retval None
 */
void x_joystick_init(GPIO_InitTypeDef GPIO_InitStructure) {

	/* Enable Pin Clock */
	__BRD_A0_GPIO_CLK();

	/* Configure push button input for rising edge interrupt*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = BRD_A0_PIN;
	HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

	/* Set interrupt priority and enable the interrupt handler */
	HAL_NVIC_SetPriority(BRD_A0_EXTI_IRQ, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_A0_EXTI_IRQ);

	GPIO_InitStructure.Pin = BRD_A0_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

	__ADC1_CLK_ENABLE();

	/* Init ADC1 */
	AdcHandleX.Instance = (ADC_TypeDef *)(ADC1_BASE);
	AdcHandleX.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandleX.Init.Resolution            = ADC_RESOLUTION12b;
	AdcHandleX.Init.ScanConvMode          = DISABLE;
	AdcHandleX.Init.ContinuousConvMode    = DISABLE;
	AdcHandleX.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleX.Init.NbrOfDiscConversion   = 0;
	AdcHandleX.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandleX.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandleX.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandleX.Init.NbrOfConversion       = 1;
	AdcHandleX.Init.DMAContinuousRequests = DISABLE;
	AdcHandleX.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleX);		//Initialise ADC

	/* Configure ADC Channel */
	AdcChanConfigX.Channel = BRD_A0_ADC_CHAN;
	AdcChanConfigX.Rank         = 1;
	AdcChanConfigX.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfigX.Offset       = 0;

	HAL_ADC_ConfigChannel(&AdcHandleX, &AdcChanConfigX);
}

void y_joystick_init(GPIO_InitTypeDef GPIO_InitStructure) {
	/* Enable Pin Clock */
	__BRD_A1_GPIO_CLK();

	/* Configure push button input for rising edge interrupt*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = BRD_A1_PIN;
	HAL_GPIO_Init(BRD_A1_GPIO_PORT, &GPIO_InitStructure);

	/* Set interrupt priority and enable the interrupt handler */
	HAL_NVIC_SetPriority(BRD_A1_EXTI_IRQ, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_A1_EXTI_IRQ);

	GPIO_InitStructure.Pin = BRD_A1_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(BRD_A1_GPIO_PORT, &GPIO_InitStructure);

	__ADC2_CLK_ENABLE();

	/* Init ADC1 */
	AdcHandleY.Instance = (ADC_TypeDef *)(ADC2_BASE);
	AdcHandleY.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandleY.Init.Resolution            = ADC_RESOLUTION12b;
	AdcHandleY.Init.ScanConvMode          = DISABLE;
	AdcHandleY.Init.ContinuousConvMode    = DISABLE;
	AdcHandleY.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleY.Init.NbrOfDiscConversion   = 0;
	AdcHandleY.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandleY.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandleY.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandleY.Init.NbrOfConversion       = 1;
	AdcHandleY.Init.DMAContinuousRequests = DISABLE;
	AdcHandleY.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleY);		//Initialise ADC

	/* Configure ADC Channel */
	AdcChanConfigY.Channel = BRD_A1_ADC_CHAN;
	AdcChanConfigY.Rank         = 1;
	AdcChanConfigY.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfigY.Offset       = 0;

	HAL_ADC_ConfigChannel(&AdcHandleY, &AdcChanConfigY);
}

void z_joystick_init(GPIO_InitTypeDef GPIO_InitStructure) {

	/* Enable Pin Clock */
	__BRD_A2_GPIO_CLK();

	/* Configure push button input for rising edge interrupt*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = BRD_A2_PIN;
	HAL_GPIO_Init(BRD_A2_GPIO_PORT, &GPIO_InitStructure);

	/* Set interrupt priority and enable the interrupt handler */
	HAL_NVIC_SetPriority(BRD_A2_EXTI_IRQ, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_A2_EXTI_IRQ);
}

void s4398172_joystick_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	x_joystick_init(GPIO_InitStructure);
	y_joystick_init(GPIO_InitStructure);
	z_joystick_init(GPIO_InitStructure);
}
