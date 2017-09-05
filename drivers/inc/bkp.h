/*!****************************************************************************
 * @file    bkp.h
 * @author  d_el
 * @version V1.0
 * @date    01.08.2016, Storozhenko Roman
 * @brief   --
 * @copyright GNU Public License
 */
#ifndef bkp_H
#define bkp_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User enum
 */
typedef enum {
	bkpOk = 0, bkpError,
} bkpStatus_type;

/*!****************************************************************************
 * User typedef
 */

/*!****************************************************************************
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */
bkpStatus_type bkp_write(uint8_t dst, void *src, uint8_t len);
bkpStatus_type bkp_read(uint8_t *dst, uint8_t src, uint8_t len);

#endif //bkp_H
/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/
