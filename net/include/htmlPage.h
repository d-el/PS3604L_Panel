/*!****************************************************************************
 * @file		htmlPage.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		20.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
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
typedef enum{
	urlDataType_html,
	urlDataType_css,
	urlDataType_bin,
	urlDataType_js,
	urlDataType_ico,
	urlDataTypeNumber
}urlType_type;

typedef struct{
	const void *payload;
	size_t size;
	urlType_type type;
}urlData_type;

typedef struct{
	char *url;
	urlData_type data;
	urlData_type (*handler)(void);
}httpResource_type;

/*!****************************************************************************
 * Exported variables
 */
extern const httpResource_type httpResource[];
extern const uint8_t getUrlNumber;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void sendHtmlPage(char *data);

#endif //htmlPage_H
/******************************** END OF FILE ********************************/
