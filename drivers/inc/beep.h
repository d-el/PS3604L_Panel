/*!****************************************************************************
 * @file		beep.h
 * @author		d_el
 * @version		V1.0
 * @date		19.12.2014
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver beep
 */
#ifndef beep_H
#define beep_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "board.h"

/*!****************************************************************************
 * Define
 */
#define F_BEEP              ( 4000 )
#define BEEP_IRQ_Priority   ( 15 )
#define BEEP_TIM            ( TIM15 )
#define TIME_B_TIM          ( TIM6 )

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */
#define	beepON()            BEEP_TIM->CR1 |= TIM_CR1_CEN
#define	beepOFF()           BEEP_TIM->CR1 &= ~TIM_CR1_CEN
#define	beepSetFreq(freq)   BEEP_TIM->ARR = SYSTEM_FREQUENCY / (freq) / 2;

/*!****************************************************************************
 * Function declaration
 */
void BeepTime(uint16_t time, uint16_t ferq);
void beep_init(void);
void Dht(void *pvParameters);

#endif //beep_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
