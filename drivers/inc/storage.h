/*!****************************************************************************
 * @file		storage.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		17.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		System of storage data to non-volatile memory
 */
#ifndef storage_H
#define storage_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "crc.h"
#include "24AAxx.h"
#include "bkp.h"
#include "startupTSK.h"
#include "baseTSK.h"
#include "chargeTSK.h"

/*!****************************************************************************
 * Define
 */
#define USE_STATIC_BUFF     (1)
#define SIZE_BUFF           (128)
//#define nvMemBaseAdr        ((void*)(0))	///< Адрес в памяти для хранения данных
//#define nvMemResAdr         ((void*)(512))	///< Адрес в памяти для хранения резервной копии данных
#define nvMemSignature      (0x2805)		///< Используемая сигнатура пакета (любая 16 бит константа)

//Перенаправление ф-й
#define nvMem_prepareMemory(pDst, len)
#define nvMem_write(pDst, pSrc, len)    eep_write((uint32_t)pDst, pSrc, len)
#define nvMem_read(pDst, pSrc, len)     eep_read(pDst, (uint32_t)pSrc, len)
#define nvMem_malloc(len)               malloc(len)
#define nvMem_free(pSrc)                free(pSrc)

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	nvMem_ok,
	nvMem_AddrIsNULL,
	nvMem_signatureError,
	nvMem_CRCError,
	nvMem_ErrorSizeMem,
	nvMem_WriteError,
	nvMem_ReadError,
	nvMem_error
} nvMem_state_type;

typedef enum {
	internalFlash,
	i2cEeprom,
	backupRegs,
} nvMem_memType_type;

typedef struct {
	void 		*dataPtr;
	uint32_t 	sizeofData;
} nvMem_memreg_type;

typedef union {
	struct {
		uint16_t prepareForWrite :1;
	} bit;
	uint16_t all;
} nvMem_flags_type;

typedef struct {
	uint16_t 			numPrm;
	nvMem_memreg_type 	*memreg;
	void 				*nvMemBase;
	void 				*nvMemBaseResrvCopy;
	nvMem_flags_type 	flags;
	uint32_t 			fullSize;
	uint32_t 			saveCnt;
} nvMem_struct_type;

typedef struct {
	void		*dataPtr;
	uint32_t	sizeofData;
} nvMem_dataDescriptor_type;

typedef struct {
	const nvMem_dataDescriptor_type *data;
	uint32_t 						numPrm;
	void 							*nvMemBase;
	uint32_t						size;
	uint32_t						saveCnt;
	nvMem_memType_type				memType;
} nvMem_partition_type;

/*!****************************************************************************
 * Exported variables
 */
extern nvMem_partition_type userConfRegion;
extern nvMem_partition_type userReservConfRegion;
extern nvMem_partition_type systemSettingRegion;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
nvMem_state_type nvMem_init(nvMem_partition_type *part);
nvMem_state_type nvMem_savePrm(nvMem_partition_type *part);
nvMem_state_type nvMem_loadPrm(nvMem_partition_type *part);

#endif //storage_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/
