/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include <FreeRTOS.h>
#include <task.h>
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include <prmSystem.h>
#include <plog.h>
#include <flash.h>
#include <imageheader.h>
#include <regulatorConnTSK.h>

#define LOG_LOCAL_LEVEL P_LOG_INFO
static const char *logTag = "ModBusTCP";
uint32_t numberRequest;
struct netconn *conn;
struct netconn *newconn;

/*!****************************************************************************
 * @brief
 */
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode){
	usAddress--;
	switch(eMode){
		/* Pass current register values to the protocol stack. */
		case MB_REG_READ:{
			Prm::IVal *ph = Prm::getbyaddress(usAddress);
			while(usNRegs > 0){
				if(ph == nullptr){
					P_LOGW(logTag, "r: illegal register address [%04X], regs %u", usAddress, usNRegs);
					return MB_ENOREG;
				}

				auto prmsize = ph->getsize();
				if(prmsize == 4 && usNRegs < 2){
					P_LOGW(logTag, "r: [%04X] usNRegs < 2 in 4 Byte parameter", usAddress);
					return MB_EINVAL;
				}

				(*ph)(true, nullptr);

				if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
					char string[32];
					ph->tostring(string, sizeof(string));
					P_LOGD(logTag, "r: [%04X] %u %s: %s %s", usAddress, usNRegs, ph->getlabel(), string, ph->getunit());
				}

				uint8_t buffer[4] = {};
				ph->serialize(buffer);
				switch(prmsize){
					case 1:
					case 2:
						*pucRegBuffer++ = buffer[1];
						*pucRegBuffer++ = buffer[0];
						usAddress++;
						usNRegs--;
						break;
					case 4:
						*pucRegBuffer++ = buffer[1];
						*pucRegBuffer++ = buffer[0];
						*pucRegBuffer++ = buffer[3];
						*pucRegBuffer++ = buffer[2];
						usAddress += 2;
						usNRegs -= 2;
						break;
				}
				ph = Prm::getNext();
			}
		}
		break;

		/* Update current register values with new values from the protocol stack. */
		case MB_REG_WRITE:{
			Prm::IVal *ph = Prm::getbyaddress(usAddress);
			while(usNRegs > 0){
				if(ph == nullptr){
					P_LOGW(logTag, "w: illegal register address [%04X]", usAddress);
					return MB_ENOREG;
				}

				auto prmsize = ph->getsize();
				if(prmsize == 4 && usNRegs < 2){
					P_LOGW(logTag, "w: [%04X] usNRegs < 2 in 4 Byte parameter", usAddress);
					return MB_EINVAL;
				}

				uint8_t buffer[4];
				switch(prmsize){
					case 1:
					case 2:
						buffer[1] = *pucRegBuffer++;
						buffer[0] = *pucRegBuffer++;
						usAddress++;
						usNRegs--;
						break;

					case 4:
						buffer[1] = *pucRegBuffer++;
						buffer[0] = *pucRegBuffer++;
						buffer[3] = *pucRegBuffer++;
						buffer[2] = *pucRegBuffer++;
						usAddress += 2;
						usNRegs -= 2;
						break;
				}

				if(!ph->deserialize(buffer)){
					P_LOGW(logTag, "w [%04X]: out of range", usAddress);
					return MB_EINVAL;
				}

				(*ph)(false, nullptr);

				if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
					char string[32];
					ph->tostring(string, sizeof(string));
					P_LOGD(logTag, "w: [%04X] %u %s: %s %s", usAddress, usNRegs, ph->getlabel(), string, ph->getunit());
				}
				ph = Prm::getNext();
			}
		}
		break;
	}
	return MB_ENOERR;
}

/*!****************************************************************************
 * @brief
 */
eMBErrorCode eMBFileRecordCB(UCHAR* pucDataBuffer, USHORT usFile, USHORT usRecord, USHORT usLen, eMBRegisterMode eMode){
	(void)usFile;
	P_LOGD(logTag, "usRecord %u", usRecord);
	size_t offset = usRecord * 128;
	const uint8_t* fwstorage_flash_start = getStorageStartAddress();
	size_t fwstorage_flash_size = getStorageSize();

	if(usFile != 1){
		return MB_EINVAL;
	}
	if(offset >= fwstorage_flash_size){
		return MB_EINVAL;
	}

	if(eMode == MB_REG_WRITE){
		uint8_t m[128];
		for(size_t i = 0; i < usLen; i++){
			m[i*2 + 0] = *pucDataBuffer++;
			m[i*2 + 1] = *pucDataBuffer++;
		}
		flash_unlock();
		const uint32_t* pFlash = (uint32_t*)&fwstorage_flash_start[offset];
		flash_write((void*)pFlash, (void*)m, usLen * 2);
		flash_lock();
	}
	return MB_ENOERR;
}

/*!****************************************************************************
 * @brief
 */
