/*!****************************************************************************
 * @file		htmlPage.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		20 сент. 2017 г.
 * @copyright	GNU Lesser General Public License v3
 * @brief		--
 */

/*!****************************************************************************
 * Include
 */
#include "htmlPage.h"

/*!****************************************************************************
 * Local prototypes for the functions
 */
urlData_type handle_statemeastask(void);

/*!****************************************************************************
 * MEMORY
 */
const char css_reset[] = {
	"*,\r\n"
	"*:after,\r\n"
	"*:before {\r\n"
	"  box-sizing: border-box;\r\n"
	"  z-index: 1;\r\n"
	"}\r\n"
	"body, div, dl, dt, dd, ul, ol, li, h1, h2, h3, h4, h5, h6, pre, form, fieldset, input, textarea, p, blockquote, th, td {\r\n"
	"	margin: 0;\r\n"
	"	padding: 0;\r\n"
	"}\r\n"
	"table {\r\n"
	"	border-collapse: collapse;\r\n"
	"	border-spacing: 0;\r\n"
	"}\r\n"
	"fieldset, img {\r\n"
	"	border: 0;\r\n"
	"}\r\n"
	"address, caption, cite, code, dfn, em, strong, th, var {\r\n"
	"	font-style: normal;\r\n"
	"	font-weight: normal;\r\n"
	"}\r\n"
	"ol, ul {\r\n"
	"	list-style: none;\r\n"
	"}\r\n"
	"caption, th {\r\n"
	"	text-align: left;\r\n"
	"}\r\n"
	"h1, h2, h3, h4, h5, h6 {\r\n"
	"	font-size: 100%;\r\n"
	"	font-weight: normal;\r\n"
	"}\r\n"
	"q:before, q:after {\r\n"
	"	content: '';\r\n"
	"}\r\n"
	"abbr, acronym {\r\n"
	"	border: 0;\r\n"
	"}\r\n"
	"header, nav, section, article, aside, footer {\r\n"
	"	display: block;\r\n"
	"}\r\n"
	"body {\r\n"
	"	width: 100%;\r\n"
	"	color: #36D695;\r\n"
	"}\r\n"
	"a:link, a:active, a:visited {\r\n"
	"	color: #fff;\r\n"
	"	text-decoration: none;\r\n"
	"}\r\n"
	"a:active {\r\n"
	"	outline: none;\r\n"
	"}\r\n"
	"a:hover {\r\n"
	"	text-decoration: underline;\r\n"
	"}\r\n"
	"p {\r\n"
	"	margin: 0\r\n"
	"}\r\n"
	"img {\r\n"
	"	border: none;\r\n"
	"}\r\n"
	"hr {\r\n"
	"    background-color: #fff;\r\n"
	"    border: none;\r\n"
	"    clear: both;\r\n"
	"    color: #fff;\r\n"
	"    height: 1px;\r\n"
	"}\r\n"
	"input, textarea {\r\n"
	"	outline: none;\r\n"
	"}\r\n"
	"input:active, textarea:active {\r\n"
	"	outline: none;\r\n"
	"}\r\n"
	":focus {\r\n"
	"	outline: none;\r\n"
	"}\r\n"
	"input {\r\n"
	"	vertical-align: middle;\r\n"
	"	border: none;\r\n"
	"}\r\n"
	"textarea {\r\n"
	"    resize: none;\r\n"
	"	border: none;\r\n"
	"}\r\n"
	"input:focus::-webkit-input-placeholder,\r\n"
	"input:focus:-moz-placeholder {\r\n"
	"    color: transparent !important;\r\n"
	"}\r\n"
	"button {\r\n"
	"	outline: none;\r\n"
	"	border: none;\r\n"
	"}\r\n"
};

