/*!****************************************************************************
 * @file		spi.h
 * @author		d_el
 * @version		V1.1
 * @date		11.01.2026
 * @copyright	License (MIT). Copyright (c) 2026 Storozhenko Roman
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
typedef enum{
	spiDiv2,
	spiDiv4,
	spiDiv8,
	spiDiv16,
	spiDiv32,
	spiDiv64,
	spiDiv128,
	spiDiv256,
}spiDiv_t;

typedef void (*spicb_type)(void* arg);

/*!****************************************************************************
 * Function declaration
 */
void spi_init(spiDiv_t divClock);
void spi_send(uint8_t data);
void spi_initBlock(void);
void spi_writeBlock(uint16_t* block, size_t len, spicb_type cb, void* arg);
void spi_fillBlock(uint16_t* block, size_t len, const uint16_t* color, spicb_type cb, void* arg);

#endif //spi_H
/******************************** END OF FILE ********************************/