BOOL xMBTCPPortInit(USHORT usTCPPort){
	// Check and erase firmware storage
	const uint32_t* pFlash = (uint32_t*)getStorageStartAddress();
	const uint32_t* pFlashEnd = pFlash + getStorageSize() / sizeof(uint32_t);
	for(; pFlash < pFlashEnd; pFlash += FLASH_PAGE_SIZE / sizeof(uint32_t)){
		for(size_t i = 0; i < FLASH_PAGE_SIZE / sizeof(uint32_t); i++){
			if(pFlash[i] != 0xFFFFFFFF){
				P_LOGI(logTag, "Erase sector, address %p", pFlash);
				flash_unlock();
				flash_erasePage((void*)pFlash);
				flash_lock();
				break;
			}
		}
	}

	P_LOGD(logTag, "Wait network");
	while(!netif_is_link_up(netif_default) && !netif_ip4_addr(netif_default)->addr){
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	P_LOGD(logTag, "Create a new TCP connection handle");
	conn = netconn_new(NETCONN_TCP);
	if(conn == NULL){
		P_LOGW(logTag, "Error create netconn");
		return FALSE;
	}

	/* Bind to port with default IP address */
	err_t err = netconn_bind(conn, NULL, usTCPPort);
	if(err != ERR_OK){
		P_LOGW(logTag, "Error netconn bind");
		return FALSE;
	}

	err = netconn_listen(conn);
	if(err != ERR_OK){
		P_LOGW(logTag, "Error listen");
		return FALSE;
	}

	return TRUE;
}

/*!****************************************************************************
 * @brief
 */
void vMBTCPPortClose(void){
	P_LOGE(logTag, "'vMBTCPPortClose' not implemented");
}

/*!****************************************************************************
 * @brief
 */
void vMBTCPPortDisable(void){
	P_LOGE(logTag, "'vMBTCPPortDisable' not implemented");
}

/*!****************************************************************************
 * @brief
 */
BOOL xMBPortTCPPoll(void){
	P_LOGD(logTag, "PortTCPPoll");
	if(newconn == nullptr){
		P_LOGD(logTag, "Accept any icoming connection");
		err_t err = netconn_accept(conn, &newconn);
		if(err != ERR_OK){
			newconn = nullptr;
			return FALSE;
		}
		else{
			P_LOGI(logTag, "Connection, Remote IP address: %s", ipaddr_ntoa(&newconn->pcb.ip->remote_ip));
			netconn_set_recvtimeout(conn, 1000/*ms*/);
			xMBPortEventPost(EV_FRAME_RECEIVED);
		}
	}
	xMBPortEventPost(EV_FRAME_RECEIVED);
	return TRUE;
}

/*!****************************************************************************
 * @brief
 */
BOOL xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT *usTCPLength){
	P_LOGD(logTag, "PortGetRequest");
	if(newconn){
		struct netbuf *inbuf;
		err_t res = netconn_recv(newconn, &inbuf);
		if(res == ERR_OK){
			uint8_t *readdata = NULL;
			u16_t buflen;
			netbuf_data(inbuf, (void**)&readdata, &buflen);

			if(readdata[6 /*UnitIdentifier*/] == 2){
				P_LOGD(logTag, "Panel");
				*ppucMBTCPFrame = readdata;
				*usTCPLength = buflen;
			}else{
				P_LOGD(logTag, "Regulator");
				reg_tcpModbusRequest((void**)&readdata, &buflen);
				xMBTCPPortSendResponse(readdata, buflen);
				*ppucMBTCPFrame = nullptr;
				*usTCPLength = 0;
			}
			/* Delete the buffer (netconn_recv gives us ownership,
			 so we have to make sure to deallocate the buffer) */
			netbuf_delete(inbuf);

		}else if(res == ERR_TIMEOUT){
			bool up = netif_is_link_up(netif_default) ? true : false;
			if(!up){
				P_LOGW(logTag, "Read timeout, link %s", up ? "Up" : "Down");
				reg_tcpModbusClosedConnection();
				return FALSE;
			}
		}
		else{
			P_LOGW(logTag, "Error in netconn_recv (%i), %s", res, lwip_strerr(res));
			netconn_delete(newconn);
			newconn = nullptr;
			reg_tcpModbusClosedConnection();
			return FALSE;
		}
	}

	return TRUE;
}

/*!****************************************************************************
 * @brief
 */
BOOL xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength){
	P_LOGD(logTag, "SendResponse");
	if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
		plog_hexdumpcolumn(pucMBTCPFrame, usTCPLength, 32);
	}
	err_t res = netconn_write(newconn, pucMBTCPFrame, usTCPLength, NETCONN_NOCOPY);
	if(res != ERR_OK){
		P_LOGW(logTag, "Write response fail, %i", res);
	}
	return FALSE;
}

/******************************** END OF FILE ********************************/
