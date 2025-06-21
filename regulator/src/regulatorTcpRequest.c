/*!****************************************************************************
 * @file		httpServerTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		HTTP Server
 */

/*!****************************************************************************
 * Include
 */
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "regulatorConnTSK.h"
#include <plog.h>

#define LOG_LOCAL_LEVEL P_LOG_INFO

/*!****************************************************************************
 * MEMORY
 */
static char *logTag = "reg_tcpModbusRequest";

typedef struct __attribute__((packed)){
	uint16_t transactionIdentifier;
	uint16_t ProtocolIdentifier;
	uint16_t Length;
	uint8_t UnitIdentifier;
}MBAPheader_t;

typedef struct __attribute__((packed)){
	MBAPheader_t MBAPheader;
	uint8_t PDU[253/*PDU*/ + 2/*reserve expansion for CRC*/];
}tcpModbusPacket_t;

tcpModbusPacket_t tcpModbusPacket;

bool reg_tcpModbusRequest(void** readdata, uint16_t* buflen){
	reg_setremote(true);
	if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
		plog_hexdumpcolumn(*readdata, *buflen, 32);
	}
	memcpy(&tcpModbusPacket, *readdata, *buflen <= sizeof(tcpModbusPacket_t) ? *buflen : 0);

	uint16_t len = __builtin_bswap16/*ntoh*/(tcpModbusPacket.MBAPheader.Length);
	if(reg_modbusRequest((uint8_t*)&tcpModbusPacket.MBAPheader.UnitIdentifier, &len)){
		size_t lentowrite = 6 + len;
		tcpModbusPacket.MBAPheader.Length = __builtin_bswap16(len);
		P_LOGD(logTag, "send response to client (%"PRIu16")", lentowrite);
		if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
			plog_hexdumpcolumn(&tcpModbusPacket, lentowrite, 32);
		}
		len = __builtin_bswap16(len); // hton
		*readdata = (void*)&tcpModbusPacket;
		*buflen = lentowrite;
	}
	return true;
}

bool reg_tcpModbusClosedConnection(void){
	reg_setremote(false);
	return true;
}

/******************************** END OF FILE ********************************/
