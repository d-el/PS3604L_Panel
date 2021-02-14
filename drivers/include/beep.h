	/*!****************************************************************************
 * @file		beep.h
 * @author		d_el
 * @version		V1.0
 * @date		19.12.2014
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver beep
 */
#ifndef beep_H
#define beep_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */
#define F_BEEP				( 4000 )
#define BEEP_IRQ_Priority	( 15 )
#define BEEP_TIM			( TIM12 )
#define TIME_B_TIM			( TIM7 )

/*!****************************************************************************
 * Macro functions
 */
#define	beepSetFreq(freq)	BEEP_TIM->ARR = SYSTEM_FREQUENCY / (freq) / 2;

/*!****************************************************************************
 * Function declaration
 */
void BeepTime(uint16_t time, uint16_t ferq);
void beep_init(void);
void Dht(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif //beep_H
/******************************** END OF FILE ********************************/
