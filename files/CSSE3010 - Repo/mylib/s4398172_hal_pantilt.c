/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_pantilt.c
 * @author  Joshua Salecich � 43981722
 * @date    02/03/2018
 * @brief   Functions for initialising the servo peripheral and writing/reading
 * 			to and from the aforementioned respectively.
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4398172_pantilt_init() � Initialise servo (GPIO, PWM, etc)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4398172_hal_pantilt.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Desired clock speed, trials */
#define CLOCK_SPEED			50000
/* Period Value  */
#define PERIOD_VALUE		(40*CLOCK_SPEED/1000)
/* Prescaler value */
#define PRESCALE			(uint32_t)((SystemCoreClock / 2) / CLOCK_SPEED) -1;
/* Independent scale for disregarding floats */
#define SCALE 				100000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_OC_InitTypeDef PWMConfig;
TIM_HandleTypeDef TIM_Init;
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);

/**
  * @brief  Using the HAL Library to alter the pulse width of the configured
  * 		PWM wave by converting an angle to PW.
  * @param  int type: Whether writing to the pan or tilt modes.
  * 		int angle: The angle that needs to be achieved.
  * @retval None
  */
void pantilt_angle_write(int type, int angle) {
	//int pulseWidth = ((491 * -angle / 9) + 8139) * PERIOD_VALUE;
	int pulseWidth = ((500 * -angle / 9) + 7000) * PERIOD_VALUE;
	pulseWidth = pulseWidth / SCALE;
	/* Rounding up for positive angles. It's not required for negative angles
	 * as rounding down is the equivalent.
	 */
	if (angle < 0) {
		pulseWidth += 1;
	}
	if (type == PAN) {
		__HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_3,
						pulseWidth);
	} else if (type == TILT) {
		__HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_4,
						pulseWidth);
	}
}

/**
  * @brief  Using the HAL Library read the value of the configured pulse width
  * 		 and convert to its corresponding angle.
  * @param  int type: Whether reading from the pan or tilt modes
  * @retval int angle: return the corresponding angle.
  */
int pantilt_angle_read(int type) {
	int pulseWidth, angle;
	if (type == PAN) {
		pulseWidth = __HAL_TIM_GET_COMPARE(&TIM_Init,
							TIM_CHANNEL_3);
	} else if (type == TILT) {
		pulseWidth = __HAL_TIM_GET_COMPARE(&TIM_Init,
							TIM_CHANNEL_4);
	}
	//angle = ((((pulseWidth * SCALE) / PERIOD_VALUE) - 8139) * 9 / 491);
	angle = ((((pulseWidth * SCALE) / PERIOD_VALUE) - 7000) * 9 / 500);
	return -angle;
}

/**
  * @brief  Initialise and configure a PWM to be displayed on the servo
  * 		hardware peripheral.
  * @param  none.
  * @retval none.
  */
void s4398172_hal_pantilt_init(void) {

	/* Initialise clocks */
	__TIM4_CLK_ENABLE();
	__BRD_D15_GPIO_CLK();
	__BRD_D14_GPIO_CLK();

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Initialise and configure D6 as a PWM pin*/
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Pin = BRD_D15_PIN;
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM4; //ch1tm1
	HAL_GPIO_Init(BRD_D15_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Pin = BRD_D14_PIN;
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM4; //ch1tm1
	HAL_GPIO_Init(BRD_D14_GPIO_PORT, &GPIO_InitStructure);

	/* Initialise TIM1_CH1 */
	TIM_Init.Instance = TIM4;
	TIM_Init.Init.Prescaler = PRESCALE;
	TIM_Init.Init.Period = PERIOD_VALUE;
	TIM_Init.Init.ClockDivision = 0;
	TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Init.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TIM_Init) != HAL_OK) {
		Error_Handler();
	}

	/* Initialise PWM */
	PWMConfig.OCMode = TIM_OCMODE_PWM1;
	PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
	PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	PWMConfig.Pulse = 7250 * PERIOD_VALUE / SCALE;
	if (HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig,
			TIM_CHANNEL_4) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&TIM_Init,
			TIM_CHANNEL_4) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig,
			TIM_CHANNEL_3) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&TIM_Init,
			TIM_CHANNEL_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief  If called simply flash the blue LED to indicate an error.
  * @param  none.
  * @retval none.
  */
static void Error_Handler(void) {
	/* Turn Blue LED on */
	BRD_LEDBlueOn();

	while (1) {
		BRD_LEDBlueToggle();
	}
}



