/*!****************************************************************************
 * @file		rng.h
 * @author		d_el
 * @version		V1.0
 * @date		24.04.2018
 * @brief		Random number generator driver
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
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
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/
