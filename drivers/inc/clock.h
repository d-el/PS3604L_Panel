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

/*!****************************************************************************
 * Define
 */

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
