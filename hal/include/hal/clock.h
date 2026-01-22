/*!****************************************************************************
 * @file		clock.h
 * @author		d_el
 * @version		V1.0
 * @date		17.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		MCU peripheral initialization
 */
#ifndef clock_H
#define clock_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	clockOk,
	clockErorHse
}clockState_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
clockState_type clock_init(void);

#ifdef __cplusplus
}
#endif

#endif //clock_H
/******************************** END OF FILE ********************************/
