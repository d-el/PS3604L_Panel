/*!****************************************************************************
 * @file		htmlPage.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		20.09.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		--
 */
#ifndef htmlPage_H
#define htmlPage_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef struct{
	enum{
		urlDataTypeHtml,
		urlDataTypeBin,
	}type;

	union{
		const char *html;
		const void *bin;
	}data;

	uint32_t	size;
}urlData_type;

typedef struct{
	char				*url;
	urlData_type		(*handler)(void);
	urlData_type		data;
}url_type;

/*!****************************************************************************
 * Exported variables
 */
extern const url_type getUrlTable[];
extern const uint8_t getUrlNumber;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void sendHtmlPage(char *data);

#endif //htmlPage_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
