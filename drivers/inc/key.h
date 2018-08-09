/*!****************************************************************************
 * @file		key.h
 * @author		d_el
 * @version		V1.0
 * @date		07.06.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for keyboard
 */
#ifndef key_H
#define key_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */
#define KEY_NUM				( 7 )
#define KEY_SAMPLES			( 2 )

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	kMode =		(1 << 0),
	kView =		(1 << 1),
	kOnOff =	(1 << 2),
	kSet =		(1 << 3),
	kZero =		(1 << 4),
	kUp =		(1 << 5),
	kDown =		(1 << 6),
} kKey_type;

typedef struct {
	uint32_t dInState;
	uint32_t dInPrevState;
	uint32_t keyState;
	uint32_t reiterationSelect;
	uint8_t toFirstReiteration;
	uint8_t toReiteration;
	uint8_t dInFilterCnt[KEY_NUM];
	uint8_t toFirstReiterationCnt[KEY_NUM];
	uint8_t toReiterationCnt[KEY_NUM];
} key_type;

/*!****************************************************************************
 * Exported variables
 */
extern key_type key;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
uint32_t keyProc(void);
uint32_t keyState(kKey_type keyMask);
void ksSet(uint16_t toFirstReiteration, uint16_t toReiteration, kKey_type reiterationKeyMask);

#endif //key_H
/******************************** END OF FILE ********************************/
