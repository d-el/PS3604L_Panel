/*!****************************************************************************
 * @file		htmlPage.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		20.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "string.h"
#include "systemTSK.h"
#include "htmlPage.h"

/*!****************************************************************************
 * Local prototypes for the functions
 */
urlData_type handle_statemeastask(void);

/*!****************************************************************************
 * MEMORY
 */
transfer_type bin_statemeastask;

#define resource_reg(resource) \
	const char *const _binary_##resource##_start; \
	const char *const _binary_##resource##_end; \
	const size_t _binary_##resource##_size;

#define resource_add(resource) \
		.data.payload = &_binary_##resource##_start, \
		.data.size = (size_t)&_binary_##resource##_size

resource_reg(net_resource_index_html)
resource_reg(net_resource_overall_js)
resource_reg(net_resource_styles_css)
resource_reg(net_resource_favico_bmp)
resource_reg(net_resource_404_html)

const httpResource_type httpResource[] = {
	{
		.url = "/ ",
		.data.type = urlDataType_html,
		resource_add(net_resource_index_html),
		.handler = NULL
	},
	{
		.url = "/overall.js",
		.data.type = urlDataType_js,
		resource_add(net_resource_overall_js),
		.handler = NULL
	},
	{
		.url = "/styles.css",
		.data.type = urlDataType_css,
		resource_add(net_resource_styles_css),
		.handler = NULL
	},
	{
		.url = "/statemeastask.bin",
		.data.type = urlDataType_bin,
		.data.size = sizeof(bin_statemeastask),
		.handler = handle_statemeastask
	},
	{
		.url = "/favicon.ico",
		.data.type = urlDataType_ico,
		resource_add(net_resource_favico_bmp),
		.handler = NULL
	},
	{
		.url = "/404",
		.data.type = urlDataType_html,
		resource_add(net_resource_404_html),
		.handler = NULL
	},
};

const uint8_t getUrlNumber = sizeof(httpResource) / sizeof(httpResource[0]);

/*!****************************************************************************
 *
 */
urlData_type handle_statemeastask(void){
	static urlData_type urlData;
	memcpy(&bin_statemeastask, &fp.tf, sizeof(transfer_type));
	urlData.payload = &bin_statemeastask;
	urlData.size = sizeof(transfer_type);
	urlData.type = urlDataType_bin;
	return urlData;
}

/******************************** END OF FILE ********************************/
