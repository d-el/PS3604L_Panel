/*!****************************************************************************
 * @file		24AAxx.h
 * @author		d_el
 * @version		V1.0
 * @date		21.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for eeprom memory 24AA04, 24AA08
 */
#ifndef e24AAxx_H
#define e24AAxx_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
* Include
*/
#include <stdint.h>
#include <stddef.h>
#include <driver/i2c.h>

/*!****************************************************************************
* Prototypes for the functions
*/
class Eep24AA{
public:
	Eep24AA() = default;
	Eep24AA(I2cManager* i2c);
	void setI2c(I2cManager* i2c);
	void init(void);
	bool write(uint16_t dst, const void *src, uint16_t len);
	bool read(void *dst, uint16_t src, uint16_t len);

private:
	I2cManager* m_i2c = nullptr;
};

#ifdef __cplusplus
}
#endif

#endif //e24AAxx_H
/******************************** END OF FILE ********************************/
