/*!****************************************************************************
 * @file		storage.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		17.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		System of storage data to non-volatile memory
 */

/*!****************************************************************************
 * Include
 */
#include "storage.h"

/*!****************************************************************************
 * Memory
 */
/*
 * - 0000 -----------------------------------
 * ------------------------------------------
 * ------ Пользовательские настройки
 * ------------------------------------------
 * - 0255 -----------------------------------

 * - 0256 -----------------------------------
 * ------------------------------------------
 * ------ Пользовательские настройки резервная копия
 * ------------------------------------------
 * - 0511 -----------------------------------

 * - 0512 -----------------------------------
 * ------------------------------------------
 * ------ Системные данные
 * ------------------------------------------
 * ------------------------------------------
 * ------------------------------------------
 * - 1023 -----------------------------------
 */

#if (USE_STATIC_BUFF > 0)
uint8_t nvMemTmpBff[SIZE_BUFF];
#endif

//! User setting data
const nvMem_dataDescriptor_type nvMem_userConfDescriptor[] = {
	{ &bs, 				sizeof(bs) },
	{ &ch, 				sizeof(ch) },
	{ &startCounter,	sizeof(startCounter) },
};
nvMem_partition_type userConfRegion = {
	.memType = i2cEeprom,
	.data = nvMem_userConfDescriptor,
	.numPrm = sizeof(nvMem_userConfDescriptor) / sizeof(nvMem_dataDescriptor_type),
	.nvMemBase = (void*) 0
};

//! User setting data reserved
nvMem_partition_type userReservConfRegion = {
	.memType = i2cEeprom,
	.data = nvMem_userConfDescriptor,
	.numPrm = sizeof(nvMem_userConfDescriptor) / sizeof(nvMem_dataDescriptor_type),
	.nvMemBase = (void*) 256
};

//! System setting data
const nvMem_dataDescriptor_type nvMem_systemSettingDescriptor[] = {
		{ &fp.fpSet, sizeof(frontPanelSetting_type) },
};
nvMem_partition_type systemSettingRegion = {
	.memType = i2cEeprom,
	.data = nvMem_systemSettingDescriptor,
	.numPrm = sizeof(nvMem_systemSettingDescriptor)/ sizeof(nvMem_dataDescriptor_type),
	.nvMemBase = (void*) 512
};

/*!****************************************************************************
 * Local prototypes for the functions
 */
nvMem_state_type nvMem_readMem(void *dst, void *src, uint32_t len, nvMem_memType_type memType);
nvMem_state_type nvMem_writeMem(void *dst, void *src, uint32_t len, nvMem_memType_type memType);

/*!****************************************************************************
 * @brief    Инициализация системы хранения
 */
nvMem_state_type nvMem_init(nvMem_partition_type *part){
	part->size = 0;

	for(uint16_t i = 0; i < part->numPrm; i++){
		part->size += part->data[i].sizeofData;
	}
	part->size += sizeof(part->saveCnt);
	part->size += sizeof(uint16_t);         //signature
	part->size += sizeof(uint16_t);         //CRC

#if (USE_STATIC_BUFF > 0)
	if(part->size > sizeof(nvMemTmpBff)){
		while(1)
			__NOP(); //Error
	}
#endif
	return nvMem_ok;
}

/*!****************************************************************************
 * @brief    Сохранение всех зарегистрированых объектов в память
 * @param    adrNvMem - адрес назначения в flash памяти
 */
