/*!****************************************************************************
 * @file		clock.h
 * @author		d_el
 * @version		V1.0
 * @date		09.01.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver clock system
 */
#ifndef clock_H
#define clock_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
//#include "stm32l4xx_hal.h"

/*!****************************************************************************
 * Define
 */
#define RCC_CRYSTAL_ISCILLATOR_FREQ     24000000
#define RCC_RC_ISCILLATOR_FREQ          8000000
#define HSE_PLL_MUL                     2
#define HSE_PLL_PREDIV                  2
#define HSI_PLL_MUL                     6
#define HSI_PLL_PREDIV                  1
#define RCC_WAIN_TIMEOUT                100000

/*!****************************************************************************
 * Enumeration
 */
typedef enum {
	useHsi, useHse
} useGen_type;

/*!****************************************************************************
 * Typedef
 */
typedef struct {
	uint32_t currentSysFrec;
	useGen_type useGen;
} clock_type;

/*!****************************************************************************
 * Exported variables
 */
extern clock_type clockSource;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void clock_init(void);
void Error_Handler(void);

#endif //clock_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
