/*!*****************************************************************************
 * @file		prmSystem.cpp
 * @author		d_el - Storozhenko Roman
 * @version		V2.3
 * @date		05.11.2024
 * @copyright	The MIT License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief		Parameters system
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <crc.h>
#include "prmSystem.h"
#include "prmSystemCallback.h"

namespace Prm {

static size_t currentIndex;
using crc_t = uint16_t;
constexpr uint16_t magic = 0x2805;


#include "parameter.def"

IVal *getbyaddress(uint16_t address){
	currentIndex = 0;
	for(auto *p : valuearray){
		if(address == p->getaddress()){
			return p;
		}
		currentIndex++;
	}
	return nullptr;
}

IVal *getNext(){
	currentIndex++;
	if(currentIndex >= valuearray.size()){
		return nullptr;
	}
	return valuearray[currentIndex];
}

size_t getSerialSize(Save save){
	size_t size = 0;
	for(auto *p : valuearray){
		if(save == p->getsave()){
			size += p->getsize();
		}
	}
	return size + sizeof(magic) + sizeof(crc_t);
}

bool serialize(Save save, uint8_t *dst){
	const uint8_t *dstentry = dst;
	memcpy(dst, &magic, sizeof(magic));
	dst += sizeof(magic);
	for(auto *p : valuearray){
		if(save == p->getsave()){
			p->serialize(dst);
			dst += p->getsize();
		}
	}
	crc_t crc = crc16Calc(&crcModBus, dstentry, dst - dstentry);
	memcpy(dst, &crc, sizeof(crc));
	return true;
}

bool deserialize(Save save, const uint8_t *src, size_t size){
	std::remove_cv_t<decltype(magic)> readmagic;
	memcpy(&readmagic, src, sizeof(readmagic));
	if(readmagic != magic) return false;
	if(crc16Calc(&crcModBus, src, size)) return false;
	src += sizeof(magic);
	for(auto *p : valuearray){
		if(save == p->getsave()){
			p->deserialize(src);
			src += p->getsize();
		}
	}
	return true;
}

template <class T>
size_t Val<T>::uprintval(char *string, size_t size, uint8_t power, uint32_t var) const{
	static const int32_t pows[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };
	if(power == 0){
		return snprintf(string, size, "%" PRIu32, var);
	}else{
		uint32_t a = var / pows[power];
		uint32_t b = var % pows[power];
		return snprintf(string, size, "%" PRIu32 ".%0*" PRIu32, a, power, b);
	}
}

template <class T>
size_t Val<T>::iprintval(char *string, size_t size, uint8_t power, int32_t var) const{
	static const int32_t pows[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };
	if(power == 0){
		return snprintf(string, size, "%" PRIi32, var);
	}else{
		bool minus = false;
		if(var < 0) var = -var, minus = true;
		uint32_t a = var / pows[power];
		uint32_t b = var % pows[power];
		return snprintf(string, size, "%s%" PRIi32 ".%0*" PRIu32, minus ? "-" : "", a, power, b);
	}
}

template <> size_t Val<bool>::tostring(char *string, size_t size) const{
	return snprintf(string, size, "%s", val ? "true" : "false");
};

template <> size_t Val<char>::tostring(char *string, size_t size) const{
	if(size < 2){
		return 0;
	}
	string[0] = val;
	string[1] = '\0';
	return 1;
};

template <> size_t Val<int8_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return iprintval(string, size, handler.power, val);
};

template <> size_t Val<uint8_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return uprintval(string, size, handler.power, val);
};

template <> size_t Val<int16_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return iprintval(string, size, handler.power, val);
};

template <> size_t Val<uint16_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return uprintval(string, size, handler.power, val);
};

template <> size_t Val<int32_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return iprintval(string, size, handler.power, val);
};

template <> size_t Val<uint32_t>::tostring(char *string, size_t size) const{
	if(handler.text){
		const char *s = handler.text->get(val);
		if(s) strncpy(string, s, size);
		return strlen(string);
	}
	return uprintval(string, size, handler.power, val);
};

template <> size_t Val<int64_t>::tostring(char *string, size_t size) const{
	// Not implemented
	(void)string;
	(void)size;
	return 0;
};

template <> size_t Val<uint64_t>::tostring(char *string, size_t size) const{
	// Not implemented
	(void)string;
	(void)size;
	return 0;
};

template <> size_t Val<float>::tostring(char *string, size_t size) const{
	return snprintf(string, size, "%f", val);
}

} // namespace Prm

/******************************** END OF FILE ********************************/
