/*!****************************************************************************
 * @file		mandelbrot.h
 * @author		Storozhenko Roman - D_EL
 * @version		V1.0.0
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef mandelbrot_H
#define mandelbrot_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
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

#endif	//mandelbrot_H
/******************************** END OF FILE ********************************/
