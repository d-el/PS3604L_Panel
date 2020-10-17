/*!****************************************************************************
 * @file		prmEditor.h
 * @author		d_el
 * @version		V1.0
 * @date		16.12.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2018 Storozhenko Roman
 * @brief		--
 */
#ifndef prmEditor_H
#define prmEditor_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include "prmSystem.h"

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
	enNoCharge,
	enCharge,
	enNoLim,
	enLimDown,
	enLimUp,
	enTransitionDown,
	enTransitionUp,
} prmEditorStatus_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function prototype
 */
void prmEditorWriteVal(const prmHandle_type *prmHandle, const prmval_type *prmval);
prmEditorStatus_type prmEditorAdd(const prmHandle_type *prmHandle, const prmval_type *prmval, int32_t multiply);
prmEditorStatus_type prmEditorBigStepUp(const prmHandle_type *prmHandle);
prmEditorStatus_type prmEditorBigStepDown(const prmHandle_type *prmHandle);
prmEditorStatus_type prmEditorUpDate(const prmHandle_type *prmHandle);
void prmEditorSetNtic(uint16_t n);

#endif //prmEditor_H
/******************************** END OF FILE ********************************/
