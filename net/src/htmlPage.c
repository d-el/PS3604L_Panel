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
#include <string.h>
#include <systemTSK.h>
#include "htmlPage.h"
#include <regulatorConnTSK.h>

/*!****************************************************************************
 * Local prototypes for the functions
 */
urlData_type handle_statemeastask(void);

/*!****************************************************************************
 * MEMORY
 */
typedef struct __attribute__ ((packed)){
	uint16_t major;
	uint16_t minor;
	uint16_t patch;

	uint8_t connect;
	uint8_t enable;

	int32_t voltage;		///< [X_XXXXXX V]
	int32_t current;		///< [X_XXXXXX A]
	uint32_t power;			///< [X_XXX Wt]
	int32_t resistance;		///< [X_XXX Ohm]
	uint32_t time;			///< [s]
	uint32_t capacity;		///< [X_XXX Ah]
	int32_t input_voltage;	///< [X_XXXXXX V]
	int16_t temperature;	///< [X_X °С]
	uint16_t status;
	uint16_t disablecause;

	int32_t voltage_set;		///< [X_XXXXXX V]
	int32_t current_set;		///< [X_XXXXXX A]
} powerSupplyState_t;

powerSupplyState_t bin_statemeastask;

#define resource_reg(resource) \
	extern const char *const _binary_##resource##_start; \
	extern const char *const _binary_##resource##_end; \
	extern const size_t _binary_##resource##_size;

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
	regState_t regMeas = {};
	bin_statemeastask.connect = reg_getState(&regMeas);
	if(bin_statemeastask.connect){
		bin_statemeastask.voltage = regMeas.voltage;
		bin_statemeastask.current = regMeas.current;
		bin_statemeastask.power = regMeas.power;
		bin_statemeastask.resistance = regMeas.resistance;
		bin_statemeastask.time = regMeas.time;
		bin_statemeastask.capacity = regMeas.capacity;
		bin_statemeastask.input_voltage = regMeas.input_voltage;
		bin_statemeastask.temperature = regMeas.temp_heatsink;
		bin_statemeastask.status = regMeas.status.all;
		bin_statemeastask.disablecause = regMeas.disablecause;

		bool regenable = false;
		reg_getEnable(&regenable);
		bin_statemeastask.enable = regenable ? 1 : 0;

		regTarget_t regTarget = {};
		reg_getTarget(&regTarget);
		bin_statemeastask.voltage_set = regTarget.voltage_set;
		bin_statemeastask.current_set = regTarget.current_set;

		regVersion_t v;
		reg_getVersion(&v);
		bin_statemeastask.major = v.major;
		bin_statemeastask.minor = v.minor;
		bin_statemeastask.patch = v.patch;
	}

	urlData.payload = &bin_statemeastask;
	urlData.size = sizeof(bin_statemeastask);
	urlData.type = urlDataType_bin;
	return urlData;
}

/******************************** END OF FILE ********************************/
