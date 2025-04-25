/*!****************************************************************************
 * @file		plog.h
 * @author		d_el
 * @version		V1.1
 * @date		21.03.2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		Logging utility
 */

#ifndef plog_H
#define plog_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/*!****************************************************************************
 * Define
 */
#define CONFIG_LOG_COLORS	1

#if CONFIG_LOG_COLORS
#define LOG_COLOR_BLACK		"30"
#define LOG_COLOR_RED		"31"
#define LOG_COLOR_GREEN		"32"
#define LOG_COLOR_BROWN		"33"
#define LOG_COLOR_BLUE		"34"
#define LOG_COLOR_PURPLE	"35"
#define LOG_COLOR_CYAN		"36"
#define LOG_COLOR(COLOR)	"\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)		"\033[1;" COLOR "m"
#define LOG_RESET_COLOR		"\033[0m"
#define LOG_COLOR_E			LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W			LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I			LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D			LOG_COLOR(LOG_COLOR_CYAN)
#define LOG_COLOR_V
#else //CONFIG_LOG_COLORS
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS

#define LOG_FORMAT(letter, format)	LOG_COLOR_ ## letter #letter " (%" PRIu32 ") %s: " format LOG_RESET_COLOR "\r\n"

#define P_LOGE( tag, format, ... ) P_LOG_LEVEL_LOCAL(P_LOG_ERROR,	tag, format, ##__VA_ARGS__)
#define P_LOGW( tag, format, ... ) P_LOG_LEVEL_LOCAL(P_LOG_WARN,	tag, format, ##__VA_ARGS__)
#define P_LOGI( tag, format, ... ) P_LOG_LEVEL_LOCAL(P_LOG_INFO,	tag, format, ##__VA_ARGS__)
#define P_LOGD( tag, format, ... ) P_LOG_LEVEL_LOCAL(P_LOG_DEBUG,	tag, format, ##__VA_ARGS__)
#define P_LOGV( tag, format, ... ) P_LOG_LEVEL_LOCAL(P_LOG_VERBOSE, tag, format, ##__VA_ARGS__)

/** runtime macro to output logs at a specified level.
 *
 * @param tag tag of the log, which can be used to change the log level by ``plog_level_set`` at runtime.
 * @param level level of the output log.
 * @param format format of the output log. see ``printf``
 * @param ... variables to be replaced into the log. see ``printf``
 *
 * @see ``printf``
 */
#define P_LOG_LEVEL(level, tag, format, ...) do {					  \
		if		(level==P_LOG_ERROR)	  { plog_write(P_LOG_ERROR,		 tag, LOG_FORMAT(E, format), plog_timestamp(), tag, ##__VA_ARGS__); } \
		else if (level==P_LOG_WARN)		  { plog_write(P_LOG_WARN,		 tag, LOG_FORMAT(W, format), plog_timestamp(), tag, ##__VA_ARGS__); } \
		else if (level==P_LOG_DEBUG)	  { plog_write(P_LOG_DEBUG,		 tag, LOG_FORMAT(D, format), plog_timestamp(), tag, ##__VA_ARGS__); } \
		else if (level==P_LOG_VERBOSE)	  { plog_write(P_LOG_VERBOSE,	 tag, LOG_FORMAT(V, format), plog_timestamp(), tag, ##__VA_ARGS__); } \
		else							  { plog_write(P_LOG_INFO,		 tag, LOG_FORMAT(I, format), plog_timestamp(), tag, ##__VA_ARGS__); } \
	} while(0)

/** runtime macro to output logs at a specified level. Also check the level with ``LOG_LOCAL_LEVEL``.
 *
 * @see ``printf``, ``P_LOG_LEVEL``
 */
#define P_LOG_LEVEL_LOCAL(level, tag, format, ...) do {				  \
		if ( LOG_LOCAL_LEVEL >= level ) P_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
	} while(0)

/*!****************************************************************************
 * Typedef
 */
/**
 * @brief Log level
 *
 */
typedef enum {
	P_LOG_NONE,		  /*!< No log output */
	P_LOG_ERROR,	  /*!< Critical errors, software module can not recover on its own */
	P_LOG_WARN,		  /*!< Error conditions from which recovery measures have been taken */
	P_LOG_INFO,		  /*!< Information messages which describe normal flow of events */
	P_LOG_DEBUG,	  /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
	P_LOG_VERBOSE	  /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
}plog_level_t;

typedef int (*plog_vsnprintf_type)(char *, size_t maxlen, const char *, va_list);
typedef ssize_t (*plog_write_type)(int fd, const void *buf, size_t count);
typedef uint32_t (*plog_timestamp_type)(void);

/*!****************************************************************************
 * Function declaration
 */
plog_vsnprintf_type plog_setVprintf(plog_vsnprintf_type func);
plog_write_type plog_setWrite(plog_write_type func);
int plog_setWriteFd(int fd);
plog_timestamp_type plog_setTimestamp(plog_timestamp_type func);
int plog_write(plog_level_t level, const char* tag, const char* format, ...) __attribute__ ((format (printf, 3, 4)));
int plog_printf(const char* format, ...) __attribute__ ((format (printf, 1, 2)));
int plog_hexdumpcolumn(const void *buffer, size_t length, size_t column);
uint32_t plog_timestamp(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //plog_H
/******************************** END OF FILE ********************************/
