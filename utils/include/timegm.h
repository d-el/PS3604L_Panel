/*!****************************************************************************
 * @file		timegm.h
 * @author		d_el
 * @version		V1.0
 * @date		Oct 4, 2022
 * @copyright	License (MIT). Copyright (c) 2022 Storozhenko Roman
 * @brief
 */

#ifndef timegm_H
#define timegm_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

/*!****************************************************************************
 * Function declaration
 */
time_t timegm(struct tm const *t);

#ifdef __cplusplus
}
#endif

#endif //timegm_H
/******************************** END OF FILE ********************************/
