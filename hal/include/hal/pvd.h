/*!****************************************************************************
 * @file		pvd.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.08.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver power voltage detector
 */
#ifndef pvd_H
#define pvd_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */
#define PVD_IRQ_Priority	(13)	///< PVD Interrupt priority

/*!****************************************************************************
 * Typedef
 */
typedef void (*suplyFaultCallBack_type)(void);

/*!****************************************************************************
 * Prototypes for the functions
 */
void pvd_init(void);
void pvd_disable(void);
void pvd_setSupplyFaultCallBack(suplyFaultCallBack_type callBack);

#ifdef __cplusplus
}
#endif

#endif //pvd_H
/******************************** END OF FILE ********************************/
