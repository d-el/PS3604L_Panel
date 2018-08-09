﻿/*!****************************************************************************
 * @file		write.h
 * @author		d_el
 * @version		V1.0
 * @date		06.12.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		System control task
 */
#ifndef printp_H
#define printp_H

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum{
	write_semihost,
	write_rtt,
	write_uart,
}stdOutInterface_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void print_init(stdOutInterface_type stdoi);

#endif //printp_H
/******************************** END OF FILE ********************************/
