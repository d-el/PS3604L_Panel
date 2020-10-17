/*!****************************************************************************
 * @file		httpServerTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		HTTP Server
 */
#ifndef httpServerTSK_H
#define httpServerTSK_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef struct{
	uint32_t	numberRequest;
}httpServer_type;

/*!****************************************************************************
 * Exported variables
 */
extern httpServer_type httpServer;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void httpServerTSK(void *pPrm);

#endif //httpServerTSK__H
/******************************** END OF FILE ********************************/