const char css_styles[] = {
		"*,\r\n"
		"*:after,\r\n"
		"*:before {\r\n"
		"  box-sizing: border-box;\r\n"
		"  z-index: 1;\r\n"
		"}\r\n"
		"body, div, dl, dt, dd, ul, ol, li, h1, h2, h3, h4, h5, h6, pre, form, fieldset, input, textarea, p, blockquote, th, td {\r\n"
		"	margin: 0;\r\n"
		"	padding: 0;\r\n"
		"}\r\n"
		"hyml, body {\r\n"
		"    min-height: 100vh;\r\n"
		"    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\r\n"
		"    background: #002B36;\r\n"
		"    color: #AB7C4D;\r\n"
		"    font-size: 16px;\r\n"
		"    font-weight: 400;\r\n"
		"    text-rendering: optimizeLegibility;\r\n"
		"}\r\n"
		"\r\n"
		"body {\r\n"
		"    display: flex;\r\n"
		"    flex-direction: column;\r\n"
		"    justify-content: space-between;\r\n"
		"    align-items: stretch;\r\n"
		"}\r\n"
		"\r\n"
		"main {\r\n"
		"    flex-grow: 1;\r\n"
		"    padding: 10px;\r\n"
		"}\r\n"
		"\r\n"
		"main > div {\r\n"
		"    margin-top: 20px;\r\n"
		"}\r\n"
		"\r\n"
		"footer {\r\n"
		"    align-self: center;\r\n"
		"    margin: 10px;\r\n"
		"}\r\n"
		"\r\n"
		"\r\n"
		"\r\n"
		"\r\n"
		"div.table-title {\r\n"
		"    display: block;\r\n"
		"    margin: auto;\r\n"
		"    max-width: 600px;\r\n"
		"    color:#D5DDE5;\r\n"
		"    background:#1b1e24;\r\n"
		"    border-bottom:4px solid #9ea7af;\r\n"
		"    border-right: 1px solid #343a45;\r\n"
		"	border-radius: 3px 3px 0 0;\r\n"
		"    font-size:23px;\r\n"
		"    font-weight: 100;\r\n"
		"    padding:7px;\r\n"
		"    text-align:left;\r\n"
		"    text-shadow: 0 1px 1px rgba(0, 0, 0, 0.1);\r\n"
		"    vertical-align:middle;\r\n"
		"}\r\n"
		"  \r\n"
		"  \r\n"
		"/*** Table Styles **/\r\n"
		".table-fill {\r\n"
		"    background: white;\r\n"
		"    border-radius:3px;\r\n"
		"    border-collapse: collapse;\r\n"
		"    margin: auto;\r\n"
		"    max-width: 600px;\r\n"
		"    padding: 3px;\r\n"
		"    width: 100%;\r\n"
		"    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);\r\n"
		"    animation: float 5s infinite;\r\n"
		"}\r\n"
		"   \r\n"
		"\r\n"
		"tr {\r\n"
		"    border-top: 1px solid #C1C3D1;\r\n"
		"    color:#666B85;\r\n"
		"    font-size:16px;\r\n"
		"    font-weight:normal;\r\n"
		"    text-shadow: 0 1px 1px rgba(256, 256, 256, 0.1);\r\n"
		"}\r\n"
		"tr:hover td {\r\n"
		"    background:#4E5066;\r\n"
		"    color:#FFFFFF;\r\n"
		"    border-top: 1px solid #22262e;\r\n"
		"}\r\n"
		"tr:first-child:hover td {\r\n"
		"    border-top: none;\r\n"
		"}\r\n"
		"tr:first-child {\r\n"
		"    border-top:none;\r\n"
		"}\r\n"
		"tr:last-child {\r\n"
		"    border-bottom:none;\r\n"
		"}\r\n"
		"\r\n"
		"   \r\n"
		"tr:nth-child(odd) td {\r\n"
		"    background:#EBEBEB;\r\n"
		"}\r\n"
		" \r\n"
		"tr:nth-child(odd):hover td {\r\n"
		"    background:#4E5066;\r\n"
		"}\r\n"
		"\r\n"
		"tr:last-child td:first-child {\r\n"
		"    border-bottom-left-radius: 3px;\r\n"
		"}\r\n"
		" \r\n"
		"tr:last-child td:last-child {\r\n"
		"    border-bottom-right-radius: 3px;\r\n"
		"}\r\n"
		" \r\n"
		"td {\r\n"
		"    background:#FFFFFF;\r\n"
		"    padding: 3px;\r\n"
		"    text-align:left;\r\n"
		"    vertical-align:middle;\r\n"
		"    font-weight:300;\r\n"
		"    font-size:18px;\r\n"
		"    text-shadow: -1px -1px 1px rgba(0, 0, 0, 0.1);\r\n"
		"    border-right: 1px solid #C1C3D1;\r\n"
		"}\r\n"
		"td:last-child {\r\n"
		"    border-right: 0px;\r\n"
		"}"
};

