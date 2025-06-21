/*!****************************************************************************
 * @file		prmSystemCallback.h
 * @author		d_el
 * @version		V1.0
 * @date		Dec 11, 2019
 * @brief
 */

#ifndef prmSystemCallback_H
#define prmSystemCallback_H

/*!****************************************************************************
 * Include
 */
#include "prmSystem.h"

/*!****************************************************************************
 * Function declaration
 */
void getFwVer(Prm::Val<uint16_t>& prm, bool read, void *arg);

#endif //prmSystemCallback_H
/******************************** END OF FILE ********************************/
