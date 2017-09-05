/*!****************************************************************************
 * @file		key.c
 * @author		d_el
 * @version		V1.0
 * @date		07.06.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver for keyboard
 */

/*!****************************************************************************
 * Include
 */
#include "key.h"

/*!****************************************************************************
 * Memory
 */
key_type key;

/*!****************************************************************************
 * @brief
 */
void dInUpdate(key_type *pkey, uint32_t num, uint32_t val){
	if(val != 0){
		if(pkey->dInFilterCnt[num] < (KEY_SAMPLES - 1)){
			pkey->dInFilterCnt[num]++;
		}else{
			pkey->dInState |= 1U << num;
		}
	}else{
		if(pkey->dInFilterCnt[num] > 0){
			pkey->dInFilterCnt[num]--;
		}else{
			pkey->dInState &= ~(1U << num);
		}
	}
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
uint32_t keyProc(void){
	key_type *pkey = &key;
	uint32_t mask;
	uint32_t iterator = 0;

	//Physical buttons
	dInUpdate(pkey, 0, !gppin_get(GP_bMode));
	dInUpdate(pkey, 1, !gppin_get(GP_bView));
	dInUpdate(pkey, 2, !gppin_get(GP_bOnOff));
	dInUpdate(pkey, 3, !gppin_get(GP_bNext));
	dInUpdate(pkey, 4, !gppin_get(GP_bZero));
	dInUpdate(pkey, 5, !gppin_get(GP_bUp));
	dInUpdate(pkey, 6, !gppin_get(GP_bDown));

	//Detect signal front
	for(mask = 1 << 0; mask < (1 << KEY_NUM); mask <<= 1){
		if(((pkey->dInPrevState & mask) == 0) && ((pkey->dInState & mask) != 0)){
			pkey->keyState |= mask;
		}else{
			pkey->keyState &= ~mask;
		}

		//Reiteration
		if((pkey->reiterationSelect & mask) != 0){
			if((pkey->dInState & mask) != 0){
				if(pkey->toFirstReiterationCnt[iterator] < pkey->toFirstReiteration){
					pkey->toFirstReiterationCnt[iterator]++;
				}else{
					if(pkey->toReiterationCnt[iterator] < pkey->toReiteration){
						pkey->toReiterationCnt[iterator]++;
					}else{
						pkey->keyState |= mask;
						pkey->toReiterationCnt[iterator] = 0;
					}
				}
			}else{
				pkey->toFirstReiterationCnt[iterator] = 0;
				pkey->toReiterationCnt[iterator] = 0;
			}
		}
		iterator++;
	}
	pkey->dInPrevState = pkey->dInState;

	return pkey->keyState;
}

/*!****************************************************************************
 * @param keyNum
 * @return
 */
uint32_t keyState(kKey_type keyMask){
	if((key.keyState & keyMask) != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @param toFirstReiteration
 * @param toReiteration
 * @param autoPress
 */
void ksSet(uint16_t toFirstReiteration, uint16_t toReiteration, kKey_type reiterationKeyMask){
	key.toFirstReiteration = toFirstReiteration;
	key.toReiteration = toReiteration;
	key.reiterationSelect = reiterationKeyMask;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