nvMem_state_type nvMem_savePrm(nvMem_partition_type *part){
	uint8_t *pBuf;
	uint8_t *pData;

#if (USE_STATIC_BUFF > 0)
	pBuf = nvMemTmpBff;
#else
	pBuf = (uint8_t*)nvMem_malloc(part->size);
#endif
	pData = pBuf;

	//Соединяем все в один массив
	//Signature
	*(uint16_t*) pData = nvMemSignature;
	pData += sizeof(uint16_t);
	//saveCnt
	part->saveCnt++;
	*(uint16_t*) pData = part->saveCnt;
	pData += sizeof(part->saveCnt);
	//memreg
	for(uint16_t i = 0; i < part->numPrm; i++){
		memcpy(pData, part->data[i].dataPtr, part->data[i].sizeofData);
		pData += part->data[i].sizeofData;
	}
	//CRC
	*(uint16_t*) pData = GetCrc(pBuf, pData - pBuf);
	pData += sizeof(uint16_t);

	//Write to memory
	if(nvMem_writeMem(part->nvMemBase, pBuf, pData - pBuf, part->memType) != nvMem_ok){
		part->saveCnt--;
		return nvMem_WriteError;
	}

	//Сбросить флаг подготовленности памяти
	//nvMem.flags.bit.prepareForWrite = 0;
#if (USE_STATIC_BUFF == 0)
	nvMem_free(pBuf);
#endif
	return nvMem_ok;
}

/*!****************************************************************************
 * @brief    Загрузка всех зарегистрированых объектов из памяти
 * @param    adrNvMem - адрес источника в flash памяти
 * @retval   - nvMem_ok
 *           - nvMem_CRCError
 *           - nvMem_signatureError
 */
nvMem_state_type nvMem_loadPrm(nvMem_partition_type *part){
	uint8_t *pBuf;
	uint8_t *pData;
	uint16_t l_crc, l_signature;

#if (USE_STATIC_BUFF > 0)
	pBuf = nvMemTmpBff;
#else
	pBuf = (uint8_t*)nvMem_malloc(part->size);
#endif
	pData = pBuf;

	//Читаем память
	if(nvMem_readMem(pBuf, part->nvMemBase, part->size, part->memType) != nvMem_ok){
		return nvMem_ReadError;
	}

	//Проверить CRC
	l_crc = GetCrc(pBuf, part->size);

	if(l_crc == 0){ //CRC OK
		l_signature = *(uint16_t*) pData;
		if(l_signature != nvMemSignature){  //signature error
#if (USE_STATIC_BUFF == 0)
			nvMem_free(pBuf);
#endif
			return nvMem_signatureError;
		}
		pData += sizeof(l_signature);
		part->saveCnt = *(uint16_t*) pData;
		pData += sizeof(part->saveCnt);

		for(uint16_t i = 0; i < part->numPrm; i++){
			memcpy(part->data[i].dataPtr, pData, part->data[i].sizeofData);
			pData += part->data[i].sizeofData;
		}
#if (USE_STATIC_BUFF == 0)
		nvMem_free(pBuf);
#endif
		return nvMem_ok;
	}else{   //CRC ERROR
#if (USE_STATIC_BUFF == 0)
		nvMem_free(pBuf);
#endif
		return nvMem_CRCError;
	}
}

/*!****************************************************************************
 * @brief    Read memory
 * @param
 * @retval   - nvMem_ok, nvMem_ReadError
 */
nvMem_state_type nvMem_readMem(void *dst, void *src, uint32_t len, nvMem_memType_type memType){
	switch(memType){
		case (internalFlash):
			memcpy(dst, src, len);
			break;
		case (i2cEeprom):
			/*if(eep_read(dst, (uint32_t) src, len) != eepOk){
				return nvMem_ReadError;
			}*/
			break;
		case (backupRegs):
			/*if(bkp_read(dst, (uint32_t) src, len) != bkpOk){
				return nvMem_ReadError;
			}*/
			break;
	}
	return nvMem_ok;
}

/*!****************************************************************************
 * @brief    Write memory
 * @param
 * @retval   - nvMem_ok, nvMem_WriteError
 */
nvMem_state_type nvMem_writeMem(void *dst, void *src, uint32_t len, nvMem_memType_type memType){
	switch(memType){
		case (internalFlash):
			//Not use in this project
			return nvMem_WriteError;
			break;
		case (i2cEeprom):
			/*if(eep_write((uint32_t) dst, src, len) != eepOk){
				return nvMem_WriteError;
			}*/
			break;
		case (backupRegs):
			/*if(bkp_write((uint32_t) dst, src, len) != bkpOk){
				return nvMem_WriteError;
			}*/
			break;
	}
	return nvMem_ok;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
