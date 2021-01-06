/*!****************************************************************************
 * @file		syscall.c
 * @author		d_el
 * @version		V1.1
 * @date		02.05.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		POSIX System Calls
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "stm32f4xx.h"

/* These magic symbols are provided by the linker.  */
extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

extern void _fini(void);
extern uint32_t __get_MSP(void);

/*!****************************************************************************
 * @brief 	Increase program data space. Malloc and related functions depend on this
 * @param 	incr - increase bytes
 * @return	previous heap end value
 */
__attribute__((weak))
caddr_t _sbrk(intptr_t incr){
	extern size_t _ebss; 							/// Defined by the linker
	static caddr_t heapEnd = (caddr_t) &_ebss;
	caddr_t prevHeapEnd = heapEnd;
	caddr_t stack = (caddr_t) __get_MSP();

	if(heapEnd + incr >= stack){
		errno = ENOMEM;
		abort();
	}

	heapEnd += incr;
	return (caddr_t) prevHeapEnd;
}

/*!****************************************************************************
 * @brief	Terminates the calling process "immediately"
 * @param 	code - exit status
 */
__attribute__((weak))
void _exit(int code){
	(void)code;
	while(1);
}

/*!****************************************************************************
 * @brief	Initialize serial data structure
 */
__attribute__((weak))
void _init(void){

}

/*!****************************************************************************
 *
 */
__attribute__((weak))
void _fini(void){

}

/*!****************************************************************************
 * @brief	Iterate over all the init routines
 */
__attribute__((weak))
void __libc_init_array(void){
	size_t count;
	size_t i;

	count = __preinit_array_end - __preinit_array_start;
	for(i = 0; i < count; i++){
		__preinit_array_start[i]();
	}

	_init();

	count = __init_array_end - __init_array_start;
	for(i = 0; i < count; i++){
		__init_array_start[i]();
	}
}

/*!****************************************************************************
 * @brief	Run all the cleanup routines
 */
__attribute__((weak))
void __libc_fini_array(void){
	size_t count;
	size_t i;

	count = __fini_array_end - __fini_array_start;
	for(i = count; i > 0; i--){
		__fini_array_start[i - 1]();
	}
	_fini();
}

/*!****************************************************************************
 * @brief	Send a signal. Minimal implementation:
 */
__attribute__((weak))
int _kill(int pid, int sig){
	(void)pid;
	(void)sig;
	errno = ENOSYS;
	return -1;
}

/*!****************************************************************************
 * @brief	Process-ID; this is sometimes used to generate strings unlikely to
 * 			conflict with other processes.
 * 			Minimal implementation, for a system without processes:
 */
__attribute__((weak))
int _getpid(void){
	errno = ENOSYS;
	return 1;
}

/*!****************************************************************************
 * @brief	Open a file. A minimal implementation without file system
 * @retval	Returns -1 on error or number of bytes sent
 */
int _open (const char *name, int flags, int mode){
	(void)name;
	(void)flags;
	(void)mode;
	errno = ENOSYS;
	return -1;
}

/*!****************************************************************************
 * @brief	Close a file
 * @retval	Returns -1 on error or number of bytes sent
 */
__attribute__((weak))
int _close(int file){
	(void)file;
	errno = ENOSYS;
	return -1;
}

/*!****************************************************************************
 * @brief	Read a character to a file. 'libc' subroutines will use this system
 * 			routine for input from all files, including stdin.
 * 			Returns -1 on error or blocks until the number of characters have
 * 			been read
 */
__attribute__((weak))
int _read(int file, char *ptr, int len){
	(void)file;
	(void)ptr;
	(void)len;
	errno = ENOSYS;
	return -1;
}

/*!****************************************************************************
 * @brief	Set position in a file. Minimal implementation:
 */
__attribute__((weak))
int _lseek(int file, int ptr, int dir){
	(void)file;
	(void)ptr;
	(void)dir;
	errno = ENOSYS;
	return -1;
}

/*!****************************************************************************
 * @brief	Status of an open file. For consistency with other minimal
 * 			implementations in these examples,
 * 			all files are regarded as character special devices.
 * 			The sys/stat.h header file required is distributed in the 'include'
 * 			subdirectory for this C library
 */
__attribute__((weak))
int _fstat(int file, struct stat *st){
	(void)file;
	st->st_mode = S_IFCHR;
	return 0;
}

/*!****************************************************************************
 * @brief	Query whether output stream is a terminal. For consistency with the
 * 			other
 * 			minimal implementations:
 */
__attribute__((weak))
int _isatty(int file){
	switch(file){
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			errno = EBADF;
			return 0;
	}
}

/******************************** END OF FILE ********************************/
