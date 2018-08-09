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

/*!*****************************************************************A***********
 * MEMORY
 */
const char html_index[] = {
	"<!DOCTYPE html>\n"
	"<html\n"
	"<head>\n"
	"	<meta charset=\"UTF-8\" />\n"
	"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
	"	<title>PS3604 State</title>\n"
	"	<link href=\"styles.css\" rel=\"stylesheet\">\n"
	"</head>\n"
	"<body>\n"
	"	<main>\n"
	"		<div>\n"
	"		<div class=\"statusbox\"></div>\n"
	"		</div>\n"
	"		\n"
	"		<div>\n"
	"		<div class=\"table-title\">STATE</div>\n"
	"			<table class=\"table-fill\">\n"
	"				<tbody class=\"table-hover table1\"></tbody>\n"
	"			</table>\n"
	"		</div>\n"
	"\n"
	"		<div>\n"
	"		<div class=\"table-title\">MEAS</div>\n"
	"			<table class=\"table-fill\">\n"
	"				<tbody class=\"table-hover table2\"></tbody>\n"
	"			</table>\n"
	"		</div>\n"
	"\n"
	"		<div>\n"
	"		<div class=\"table-title\">SETTING</div>\n"
	"			<table class=\"table-fill\">\n"
	"				<tbody class=\"table-hover table3\"></tbody>\n"
	"			</table>\n"
	"		</div>\n"
	"	</main>\n"
	"\n"
	"	<footer>\n"
	"		<div>\n"
	"		©copyright: 2013-2018 Storozhenko Roman\n"
	"		</div>\n"
	"	</footer>\n"
	"	<script src=\"overall.js\"></script>\n"
	"</body>\n"
	"</html>\n"
};

const char css_styles[] = {
	"*,\n"
	"*:after,\n"
	"*:before {\n"
	"  box-sizing: border-box;\n"
	"  z-index: 1;\n"
	"}\n"
	"body, div, dl, dt, dd, ul, ol, li, h1, h2, h3, h4, h5, h6, pre, form, fieldset, input, textarea, p, blockquote, th, td {\n"
	"	margin: 0;\n"
	"	padding: 0;\n"
	"}\n"
	"hyml, body {\n"
	"	min-height: 100vh;\n"
	"	font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
	"	background: rgb(21, 75, 101);\n"
	"	color: #D5DDE5;\n"
	"	font-size: 16px;\n"
	"	font-weight: 400;\n"
	"	text-rendering: optimizeLegibility;\n"
	"}\n"
	"body {\n"
	"	display: flex;\n"
	"	flex-direction: column;\n"
	"	justify-content: space-between;\n"
	"	align-items: stretch;\n"
	"}\n"
	"main {\n"
	"	flex-grow: 1;\n"
	"}\n"
	"footer {\n"
	"	background: #1b1e24;\n"
	"	text-align: center;\n"
	"	min-height: 50px;\n"
	"	margin-top: 30px;\n"
	"	display: flex;\n"
	"	flex-direction: column;\n"
	"	justify-content: center;\n"
	"	align-items: center;\n"
	"}\n"
	"/*** statusbox **/\n"
	".statusbox {\n"
	"	margin: 12px auto 0 auto;\n"
	"	display: none;\n"
	"	max-width: 600px;\n"
	"	color: #D5DDE5;\n"
	"	background: #398a1e;\n"
	"	border-radius: 3px;\n"
	"	font-size: 23px;\n"
	"	font-weight: 100;\n"
	"	text-align: center;\n"
	"	text-shadow: 0 1px 1px rgba(0, 0, 0, 0.1);\n"
	"	vertical-align: middle;\n"
	"	transition: all ease-in-out 500ms;\n"
	"	opacity: 1;\n"
	"	height: 35px;\n"
	"	line-height: 35px;\n"
	"}\n"
	".statusbox.hide {\n"
	"	opacity: 0;\n"
	"	transform: scale(0);\n"
	"	height: 0;\n"
	"	line-height: 0;\n"
	"}\n"
	"/*** Table Styles **/\n"
	"main > div {\n"
	"	margin: 12px 12px 0 12px;\n"
	"}\n"
	"div.table-title {\n"
	"	display: block;\n"
	"	margin: auto;\n"
	"	max-width: 600px;\n"
	"	color:#D5DDE5;\n"
	"	background:#1b1e24;\n"
	"	border-bottom:4px solid #9ea7af;\n"
	"	border-radius: 3px 3px 0 0;\n"
	"	font-size:23px;\n"
	"	font-weight: 100;\n"
	"	padding:7px;\n"
	"	text-align:left;\n"
	"	text-shadow: 0 1px 1px rgba(0, 0, 0, 0.1);\n"
	"	vertical-align:middle;\n"
	"}\n"
	".table-fill {\n"
	"	background: white;\n"
	"	border-radius:3px;\n"
	"	border-collapse: collapse;\n"
	"	margin: auto;\n"
	"	max-width: 600px;\n"
	"	padding: 3px;\n"
	"	width: 100%;\n"
	"	box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);\n"
	"	animation: float 5s infinite;\n"
	"}\n"
	"tr {\n"
	"	border-top: 1px solid #C1C3D1;\n"
	"	color:#666B85;\n"
	"	font-size:16px;\n"
	"	font-weight:normal;\n"
	"	text-shadow: 0 1px 1px rgba(256, 256, 256, 0.1);\n"
	"}\n"
	"tr:hover td {\n"
	"	background:#4E5066;\n"
	"	color:#FFFFFF;\n"
	"	border-top: 1px solid #22262e;\n"
	"}\n"
	"tr:first-child:hover td {\n"
	"	border-top: none;\n"
	"}\n"
	"tr:first-child {\n"
	"	border-top:none;\n"
	"}\n"
	"tr:last-child {\n"
	"	border-bottom:none;\n"
	"}\n"
	"tr:nth-child(odd) td {\n"
	"	background:#EBEBEB;\n"
	"}\n"
	"tr:nth-child(odd):hover td {\n"
	"	background:#4E5066;\n"
	"}\n"
	"tr:last-child td:first-child {\n"
	"	border-bottom-left-radius: 3px;\n"
	"}\n"
	"tr:last-child td:last-child {\n"
	"	border-bottom-right-radius: 3px;\n"
	"}\n"
	"td {\n"
	"	background:#FFFFFF;\n"
	"	padding: 4px 8px;\n"
	"	text-align:left;\n"
	"	vertical-align:middle;\n"
	"	font-weight:300;\n"
	"	font-size:18px;\n"
	"	text-shadow: -1px -1px 1px rgba(0, 0, 0, 0.1);\n"
	"	border-right: 1px solid #C1C3D1;\n"
	"}\n"
	"td:last-child {\n"
	"	border-right: 0px;\n"
	"}\n"
};

