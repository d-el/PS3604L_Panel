/*!****************************************************************************
 * @file		net.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		6 окт. 2017 г.
 * @copyright	GNU Lesser General Public License v3
 * @brief		--
 */
#ifndef net_H
#define net_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4x7_eth.h"
#include "ethernetif.h"
#include "netif.h"
#include "tcpip.h"

/*!****************************************************************************
 * Define
 */

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

/*!****************************************************************************
 * Function declaration
 */
void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway);
void netSettingUpdate(void);

#endif //net_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
