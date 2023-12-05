/*!****************************************************************************
 * @file		spi.h
 * @author		d_el
 * @version		V1.0
 * @date		05.12.2023
 * @copyright	License (MIT). Copyright (c) 2023 Storozhenko Roman
 * @brief
 */

#ifndef spi_H
#define spi_H

/*!****************************************************************************
 * Include
 */
#include <stddef.h>
#include <stdint.h>

/*!****************************************************************************
 * Typedef
 */
typedef void (*spicb_type)(void);

/*!****************************************************************************
 * Function declaration
 */
void spi_init(void);
void spi_send(uint8_t data);
void spi_initBlock(void);
void spi_writeBlock(uint16_t* block, size_t len, spicb_type cb);
void spi_fillBlock(uint16_t* block, size_t len, const uint16_t* color, spicb_type cb);

#endif //spi_H
/******************************** END OF FILE ********************************/
