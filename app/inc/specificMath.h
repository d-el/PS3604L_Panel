/*!****************************************************************************
 * @file    specificMath.h 
 * @author  Storozhenko Roman - D_EL
 * @version V1.1
 * @date    12-09-2015
 * @brief
 */
#ifndef specificMath_H
#define specificMath_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "IQmathLib.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User typedef
 */

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */
#define iq_filtr(valOld, valNew, Kfiltr) (((valOld = valOld + (valNew - (valOld >> Kfiltr)))) >> Kfiltr)
#define IQtoInt(_iqval, _mul)  (int32_t)(((int64_t)_iqval * _mul + _IQ(1)/2) / _IQ(1))
#define IntToIQ(_intval, _mul) (_iq)(((int64_t)_intval * _IQ(1) + _mul/2) / _mul)
#define IQ14toInt(_iqval, _mul) (int32_t)(((int64_t)_iqval * _mul + _IQ14(1)/2) / _IQ14(1))
#define divWithRounding(val, div) (((val) + ((div / 2))) / (div))		/* Divide with rounding */
/*!****************************************************************************
 * Prototypes for the functions
 */
_iq iq_Fy_x1x2y1y2x(_iq x1, _iq x2, _iq y1, _iq y2, _iq x);
_iq s32iq_Fy_x1x2y1y2x(int32_t x1, int32_t x2, _iq y1, _iq y2, int32_t x);
int32_t iqs32_Fy_x1x2y1y2x(_iq x1, _iq x2, int32_t y1, int32_t y2, _iq x);

#endif //specificMath_H
/*************** (C) COPYRIGHT ************** END OF FILE ********* D_EL *****/
