/*!****************************************************************************
 * @file		prmSystem.h
 * @author		d_el - Storozhenko Roman
 * @version		V2.0
 * @date		25.01.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2021 Storozhenko Roman
 * @brief		Parameters system
 */
#ifndef PRMSYSTEM_H
#define PRMSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include <array>
#include <type_traits>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmath>

namespace Prm {

/*!****************************************************************************
 * Define
 */
enum Save: uint8_t{
	notsave,
	savesys,
	saveuse
};

struct Text{
	const int v;
	const char *s;
};

template <class T> class ValHandler
{
public:
	constexpr ValHandler(
			const char *_label, const char *_units,
			T _def, T _min, T _max, T _step, T _bigstep,
			uint16_t _addr, uint16_t _arg, uint16_t _power,
			void (*_callback)(const ValHandler& prm, bool read, void *arg),
			Save _save) :
		label(_label),
		units(_units),
		def(_def),
		min(_min),
		max(_max),
		step(_step),
		bigstep(_bigstep),
		addr(_addr),
		arg(_arg),
		power(_power),
		save(_save),
		callback(_callback)
	{};

	const char *label;
	const char *units;
	const T def;
	const T min;
	const T max;
	const T step;
	const T bigstep;
	const uint16_t addr;
	constexpr static size_t size = sizeof(T);
	const uint16_t arg;
	const uint8_t power :4;
	const Save save;
	void (*callback)(const ValHandler& prm, bool read, void *arg);
};

class IVal{
public:
	virtual void setdef() = 0;
	virtual void step(int32_t step) = 0;
	virtual void bigstep(int32_t step) = 0;
	virtual const char* getlabel() const = 0;
	virtual const char* getunit() const = 0;
	virtual size_t getsize() const = 0;
	virtual uint16_t getaddress() const = 0;
	virtual Save getsave() const = 0;
	virtual void serialize(void *dst) const = 0;
	virtual bool deserialize(const void *src) = 0;
	virtual size_t tostring(char *string, size_t size) const = 0;
};

template <class T> class Val: public IVal
{
public:
	Val(const ValHandler<T> &_handler) :
		handler(_handler)
	{ val = handler.def; }

	void setdef(){
		val = handler.def;
	}

	void step(int32_t step){
		val += handler.step * step;
		if(val > handler.max) val = handler.max;
		if(val < handler.min) val = handler.min;
	}

	void bigstep(int32_t step){
		auto result = val + handler.bigstep * step;
		if(step > 0 && result > handler.max) result = handler.max;
		if(step < 0 && result < handler.min) result = handler.min;
		val = result;
	}

	const char* getlabel() const {
		return handler.label;
	}

	const char* getunit() const {
		return handler.units;
	}

	size_t getsize() const {
		return handler.size;
	}

	uint16_t getaddress() const {
		return handler.addr;
	}

	virtual Save getsave() const {
		return handler.save;
	}

	void serialize(void *dst) const {
		memcpy(dst, &val, sizeof(val));
	}

	bool deserialize(const void *src){
		T v = 0;
		memcpy(&v, src, sizeof(v));
		if constexpr(std::is_same_v<T, float>){
			if(std::isnan(v)){
				return false;
			}
		}
		if(v > handler.max || v < handler.min){
			return false;
		}
		val = v;
		return true;
	}

	size_t tostring(char *string, size_t size) const;

private:
	size_t uprintval(char *string, size_t size, uint8_t power, uint32_t var) const;
	size_t iprintval(char *string, size_t size, uint8_t power, int32_t var) const;

public:
	T val;
	const ValHandler<T> &handler;
};

IVal *getbyaddress(uint16_t address);
size_t getSerialSize(Save save);
bool serialize(Save save, uint8_t *dst);
bool deserialize(Save save, const uint8_t *src);

#include "parameter.dcl"

};

#endif /* PRMSYSTEM_H */
/******************************** END OF FILE ********************************/
