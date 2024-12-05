/*!****************************************************************************
 * @file		uniqueDeviceId.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		26.07.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "uniqueDeviceId.h"

/*!****************************************************************************
 * Define
 */
#define UNIQDEVID_BASE	0x1FFF7A10

// UID[95:64]: LOT_NUM[55:24] – Lot number (ASCII encoded)
// UID[39:32]: WAF_NUM[7:0] – Wafer number (8-bit unsigned number)
// UID[63:40]: LOT_NUM[23:0] – Lot number (ASCII encoded)
// UID[31:0]: X and Y coordinates on the wafer

/*!****************************************************************************
 * MEMORY
 */
const uint32_t *const uniqDevId = (uint32_t*)UNIQDEVID_BASE;

/*!****************************************************************************
 * @retval ID
 */
uint32_t uid_get(void){
	uint32_t dId = uniqDevId[0];
	return dId;
}

/******************************** END OF FILE ********************************/
