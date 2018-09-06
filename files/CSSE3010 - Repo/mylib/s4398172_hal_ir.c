/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_ir.c
 * @author  Joshua Salecich � 43981722
 * @date    10/03/2018
 * @brief   IR transmitter and receiver peripheral driver
 *	     REFERENCE: IR_datasheet.pdf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_ir_init() � intialise carrier, tx and sample functions
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_ir.h"

/* Private define ------------------------------------------------------------*/
/* Prescaler value for all clocks. */
#define PRESCALER 		(uint32_t)(SystemCoreClock / CLOCK_SPEED) -1
/* Predefined speed for all clocks to operate. */
#define CLOCK_SPEED		700000
/* 38kHz PWM carrier wave */
#define PWM_PERIOD		CLOCK_SPEED / 38000

/* Private variables ---------------------------------------------------------*/
/* Base handler for TIM1. */
TIM_HandleTypeDef TIM_1_Init;
/* Base handler for TIM2. */
TIM_HandleTypeDef TIM_2_Init;
/* Base handler for TIM3. */
TIM_HandleTypeDef TIM_3_Init;
/* Base handler for TIM4. */
TIM_HandleTypeDef TIM_4_Init;
/* PWM configuration structure. */
TIM_OC_InitTypeDef PWMConfig;
/* Input Capture configuration structure. */
TIM_IC_InitTypeDef sICConfig;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Simple error handler. If called toggled the blue LED.
 * @param  None
 * @retval None
 */
void error_handler(void) {
	/* Turn Blue LED on */
	BRD_LEDBlueOn();

	while (1) {
		BRD_LEDBlueToggle();
	}
}

/**
 * @brief  Turn the carrier both on or off. Done so by effecting the duty cycle.
 * @param  None
 * @retval None
 */
void irhal_carrier(int state) {
	if (state) {
		__HAL_TIM_SET_COMPARE(&TIM_2_Init, TIM_CHANNEL_1,
							PWM_PERIOD * 50/100);
	} else {
		__HAL_TIM_SET_COMPARE(&TIM_2_Init, TIM_CHANNEL_1,
									PWM_PERIOD * 0/100);
	}
}

/**
 * @brief  Initialise and configure the pin, clock and PWM for the carrier wave.
 * @param  GPIO_InitTypeDef GPIO_InitStructure: GPIO init structure definition.
 * @retval None
 */
void init_carrier(GPIO_InitTypeDef GPIO_InitStructure) {
	/* Initialise 38kHz PWM  (carrier) output pin */
	GPIO_InitStructure.Pin = BRD_D35_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(BRD_D35_GPIO_PORT, &GPIO_InitStructure);

	/* Initialise TIM3 as a PWM wave */
	TIM_2_Init.Instance = TIM2;
	TIM_2_Init.Init.Prescaler = PRESCALER;
	TIM_2_Init.Init.Period = PWM_PERIOD;
	TIM_2_Init.Init.ClockDivision = 0;
	TIM_2_Init.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_2_Init.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TIM_2_Init) != HAL_OK) {
		error_handler();
	}

	/* Configure the PWM wave */
	PWMConfig.OCMode = TIM_OCMODE_PWM1;
	PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
	PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	PWMConfig.Pulse = PWM_PERIOD * 50/100;
	if (HAL_TIM_PWM_ConfigChannel(&TIM_2_Init, &PWMConfig,
			TIM_CHANNEL_4) != HAL_OK) {
		error_handler();
	}
	if (HAL_TIM_PWM_Start(&TIM_2_Init,
			TIM_CHANNEL_4) != HAL_OK) {
		error_handler();
	}
}

/**
 * @brief  Initialise and configure the pin, clock for the interrupt.
 * @param  GPIO_InitTypeDef GPIO_InitStructure: GPIO init structure definition.
 * @retval None
 */
void init_transmitter(GPIO_InitTypeDef GPIO_InitStructure) {
	/* Initialise 0-50Hz output pin */
	GPIO_InitStructure.Pin = BRD_D37_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
 	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BRD_D37_GPIO_PORT, &GPIO_InitStructure);

	/* Initialise TIM3 as an interrupt */
	TIM_3_Init.Instance = TIM3;				//Enable Timer 2
  	TIM_3_Init.Init.Period = CLOCK_SPEED / 1000;	//1ms period
  	TIM_3_Init.Init.Prescaler = PRESCALER;
  	TIM_3_Init.Init.ClockDivision = 0;
	TIM_3_Init.Init.RepetitionCounter = 0;
  	TIM_3_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_3_Init);

	/* Set priority of Timer 3 update Interrupt
	 * [0 (HIGH priority) to 15(LOW priority)] */

	/* 	Set Main priority to 10 and sub-priority to 0. */
	HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);

	/* Enable the Timer 3 interrupt */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	/* Start Timer 3 base unit in interrupt mode */
	HAL_TIM_Base_Start_IT(&TIM_3_Init);
}

/**
 * @brief  Initialise and configure the pin and clock for the input capture mode
 * 		   for sampling.
 * @param  GPIO_InitTypeDef GPIO_InitStructure: GPIO init structure definition.
 * @retval None
 */
void init_receiver(GPIO_InitTypeDef GPIO_InitStructure) {

	// Configure the D38 pin with TIM1 alternate function
	GPIO_InitStructure.Pin = BRD_D38_PIN;					// Pin
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;  // No pull-(up/down) resistor enabled
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM1;
	// See Table 12 STM32F427xx and STM32F429xx alternate function mapping and
	// also see HAL GPIO Firmware defines Section 29.3 in UM1725

	HAL_GPIO_Init(BRD_D38_GPIO_PORT, &GPIO_InitStructure);

	/* TIM Base configuration */
	TIM_1_Init.Instance = TIM1;						//Enable Timer 1
	TIM_1_Init.Init.Period = 0xFFFF;		//Minimise update events, set to max.
	TIM_1_Init.Init.Prescaler = PRESCALER;		//Set prescaler value
	TIM_1_Init.Init.ClockDivision = 0;				//Set clock division
	TIM_1_Init.Init.RepetitionCounter = 0;			// Set reload Value
	TIM_1_Init.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.

	// Configure the Input Capture channel
	// We are using Channel 4 (TIM1_CH4)
	sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;//RISING;
	sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sICConfig.ICFilter = 0;

	// Configure the NVIC for TIM1 capture and compare
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0x0F, 0x00);

	// Enable the TIM1 CC global interrupt
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn); //hAL?

	if (HAL_TIM_IC_Init(&TIM_1_Init) != HAL_OK) {
		/* Initialisation Error */
		error_handler();
	}

	if (HAL_TIM_IC_ConfigChannel(&TIM_1_Init, &sICConfig, TIM_CHANNEL_4)
			!= HAL_OK) {
		// Config error
		error_handler();
	}
	// Start the Input Capture in interrupt mode
	if (HAL_TIM_IC_Start_IT(&TIM_1_Init, TIM_CHANNEL_4) != HAL_OK) {
		// Start error
		error_handler();
	}
}

/**
 * @brief  Call all functions for initialising: carrier, transmitter \
 * 		   and receiver.
 * @param  None
 * @retval None
 */
void s4398172_hal_ir_init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	__TIM1_CLK_ENABLE();
	__TIM2_CLK_ENABLE();
	__TIM3_CLK_ENABLE();

	__BRD_D35_GPIO_CLK();
	__BRD_D37_GPIO_CLK();
	__BRD_D38_GPIO_CLK();

	//init_carrier(GPIO_InitStructure);
	//init_transmitter(GPIO_InitStructure);
	init_receiver(GPIO_InitStructure);
}


