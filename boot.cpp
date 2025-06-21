/******************************************************************************
 * @file		boot.cpp                                                      *
 * @author		Storozhenko Roman - D_EL                                      *
 * @version		V1.0                                                          *
 * @date		13.02.2025                                                    *
 * @brief		Bootloader                                                    *
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman   *
******************************************************************************/

/*!****************************************************************************
* Include
*/
#include <string.h>
#include <clock.h>
#include <flash.h>
#include <crc32.h>
#include <cm4.h>
#include <imageheader.h>

static bool checkProgram(const uint8_t* addr);

/*!****************************************************************************
* @brief    main function
*/
int main(void){
	clock_init();
	crc32_init();

	const uint8_t* main_flash_start = getImageStartAddress();
	const uint8_t* fwstorage_flash_start = getStorageStartAddress();
	size_t fwstorage_flash_size = getStorageSize();

	const imageHeader_t* imageHeader = (imageHeader_t*)main_flash_start;
	const imageHeader_t* storageImageHeader = (imageHeader_t*)fwstorage_flash_start;

	bool upgrade = false;

	if(checkProgram(fwstorage_flash_start)){
		if(checkProgram(main_flash_start)){
			if(imageHeader->versionMajor != storageImageHeader->versionMajor ||
				imageHeader->versionMinor != storageImageHeader->versionMinor ||
				imageHeader->versionPach != storageImageHeader->versionPach ||
				imageHeader->valueCRC != storageImageHeader->valueCRC){
				upgrade = true;
			}
		}else{
			upgrade = true;
		}
	}

	if(upgrade){
		flash_unlock();

		for(const uint8_t*  p = main_flash_start; p < fwstorage_flash_start; p += FLASH_PAGE_SIZE){
			flash_erasePage((void*)p);
		}

		flash_write((void*)main_flash_start, (void*)fwstorage_flash_start, storageImageHeader->size);

		if(checkProgram(main_flash_start)){ // Verify main firmware
			for(const uint8_t*  p = fwstorage_flash_start; p < fwstorage_flash_start + fwstorage_flash_size; p += FLASH_PAGE_SIZE){
				flash_erasePage((void*)p);	// Erase page
			}
		}

		flash_lock();
	}

	if(checkProgram(main_flash_start)){
		uint32_t* exe = (uint32_t*)(&(main_flash_start)[imageHeader->exeOffset]);
		cm4_executeprogram(exe);
	}
	while(1);
	return 0;
}

/*!****************************************************************************
 * @brief
 */
static bool checkProgram(const uint8_t* addr){
	const imageHeader_t* imageHeader = (imageHeader_t*)addr;
	if(imageHeader->magic != getHeaderMagic()){
		return false;
	}
	if(imageHeader->size == 0){
		return false;
	}
	const size_t skipbytes = 4/*CRC*/ + 4/*Size*/;
	uint8_t buffer[skipbytes];
	memset(buffer, 0, skipbytes);
	uint32_t crc = crc32_posix(buffer, sizeof(buffer), true);
	crc = crc32_posix(addr + skipbytes, imageHeader->size - skipbytes, false);
	return imageHeader->valueCRC == crc;
}

/******************************** END OF FILE ********************************/