const char html_index[] = {
		"<!DOCTYPE html>\r\n"
		"<html lang=\"ru\">\r\n"
		"<head>\r\n"
		"	<meta charset=\"UTF-8\" />\r\n"
		"    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \r\n"
		"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n"
		"	<title>PS3604 State</title>\r\n"
		"    <style>\r\n"
		"        *,\r\n"
		"    *:after,\r\n"
		"    *:before {\r\n"
		"      box-sizing: border-box;\r\n"
		"      z-index: 1;\r\n"
		"    }\r\n"
		"    body, div, dl, dt, dd, ul, ol, li, h1, h2, h3, h4, h5, h6, pre, form, fieldset, input, textarea, p, blockquote, th, td {\r\n"
		"        margin: 0;\r\n"
		"        padding: 0;\r\n"
		"    }\r\n"
		"    hyml, body {\r\n"
		"        min-height: 100vh;\r\n"
		"        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\r\n"
		"        background: rgb(21, 75, 101);\r\n"
		"        color: #D5DDE5;\r\n"
		"        font-size: 16px;\r\n"
		"        font-weight: 400;\r\n"
		"        text-rendering: optimizeLegibility;\r\n"
		"    }\r\n"
		"\r\n"
		"    body {\r\n"
		"        display: flex;\r\n"
		"        flex-direction: column;\r\n"
		"        justify-content: space-between;\r\n"
		"        align-items: stretch;\r\n"
		"    }\r\n"
		"\r\n"
		"    main {\r\n"
		"        flex-grow: 1;\r\n"
		"    }\r\n"
		"\r\n"
		"    footer {\r\n"
	    "        background: #1b1e24;\r\n"
		"        text-align: center;\r\n"
	    "        min-height: 50px;\r\n"
	    "        margin-top: 50px;\r\n"
	    "        display: flex;\r\n"
	    "        flex-direction: column;\r\n"
	    "        justify-content: center;\r\n"
	    "        align-items: center;\r\n"
		"    }\r\n"
		"\r\n"
		"    /*** Table Styles **/\r\n"
		"    main > div {\r\n"
		"        margin: 12px 12px 0 12px;\r\n"
		"    }\r\n"
		"\r\n"
		"    div.table-title {\r\n"
		"        display: block;\r\n"
		"        margin: auto;\r\n"
		"        max-width: 600px;\r\n"
		"        color:#D5DDE5;\r\n"
		"        background:#1b1e24;\r\n"
		"        border-bottom:4px solid #9ea7af;\r\n"
		"        border-right: 1px solid #343a45;\r\n"
		"        border-radius: 3px 3px 0 0;\r\n"
		"        font-size:23px;\r\n"
		"        font-weight: 100;\r\n"
		"        padding:7px;\r\n"
		"        text-align:left;\r\n"
		"        text-shadow: 0 1px 1px rgba(0, 0, 0, 0.1);\r\n"
		"        vertical-align:middle;\r\n"
		"    }\r\n"
		"      \r\n"
		"    .table-fill {\r\n"
		"        background: white;\r\n"
		"        border-radius:3px;\r\n"
		"        border-collapse: collapse;\r\n"
		"        margin: auto;\r\n"
		"        max-width: 600px;\r\n"
		"        padding: 3px;\r\n"
		"        width: 100%;\r\n"
		"        box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);\r\n"
		"        animation: float 5s infinite;\r\n"
		"    }\r\n"
		"\r\n"
		"    tr {\r\n"
		"        border-top: 1px solid #C1C3D1;\r\n"
		"        color:#666B85;\r\n"
		"        font-size:16px;\r\n"
		"        font-weight:normal;\r\n"
		"        text-shadow: 0 1px 1px rgba(256, 256, 256, 0.1);\r\n"
		"    }\r\n"
		"    tr:hover td {\r\n"
		"        background:#4E5066;\r\n"
		"        color:#FFFFFF;\r\n"
		"        border-top: 1px solid #22262e;\r\n"
		"    }\r\n"
		"    tr:first-child:hover td {\r\n"
		"        border-top: none;\r\n"
		"    }\r\n"
		"    tr:first-child {\r\n"
		"        border-top:none;\r\n"
		"    }\r\n"
		"    tr:last-child {\r\n"
		"        border-bottom:none;\r\n"
		"    }\r\n"
		"       \r\n"
		"    tr:nth-child(odd) td {\r\n"
		"        background:#EBEBEB;\r\n"
		"    }\r\n"
		"     \r\n"
		"    tr:nth-child(odd):hover td {\r\n"
		"        background:#4E5066;\r\n"
		"    }\r\n"
		"\r\n"
		"    tr:last-child td:first-child {\r\n"
		"        border-bottom-left-radius: 3px;\r\n"
		"    }\r\n"
		"     \r\n"
		"    tr:last-child td:last-child {\r\n"
		"        border-bottom-right-radius: 3px;\r\n"
		"    }\r\n"
		"     \r\n"
		"    td {\r\n"
		"        background:#FFFFFF;\r\n"
		"        padding: 3px;\r\n"
		"        text-align:left;\r\n"
		"        vertical-align:middle;\r\n"
		"        font-weight:300;\r\n"
		"        font-size:18px;\r\n"
		"        text-shadow: -1px -1px 1px rgba(0, 0, 0, 0.1);\r\n"
		"        border-right: 1px solid #C1C3D1;\r\n"
		"    }\r\n"
		"    td:last-child {\r\n"
		"        border-right: 0px;\r\n"
		"    }\r\n"
		"    </style>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"    <main>\r\n"
		"        <div>\r\n"
		"        <div class=\"table-title\">STATE</div>\r\n"
		"            <table class=\"table-fill\">\r\n"
		"                <tbody class=\"table-hover table1\"></tbody>\r\n"
		"            </table>\r\n"
		"        </div>\r\n"
		"\r\n"
		"        <div>\r\n"
		"        <div class=\"table-title\">MEAS</div>\r\n"
		"            <table class=\"table-fill\">\r\n"
		"                <tbody class=\"table-hover table2\"></tbody>\r\n"
		"            </table>\r\n"
		"        </div>\r\n"
		"\r\n"
		"        <div>\r\n"
		"        <div class=\"table-title\">SETTING</div>\r\n"
		"            <table class=\"table-fill\">\r\n"
		"                <tbody class=\"table-hover table3\"></tbody>\r\n"
		"            </table>\r\n"
		"        </div>\r\n"
		"    </main>\r\n"
		"\r\n"
		"    <footer>\r\n"
        "        <div>\r\n"
		"        ©copyright: 2013-2017 Storozhenko Roman\r\n"
        "        </div>\r\n"
		"    </footer>\r\n"
		"    <script>\r\n"
		"    function nano(template, data) {\r\n"
		"        return template.replace(/\\{([\\w\.]*)\\}/g, function(str, key) {\r\n"
		"          var keys = key.split(\".\"), v = data[keys.shift()];\r\n"
		"          for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];\r\n"
		"          return (typeof v !== \"undefined\" && v !== null) ? v : \"\";\r\n"
		"        });\r\n"
		"    }\r\n"
		"\r\n"
		"    function updateTable(query, data) {\r\n"
		"        var table = document.querySelector(query);\r\n"
		"        table.innerHTML = '';\r\n"
		"        for(var key in data) {\r\n"
		"            table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);\r\n"
		"        }\r\n"
		"    }\r\n"
		"\r\n"
		"    function updateInfo() {\r\n"
		"        var oReq = new XMLHttpRequest();\r\n"
		"        oReq.open('GET', 'statemeastask.bin', true);\r\n"
		"        oReq.timeout = 5000;\r\n"
		"        oReq.responseType = 'arraybuffer';\r\n"
		"        oReq.onload = function (oEvent) {\r\n"
		"            if(oReq.status != 200) {\r\n"
		"                return;\r\n"
		"            }\r\n"
		"\r\n"
		"            var x = new DataView(oReq.response, 0);\r\n"
		"\r\n"
		"            // State\r\n"
		"            var state = x.getUint32(0, true);\r\n"
		"            updateTable('.table1', [\r\n"
		"                {name: 'current', val: (state & 1 << 0) ? 'ON' : 'OFF'},\r\n"
		"                {name: 'switch', val: (state & 1 << 1) ? '<p style=\"color:red;\">ON</p>' : '<p style=\"color:blue;\">OFF</p>'},\r\n"
		"                {name: 'output', val: (state & 1 << 2) ? 'CC' : 'CV'}\r\n"
		"            ]);\r\n"
		"\r\n"
		"            // Meas\r\n"
		"            updateTable('.table2', [\r\n"
		"                {name: 'power', val: x.getUint32(4, true) / 1000.0 + ' Wt'},\r\n"
		"                {name: 'resistens', val: x.getUint32(8, true) / 1000.0 + ' Ohm'},\r\n"
		"                {name: 'time', val: x.getUint32(12, true) + ' s'},\r\n"
		"                {name: 'capacity', val: x.getUint32(16, true) / 1000.0 + ' Ah'},\r\n"
		"                {name: 'u', val: x.getUint32(20, true) / 1000000.0 + ' V'},\r\n"
		"                {name: 'i', val: x.getUint32(24, true) / 1000000.0 + ' A'},\r\n"
		"                {name: 'uin', val: x.getUint16(32, true) / 1000.0 + ' V'},\r\n"
		"                {name: 'temperature', val: x.getUint16(34, true) / 10.0 + ' °C'}\r\n"
		"            ]);\r\n"
		"\r\n"
		"            // Settings\r\n"
		"            updateTable('.table3', [\r\n"
		"                {name: 'u', val: x.getUint32(36, true) / 1000000.0 + ' V'},\r\n"
		"                {name: 'i', val: x.getUint32(40, true) / 1000000.0 + ' A'},\r\n"
		"            ]);\r\n"
		"        };\r\n"
		"        oReq.send(null);\r\n"
		"    }\r\n"
		"    updateInfo();\r\n"
		"    setInterval(updateInfo, 2000);\r\n"
		"    </script>\r\n"
		"</body>\r\n"
		"</html>"
};

