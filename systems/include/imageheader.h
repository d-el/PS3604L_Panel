/*
 * imageheader.h
 *
 *  Created on: Mar 12, 2021
 *      Author: del
 */

#ifndef IMAGEHEADER_H
#define IMAGEHEADER_H

#include <stdint.h>

typedef struct imageHeader{
	uint32_t valueCRC;
	uint32_t size; // Size image include this header
	uint32_t magic;
	uint32_t versionMajor;
	uint32_t versionMinor;
	uint32_t versionPach;
	uint32_t exeOffset; // Offset execute from start image
}imageHeader_t;

extern uint32_t _header_magic;
extern uint8_t _main_flash_start[];				/// See memory.ld
extern uint8_t _main_flash_size;
extern uint8_t _fwstorage_flash_start[];		/// See memory.ld
extern uint8_t _fwstorage_flash_size;			/// See memory.ld

static inline uint32_t getHeaderMagic(void){
	return (uint32_t)0x38272585;
}

static inline const imageHeader_t* getImageHeader(void){
	return (imageHeader_t*)&_main_flash_start;
}

static inline const uint8_t* getImageStartAddress(void){
	return (uint8_t*)&_main_flash_start;
}

static inline intptr_t getImageSize(void){
	return (intptr_t)&_main_flash_size;
}

static inline const imageHeader_t* getStorageHeader(void){
	return (imageHeader*)&_fwstorage_flash_start;
}

static inline const uint8_t* getStorageStartAddress(void){
	return (uint8_t*)&_fwstorage_flash_start;
}

static inline intptr_t getStorageSize(void){
	return (intptr_t)&_fwstorage_flash_size;
}

#endif /* IMAGEHEADER_H */
