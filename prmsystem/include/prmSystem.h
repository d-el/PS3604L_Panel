/*!****************************************************************************
 * @file		prmSystem.h
 * @author		d_el - Storozhenko Roman
 * @version		V2.2
 * @date		07.01.2024
 * @copyright	The MIT License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief		Parameters system
 */
#ifndef PRMSYSTEM_H
#define PRMSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include <array>
#include <type_traits>
#include <cmath>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

namespace Prm {

/*!****************************************************************************
 * Define
 */
enum Save: uint8_t{
	notsave,
	savesys,
	saveuse
};

class IText{
public:
	virtual const char* get(int i) const = 0;
};

template<size_t n>
class Text: public IText
{
public:
	template <typename... Types>
	constexpr Text(Types... ts) : textvalues{ { ts... } } {}

	const char* get(int i) const {
		for(auto &t : textvalues){
			if(t.v == i) return t.t;
		}
		return nullptr;
	}

private:
	struct TextVal{
		int v;
		const char *t;
	};
	const std::array<TextVal, n> textvalues;
};

template <class T> class Val;

template <class T> class ValHandler
{
public:
	constexpr ValHandler(
			const char *_label, const char *_units,
			T _def, T _min, T _max, T _step, T _bigstep,
			uint16_t _addr, void* _arg, uint16_t _power,
			void (*_callback)(Val<T>& prm, bool read, void *arg), const IText *_text,
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
		callback(_callback),
		text(_text)
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
	void* const arg;
	const uint8_t power :4;
	const Save save;
	void (*callback)(Val<T>& prm, bool read, void *arg);
	const IText *text;
};

class IVal{
public:
	virtual void setdef() = 0;
	virtual void step(int32_t step) = 0;
	virtual void bigstep(int32_t step) = 0;
	virtual const char* getlabel() const = 0;
	virtual const char* getunit() const = 0;
	virtual size_t getsize() const = 0;
	virtual void* getarg() const = 0;
	virtual uint16_t getaddress() const = 0;
	virtual Save getsave() const = 0;
	virtual void serialize(void *dst) const = 0;
	virtual bool deserialize(const void *src) = 0;
	virtual size_t tostring(char *string, size_t size) const = 0;
	virtual void operator()(bool read, void *arg) = 0;
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
		stepsize(step, handler.step);
	}

	void bigstep(int32_t step){
		stepsize(step, handler.bigstep);
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

	void* getarg() const {
		return handler.arg;
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

	void operator()(bool read, void *arg){
		if(handler.callback) handler.callback(*this, read, arg);
	}

	void operator=(T v){
		val = v;
		if(handler.callback) handler.callback(*this, false, nullptr);
	}

	operator T(){
		if(handler.callback) handler.callback(*this, true, nullptr);
		return val;
	}

private:
	void stepsize(int32_t step, T stepsize){
		T result = stepsize * abs(step);
		if(step < 0)
			val = result > val - handler.min
				? handler.min
				: val - result;
		else
			val = result > handler.max - val
				? handler.max
				: val + result;
	}

	size_t uprintval(char *string, size_t size, uint8_t power, uint32_t var) const;
	size_t iprintval(char *string, size_t size, uint8_t power, int32_t var) const;

public:
	T val;

private:
	const ValHandler<T> &handler;
};

IVal *getbyaddress(uint16_t address);
IVal *getNext();
size_t getSerialSize(Save save);
bool serialize(Save save, uint8_t *dst);
bool deserialize(Save save, const uint8_t *src, size_t size);

#include "parameter.dcl"

};

#endif /* PRMSYSTEM_H */
/******************************** END OF FILE ********************************/
