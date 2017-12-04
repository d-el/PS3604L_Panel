/*!****************************************************************************
 * @file		debugPrint.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		6 окт. 2017 г.
 * @copyright	GNU Lesser General Public License v3
 * @brief		--
 */
#ifndef debugPrint_H
#define debugPrint_H

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
void println(const char *fmt, ...);
void print(const char *fmt, ...);

#ifdef NDEBUG           /* required by ANSI standard */
#	define print(e) ((void)0)
#else
#	define print l_print
#endif /* !NDEBUG */

#ifdef NDEBUG           /* required by ANSI standard */
#	define println(e) ((void)0)
#else
#	define println l_println
#endif /* !NDEBUG */

#ifdef NDEBUG           /* required by ANSI standard */
#	define printdmsg(debug, message) ((void)0)
#else
#	define printdmsg(debug, message) \
		do { \
			if(debug){ \
				print message; \
			} \
		}while(0);
#endif /* !NDEBUG */

#define stopif(assertion, error_action, ...) {                    	\
        if (assertion){                                           	\
        	println(__VA_ARGS__); 									\
            {error_action;}                  						\
        } }

/*!****************************************************************************
 * Function declaration
 */
void l_print(const char *fmt, ...);
void l_println(const char *fmt, ...);

#endif //debugPrint_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
