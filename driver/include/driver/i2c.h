/*!****************************************************************************
 * @file		i2c.h
 * @author		d_el
 * @version		V1.0
 * @date		Jan 22, 2026
 * @copyright	License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief
 */

#ifndef drv_i2c_H
#define drv_i2c_H

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */
class I2cManager{
public:
	bool init();
	bool read(uint8_t devAddr, void* dst, size_t len, uint16_t timeout=m_timeout_ms);
	bool write(uint8_t devAddr, const void* src, size_t len, bool needstop, uint16_t timeout=m_timeout_ms);
private:

private:
	void* m_sem = nullptr;
	void* m_mutex = nullptr;
	static constexpr uint16_t m_timeout_ms = 100;
};

I2cManager& i2c(uint8_t number);

#endif //i2c_H
/******************************** END OF FILE ********************************/
