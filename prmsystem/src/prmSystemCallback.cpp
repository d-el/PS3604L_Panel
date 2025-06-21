/*!****************************************************************************
 * @file		prmSystemCallback.c
 * @author		d_el
 * @version		V1.0
 * @date		Dec 11, 2019
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <stddef.h>
#include <version.h>
#include "prmSystemCallback.h"

void getFwVer(Prm::Val<uint16_t>& prm, bool read, void *arg){
	(void)read;
	(void)arg;

	switch(reinterpret_cast<uint32_t>(prm.getarg())){
		case 0:
			prm.val = getVersionMajor();
			break;
		case 1:
			prm.val = getVersionMinor();
			break;
		case 2:
			prm.val = getVersionPatch();
			break;
		case 3:
			prm.val = getVersionTweak();
			break;
	}
}

/******************************** END OF FILE ********************************/