const char js_overall[] = {
	"function nano(template, data) {\n"
	"	return template.replace(/\\{([\\w\\.]*)\\}/g, function(str, key) {\n"
	"	var keys = key.split(\".\"), v = data[keys.shift()];\n"
	"	for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];\n"
	"	return (typeof v !== \"undefined\" && v !== null) ? v : \"\";\n"
	"	});\n"
	"}\n"
	"\n"
	"function updateTable(query, data) {\n"
	"	var table = document.querySelector(query);\n"
	"	table.innerHTML = '';\n"
	"	for(var key in data) {\n"
	"		table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);\n"
	"	}\n"
	"}\n"
	"\n"
	"function updateInfo() {\n"
	"\n"
	"	var oReq = new XMLHttpRequest();\n"
	"	oReq.open('GET', 'statemeastask.bin', true);\n"
	"	oReq.timeout = 5000;\n"
	"	oReq.responseType = 'arraybuffer';\n"
	"	\n"
	"	oReq.ontimeout = function (oEvent) {\n"
	"		var statusbox = document.querySelector('.statusbox');\n"
	"		if(statusbox.classList.contains('hide')){\n"
	"			statusbox.style.background = 'red';\n"
	"			statusbox.innerHTML = 'Connection timeout';\n"
	"			statusbox.classList.remove('hide');\n"
	"		}\n"
	"	}\n"
	"	\n"
	"	oReq.onload = function (oEvent) {\n"
	"		if(oReq.status == 200){\n"
	"			var statusbox = document.querySelector('.statusbox');\n"
	"			if(!statusbox.classList.contains('hide')){\n"
	"				statusbox.style.background = '#398a1e';\n"
	"				statusbox.style.display = 'block';\n"
	"				statusbox.innerHTML = 'Connection successful';\n"
	"				setTimeout(function() {\n"
	"					statusbox.classList.add('hide');\n"
	"				}, 5000);\n"
	"			}\n"
	"		}\n"
	"\n"
	"		var x = new DataView(oReq.response, 0);\n"
	"\n"
	"		// State\n"
	"		var state = x.getUint32(0, true);\n"
	"		updateTable('.table1', [\n"
	"			{name: 'overcurrent', val: (state & 1 << 0) ? '<p style=\"color:red;\">YES</p>' : 'NO'},\n"
	"			{name: 'switch', val: (state & 1 << 1) ? '<p style=\"color:red;\">ON</p>' : 'OFF'},\n"
	"			{name: 'output', val: (state & 1 << 2) ? 'CC' : 'CV'}\n"
	"		]);\n"
	"\n"
	"		// Meas\n"
	"		updateTable('.table2', [\n"
	"			{name: 'power', val: x.getUint32(4, true) / 1000.0 + ' Wt'},\n"
	"			{name: 'resistance', val: x.getUint32(8, true) / 1000.0 + ' Ohm'},\n"
	"			{name: 'time', val: x.getUint32(12, true) + ' s'},\n"
	"			{name: 'capacity', val: x.getUint32(16, true) / 1000.0 + ' Ah'},\n"
	"			{name: 'u', val: x.getUint32(20, true) / 1000000.0 + ' V'},\n"
	"			{name: 'i', val: x.getUint32(24, true) / 1000000.0 + ' A'},\n"
	"			{name: 'uin', val: x.getUint16(32, true) / 1000.0 + ' V'},\n"
	"			{name: 'temperature', val: x.getUint16(34, true) / 10.0 + ' °C'}\n"
	"		]);\n"
	"\n"
	"		// Settings\n"
	"		updateTable('.table3', [\n"
	"			{name: 'u', val: x.getUint32(36, true) / 1000000.0 + ' V'},\n"
	"			{name: 'i', val: x.getUint32(40, true) / 1000000.0 + ' A'},\n"
	"		]);\n"
	"	};\n"
	"	oReq.send(null);\n"
	"}\n"
	"updateInfo();\n"
	"setInterval(updateInfo, 500);\n"
};

