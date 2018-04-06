/*!****************************************************************************
 * @file		uniqueDeviceId.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		26.07.2017
 * @copyright	GNU Lesser General Public License v3
 */

/*!****************************************************************************
 * Include
 */
#include "uniqueDeviceId.h"

/*!****************************************************************************
 * MEMORY
 */
const uint32_t *uniqDevId = (uint32_t*)UNIQDEVID_BASE;

const uint32_t registerId[] = {
	makeID(0x9EAA7DF7),
};
uint8_t registerIdNum = sizeof(registerId)/sizeof(uint32_t);

/*!****************************************************************************
 * @retval	displayed ID
 */
uint32_t getDid(void){
	uint32_t dId = makeID(uniqDevId[0] ^ uniqDevId[1] ^ uniqDevId[2] ^ uniqDevId[3] ^ uniqDevId[4]);
	return dId;
}

/*!****************************************************************************
 * @retval	0 - registered, 1 - not registered
 */
uint8_t checkUniqDevId(void){
	uint32_t dId = getDid();
	uint32_t tId = makeID(dId);

	for(uint8_t i = 0; i < registerIdNum; i++){
		uint32_t fId = registerId[i];
		if(tId == fId){
			return 0;
		}
	}
	return 1;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
