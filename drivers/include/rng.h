/*!****************************************************************************
 * @file		rng.h
 * @author		d_el
 * @version		V1.0
 * @date		24.04.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Random number generator driver
 */

#ifndef rng_H
#define rng_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Function declaration
 */
void rng_init(void);
int32_t rng_get(void);

#endif //rng_H
/******************************** END OF FILE ********************************/
