/*!****************************************************************************
 * @file    mandelbrot.h
 * @author  Storozhenko Roman - D_EL
 * @version V1.0.0
 */
#ifndef mandelbrot_H
#define mandelbrot_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "display.h"
#include "IQmathLib.h"

/*!****************************************************************************
 * User define
 */
/* Maximum number of iterations for the escape-time calculation */
#define max_iter 32

/*!****************************************************************************
 * Prototypes for the functions
 */
void iq_mandel(_iq cx, _iq cy, _iq scale);

#endif  //mandelbrot_H
/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/
