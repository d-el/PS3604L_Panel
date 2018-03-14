/*!****************************************************************************
 * @file		printp.h
 * @author		d_el
 * @version		V1.0
 * @date		06.12.2017
 * @brief		System control task
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
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

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */
#ifndef NDEBUG           /* required by ANSI standard */

	#define printp(...) l_print(__VA_ARGS__)

	#define report(test, ...) 				\
		do { 								\
			if(test){ 						\
				l_print(__VA_ARGS__); 		\
			} 								\
		}while(0);

	#define stopif(assertion, error_action, ...) {              \
		if (assertion){                                         \
			l_print(__VA_ARGS__); 								\
			{													\
				error_action;									\
			}                  									\
		} }

#else //NDEBUG

	#define printp(...) ((void)0)

	#define report(test, ...) ((void)0)

	#define stopif(assertion, error_action, ...) ((void)0)

#endif /* !NDEBUG */

/*!****************************************************************************
 * Function declaration
 */
void l_print(const char *fmt, ...);

#endif //printp_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
