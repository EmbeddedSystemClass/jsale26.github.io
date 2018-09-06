/**
 ******************************************************************************
 * @file    mylib/s4398172_hal_ir.h
 * @author  Joshua Salecich � 43981722
 * @date   	10/03/2018
 * @brief 	IR tx and rx driver.
 *	     REFERENCE: ir_datasheet.pdf
 *
 *			NOTE: REPLACE s4398172 with your student login.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * 	s4398172_hal_ir_carrieron()
 * 	s4398172_hal_ir_carrieroff()
 * 	s4398172_hal_ir_datamodulation_set()
 * 	s4398172_hal_ir_datamodulation_clr()
 ******************************************************************************
 */

#ifndef s4398172_HAL_IR_H
#define s4398172_HAL_IR_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Access generic carrier function and turn on */
#define s4398172_hal_ir_carrieron() irhal_carrier(1)
/* Access generic carrier function and turn on */
#define s4398172_hal_ir_carrieroff() irhal_carrier(0)
/* Set the datamodulation to on by writing true to pin */
#define s4398172_hal_ir_datamodulation_set() HAL_GPIO_WritePin(BRD_D37_GPIO_PORT, BRD_D37_PIN, 1);
/* Set the datamodulation to off by writing false to pin */
#define s4398172_hal_ir_datamodulation_clr() HAL_GPIO_WritePin(BRD_D37_GPIO_PORT, BRD_D37_PIN, 0);
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* External function prototypes -----------------------------------------------*/
extern void s4398172_hal_ir_init(void);
extern void error_handler(void);

#endif

