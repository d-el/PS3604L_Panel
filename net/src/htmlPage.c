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
 * MEMORY
 */
const char htmlStart[] = {
	"<!DOCTYPE html>"
	"<html lang=\"en-US\" style=\"height: 100%;\">"
	"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
		"<meta http-equiv=\"Refresh\" content=\"1\" />"
		"<title>PS3604L</title>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
	"</head>"
	"<body>"

		/*"<br><br>"
		"<div id='content' style=\"height: 50px; width: 150px; font-size: 12pt; border: 1px solid #999; background-color: #B0E0E6;\" >"
		"<p>©2013-2017 Storozhenko Roman</p>"
		"<br><br>"
	"</body>"*/
};

const char htmlEnd[] = {
		"<br><br>"
		"<div id=\"content\" style=\"text-align: center; height: 50px; width: 300px; font-size: 12pt; border: 1px solid #999; background-color: #B0E0E6;\">"
		"<p>©2013-2017 Storozhenko Roman</p>"
		"<br><br>"
	"</body>"
};

typedef struct{
	uint32_t ip;
	uint32_t cnt;
	uint32_t state;
}ipcnt_type;
ipcnt_type	ipcnt[10];

char table2[] = {
	"<style>"
		"table, th, td {"
			"border: 1px solid black;"
			"border-collapse: collapse;"
		"}"
	"</style>"
	"<table style=\"width:300px\">"
		/*"<tr>"
			"<th>Firstname</th>"
			"<th>Lastname</th> "
		"</tr>"
		"<tr>"
			"<td>Jill</td>"
			"<td>Smith</td>"
		"</tr>"
	"</table>"*/
};

/*!****************************************************************************
 *
 */
void table2InsertRowHeader(char *bff, char *s1, char *s2){
	/*
	"<tr>"
		"<th>Firstname</th>"
		"<th>Lastname</th> "
	"</tr>"
	*/

	strcat(bff, "<tr><th>");
	strcat(bff, s1);
	strcat(bff, "</th><th>");
	strcat(bff, s2);
	strcat(bff, "</th></tr>");
}

/*!****************************************************************************
 *
 */
void table2InsertWideHeader(char *bff, char *s){
	/*
	<tr>
		<th colspan="3">Telephone</th>
	</tr>
	*/

	strcat(bff, "<tr><th colspan=\"2\">");
	strcat(bff, s);
	strcat(bff, "</th></tr>");
}

/*!****************************************************************************
 *
 */
void table2InsertRow(char *bff, char *s1, char *s2){
	/*
	"<tr>"
		"<td>Jill</td>"
		"<td>Smith</td>"
	"</tr>"
	*/

	strcat(bff, "<tr><td>");
	strcat(bff, s1);
	strcat(bff, "</td><td>");
	strcat(bff, s2);
	strcat(bff, "</td></tr>");
}

void table2InsertEnd(char *bff){
	strcat(bff, "</table>");
}

/*!****************************************************************************
 * @param data
 */
void makeTableSet(char *data){
	char str[16];

	strcat(data, table2);
	table2InsertWideHeader(data, "Set parameter");

	sprintf(str, "%u.%03u V", fp.tf.task.u / 1000000, (fp.tf.task.u / 1000) % 1000);
	table2InsertRow(data, "Voltage", str);

	sprintf(str, "%u.%03u A", fp.tf.task.i / 1000000, (fp.tf.task.i / 1000) % 1000);
	table2InsertRow(data, "Current limitation", str);

	table2InsertEnd(data);
}

/*!****************************************************************************
 * @param data
 */
void makeTableMeas(char *data){
	char str[16];

	strcat(data, table2);
	table2InsertWideHeader(data, "Meas parameter");

	sprintf(str, "%u.%03u V", fp.tf.meas.u / 1000000, (fp.tf.meas.u / 1000) % 1000);
	table2InsertRow(data, "Voltage", str);

	sprintf(str, "%u.%03u A", fp.tf.meas.i / 1000000, (fp.tf.meas.i / 1000) % 1000);
	table2InsertRow(data, "Current", str);

	sprintf(str, "%u.%03u W", fp.tf.meas.power / 1000, fp.tf.meas.power % 1000);
	table2InsertRow(data, "Power", str);

	if(fp.tf.meas.resistens < 99999){
		sprintf(str, "%05u Ohm", fp.tf.meas.resistens);
	}else{
		strcpy(str, "---");
	}
	table2InsertRow(data, "Resistens", str);

	sprintf(str, "%u.%u °С", fp.tf.meas.temperatureLin / 10, fp.tf.meas.temperatureLin % 10);
	table2InsertRow(data, "Temperature", str);

	switch(fp.tf.task.mode){
		case mode_off:					//!< mode_off
			table2InsertRow(data, "Mode", "off");
			break;
		case mode_overcurrentShutdown:	//!< mode_overcurrentShutdown
			table2InsertRow(data, "Mode", "overcurrentShutdown");
						break;
		case mode_limitation:         	//!< mode_limitation
			table2InsertRow(data, "Mode", "limitation");
						break;
		case mode_timeShutdown:       	//!< mode_timeShutdown
			table2InsertRow(data, "Mode", "timeShutdown");
						break;
		case mode_lowCurrentShutdown: 	//!< mode_lowCurrentShutdown
			table2InsertRow(data, "Mode", "lowCurrentShutdown");
						break;
		case mode_Uadj:                 //!< mode_Uadj
			table2InsertRow(data, "Mode", "Uadj");
						break;
		case mode_Iadj:                 //!< mode_Iadj
			table2InsertRow(data, "Mode", "Iadj");
						break;
		case mode_raw:
			table2InsertRow(data, "Mode", "raw");
						break;
		default:
			table2InsertRow(data, "Mode", "error");
	}

	if(fp.tf.state.bit.modeIlim != 0){
		table2InsertRow(data, "Limitation", "ON");
	}else{
		table2InsertRow(data, "Limitation", "OFF");
	}

	if(fp.tf.state.bit.switchIsON != 0){
		table2InsertRow(data, "Swich state", "ON");
	}else{
		table2InsertRow(data, "Swich state", "OFF");
	}

	table2InsertEnd(data);
}

/*!****************************************************************************
 *
 */
ipcnt_type *ipFind(uint32_t ip){
	for(uint32_t i = 0; i < sizeof(ipcnt) / sizeof(ipcnt[0]); i++){
		if((ip == ipcnt[i].ip) && (ipcnt[i].state != 0)){
			return &ipcnt[i];
		}
	}
	return NULL;
}

/*!****************************************************************************
 *
 */
void ipInsert(uint32_t ip){
	for(uint32_t i = 0; i < sizeof(ipcnt) / sizeof(ipcnt[0]); i++){
		if(ipcnt[i].state == 0){
			ipcnt[i].ip = ip;
			ipcnt[i].cnt = 1;
			ipcnt[i].state = 1;
			return;
		}
	}
}

/*!****************************************************************************
 *
 */
void sendHtmlPage(char *data){
	strcat(data, htmlStart);
	makeTableSet(data);
	strcat(data, "<br>");
	makeTableMeas(data);

	//strcat(data, makeTableIp(conn->pcb.ip->remote_ip.addr));
	//rtc_getTime(&timeStrct);
	strcat(data, htmlEnd);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