const char js_overall[] = {
		"function nano(template, data) {\r\n"
		"    return template.replace(/\{([\w\.]*)\}/g, function(str, key) {\r\n"
		"      var keys = key.split(\".\"), v = data[keys.shift()];\r\n"
		"      for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];\r\n"
		"      return (typeof v !== \"undefined\" && v !== null) ? v : \"\";\r\n"
		"    });\r\n"
		"}\r\n"
		"\r\n"
		"function updateTable(query, data) {\r\n"
		"    var table = document.querySelector(query);\r\n"
		"    table.innerHTML = '';\r\n"
		"    for(var key in data) {\r\n"
		"        table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);\r\n"
		"    }\r\n"
		"}\r\n"
		"\r\n"
		"function updateInfo() {\r\n"
		"    var oReq = new XMLHttpRequest();\r\n"
		"    oReq.open('GET', 'statemeastask.bin', true);\r\n"
		"    oReq.timeout = 5000;\r\n"
		"    oReq.responseType = 'arraybuffer';\r\n"
		"    oReq.onload = function (oEvent) {\r\n"
		"        if(oReq.status != 200) {\r\n"
		"            return;\r\n"
		"        }\r\n"
		"\r\n"
		"        var x = new DataView(oReq.response, 0);\r\n"
		"\r\n"
		"        // State\r\n"
		"        var state = x.getUint32(0, true);\r\n"
		"        updateTable('.table1', [\r\n"
		"            {name: 'current', val: (state & 1 << 0) ? 'ON' : 'OFF'},\r\n"
		"            {name: 'switch', val: (state & 1 << 1) ? '<p style=\"color:red;\">ON</p>' : '<p style=\"color:blue;\">OFF</p>'},\r\n"
		"            {name: 'output', val: (state & 1 << 2) ? 'CC' : 'CV'}\r\n"
		"        ]);\r\n"
		"\r\n"
		"        // Meas\r\n"
		"        updateTable('.table2', [\r\n"
		"	        {name: 'power', val: x.getUint32(4, true) / 1000.0 + ' Wt'},\r\n"
		"	        {name: 'resistens', val: x.getUint32(8, true) / 1000.0 + ' Ohm'},\r\n"
		"            {name: 'time', val: x.getUint32(12, true) + ' s'},\r\n"
		"            {name: 'capacity', val: x.getUint32(16, true) / 1000.0 + ' Ah'},\r\n"
		"            {name: 'u', val: x.getUint32(20, true) / 1000000.0 + ' V'},\r\n"
		"            {name: 'i', val: x.getUint32(24, true) / 1000000.0 + ' A'},\r\n"
		"            {name: 'uin', val: x.getUint16(32, true) / 1000.0 + ' V'},\r\n"
		"            {name: 'temperature', val: x.getUint16(34, true) / 10.0 + ' °C'}\r\n"
		"        ]);\r\n"
		"\r\n"
		"        // Settings\r\n"
		"        updateTable('.table3', [\r\n"
		"            {name: 'u', val: x.getUint32(36, true) / 1000000.0 + ' V'},\r\n"
		"            {name: 'i', val: x.getUint32(40, true) / 1000000.0 + ' A'},\r\n"
		"        ]);\r\n"
		"    };\r\n"
		"    oReq.send(null);\r\n"
		"}\r\n"
		"updateInfo();\r\n"
		"setInterval(updateInfo, 2000);"
};