const char html_404[] = {
	"<!DOCTYPE html>\n"
	"<html\n"
	"<head>\n"
	"	<meta charset=\"UTF-8\" />\n"
	"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
	"	<title>PS3604 State</title>\n"
	"	<link href=\"styles.css\" rel=\"stylesheet\">\n"
	"</head>\n"
	"<body>\n"
	"	<main>\n"
	"	<div class=\"error\">\n"
	"		<p>404</p>\n"
	"		<hr width=\"70%\" size=\"1\" />\n"
	"		<p>Page not found</p>\n"
	"	</div>\n"
	"	</main>\n"
	"	\n"
	"	<footer>\n"
	"		<div>\n"
	"		©copyright: 2013-2018 Storozhenko Roman\n"
	"		</div>\n"
	"	</footer>\n"
	"	\n"
	"</body>\n"
	"</html>\n"
};

const uint8_t bin_favicon[190] = {
	0x42,0x4d,0xbe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x28,
	0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
	0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0xc3,0x0e,0x00,0x00,0xc3,0x0e,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
	0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xe0,0x0f,0xff,0xff,0x00,0x03,0xff,0xfe,
	0x00,0x00,0xff,0xfc,0x0f,0xe0,0x7f,0xf8,0x7f,0xfc,0x3f,0xf0,0xff,0xfe,0x1f,
	0xe1,0xff,0xff,0x0f,0xe1,0xcf,0xf7,0x8f,0xc3,0xc7,0xc7,0x87,0xc7,0xe3,0x8f,
	0xc7,0xc7,0xe8,0x2f,0xe3,0x8f,0xe6,0x4f,0xe3,0x8f,0xf7,0xcf,0xe3,0x8f,0xe7,
	0xcf,0xe3,0x8f,0xc7,0xc7,0xe3,0x8f,0x8f,0xf3,0xe3,0x8f,0x03,0x81,0xe3,0x8e,
	0x01,0x00,0x63,0xc7,0xfd,0x3f,0xe3,0xc7,0xfd,0x7f,0xc7,0xc3,0xfc,0x7f,0x87,
	0xe3,0xfe,0x7f,0x8f,0xe1,0xfe,0xff,0x0f,0xf0,0xfe,0xfe,0x1f,0xf8,0x7f,0xfc,
	0x3f,0xfc,0x0f,0xf0,0x7f,0xfe,0x00,0x00,0xff,0xff,0x00,0x03,0xff,0xff,0xe0,
	0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

transfer_type bin_statemeastask;

const url_type getUrlTable[] = {
	{
		.url = "/ ",
		.handler = NULL,
		.data.type = urlDataType_html,
		.data.data.html = html_index,
		.data.size = sizeof(html_index) - 1,
	},
	{
		.url = "/overall.js",
		.handler = NULL,
		.data.type = urlDataType_js,
		.data.data.bin = js_overall,
		.data.size = sizeof(js_overall) - 1,
	},
	{
		.url = "/styles.css",
		.handler = NULL,
		.data.type = urlDataType_css,
		.data.data.bin = css_styles,
		.data.size = sizeof(css_styles) - 1,
	},
	{
		.url = "/statemeastask.bin",
		.handler = handle_statemeastask,
		.data.type = urlDataType_bin,
		.data.size = sizeof(bin_statemeastask),
	},
	{
		.url = "/favicon.ico",
		.handler = NULL,
		.data.type = urlDataType_ico,
		.data.data.bin = bin_favicon,
		.data.size = sizeof(bin_favicon),
	},
	{
		.url = "/404",
		.handler = NULL,
		.data.type = urlDataType_html,
		.data.data.html = html_404,
		.data.size = sizeof(html_404) - 1,
	},
};

const uint8_t getUrlNumber = sizeof(getUrlTable) / sizeof(getUrlTable[0]);

/*!****************************************************************************
 *
 */
urlData_type handle_statemeastask(void){
	static urlData_type urlData;
	memcpy(&bin_statemeastask, &fp.tf, sizeof(transfer_type));
	urlData.data.bin = &bin_statemeastask;
	urlData.size = sizeof(transfer_type);
	urlData.type = urlDataType_bin;
	return urlData;
}

/******************************** END OF FILE ********************************/
