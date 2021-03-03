/*!****************************************************************************
 * @file		enco.h
 * @author		d_el
 * @version		V2.4
 * @date		16.12.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2018 Storozhenko Roman
 * @brief		encoder driver
 */
#ifndef ENCO_H
#define ENCO_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Prototypes for the functions
 */
void enco_init(void);
int16_t enco_read(void);
int32_t enco_update(void);
void enco_settic(uint16_t n);

#ifdef __cplusplus
}
#endif

#endif //ENCO_H
/******************************** END OF FILE ********************************/