const char html_404[] = {
	"<!DOCTYPE html>"
	"<html lang=\"en-US\" style=\"height: 100%;\">"
	"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
		"<title>Page Not Found (404)</title>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"        <style>\r\n"
"            table {\r\n"
"                border: 0px solid black;\r\n"
"                border-collapse: collapse;\r\n"
"                width: 200px;\r\n"
"                font: cursive\r\n"
"            }\r\n"
"            body {\r\n"
"                background: #002B36;\r\n"
"                color: #36D695;\r\n"
"            }\r\n"
"        </style>\r\n"
	"</head>"
	"<body>"
	"<div align=\"center\">"
		"<p>"
			"<font face=\"Cursive\" size=\"7\">404 </font>"
			"<font face=\"Cursive\" size=\"6\">error</font>"
		"</p>"
		"<hr align=\"center\" width=\"70%\" size=\"1\" color=\"#36D695\" />"
		"<p><font face=\"Cursive\" size=\"6\">Page not found</font></p>"
	"</div>"
	"</body>"
	"</html>"
	"\r\n"
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

meas_type bin_meas = {
	.power			= 1,           		///< [X_XXX Wt]
	.resistens		= 2,      			///< [X_XXX Ohm]
	.time			= 3,           		///< [s]
	.capacity		= 25456,       		///< [X_XXX A/h]
	.u				= 22220000,       	///< [X_XXXXXX V]
	.i				= 7900000,      	///< [X_XXXXXX A]
	.adcu			= 15,           	///< [LSB]
	.adci			= 16,          		///< [LSB]
	.uin			= 38000,           	///< [X_XXX V]
	.temperatureLin	= 256, 				///< [X_X °С]
};

transfer_type bin_statemeastask;

const url_type getUrlTable[] = {
	{
		.url = "/ ",
		.handler = NULL,
		.data.type = urlDataTypeHtml,
		.data.data.html = html_index,
		.data.size = sizeof(html_index) - 1,
	},
	{
		.url = "/overall.js",
		.handler = NULL,
		.data.type = urlDataTypeBin,
		.data.data.bin = js_overall,
		.data.size = sizeof(js_overall) - 1,
	},
	{
		.url = "/reset.css",
		.handler = NULL,
		.data.type = urlDataTypeBin,
		.data.data.bin = css_reset,
		.data.size = sizeof(css_reset) - 1,
	},
	{
		.url = "/styles.css",
		.handler = NULL,
		.data.type = urlDataTypeBin,
		.data.data.bin = css_styles,
		.data.size = sizeof(css_styles) - 1,
	},
	{
		.url = "/statemeastask.bin",
		.handler = handle_statemeastask,
		.data.type = urlDataTypeBin,
		.data.size = sizeof(bin_statemeastask),
	},
	{
		.url = "/favicon.ico",
		.handler = NULL,
		.data.type = urlDataTypeBin,
		.data.data.bin = bin_favicon,
		.data.size = sizeof(bin_favicon),
	},
	{
		.url = "/404",
		.handler = NULL,
		.data.type = urlDataTypeHtml,
		.data.data.html = html_404,
		.data.size = sizeof(html_404) - 1,
	},
};

const uint8_t getUrlNumber = sizeof(getUrlTable) / sizeof(url_type);

/*!****************************************************************************
 *
 */
urlData_type handle_statemeastask(void){
	static uint16_t t;

	static urlData_type urlData;
	memcpy(&bin_statemeastask, &fp.tf, sizeof(transfer_type));
	bin_statemeastask.meas.time = t++;
	bin_statemeastask.meas.temperatureLin = 266;
	bin_statemeastask.state.bit.switchIsON ^= 1;

	urlData.data.bin = &bin_statemeastask;
	urlData.size = sizeof(transfer_type);
	urlData.type = urlDataTypeBin;
	return urlData;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
