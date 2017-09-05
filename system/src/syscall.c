/*!****************************************************************************
 * @file		syscall.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		02.05.2017
 * @copyright	GNU Lesser General Public License v3
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

//extern void _init();
extern void _fini(void);
extern uint32_t __get_MSP(void);

/*!****************************************************************************
 */
/*int _gettimeofday(struct timeval *tv, struct timezone *tz){
 return 0;
 }*/

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
void _exit(int code __attribute__((unused))){
	while(1){
		__NOP();
	}
}

/*!****************************************************************************
 *
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
 * Iterate over all the init routines
 */
__attribute__((weak))
void __libc_init_array(void){
	size_t count;
	size_t i;

	count = __preinit_array_end - __preinit_array_start;
	for(i = 0; i < count; i++)
		__preinit_array_start[i]();

	_init();

	count = __init_array_end - __init_array_start;
	for(i = 0; i < count; i++)
		__init_array_start[i]();
}

/*!****************************************************************************
 * Run all the cleanup routines
 */
__attribute__((weak))
void __libc_fini_array(void){
	size_t count;
	size_t i;

	count = __fini_array_end - __fini_array_start;
	for(i = count; i > 0; i--)
		__fini_array_start[i - 1]();

	_fini();
}

/*!****************************************************************************
 * kill - послать сигнал процессу
 */
__attribute__((weak))
int _kill(int pid, int sig){
	errno = EINVAL;
	return (-1);
}

/*!****************************************************************************
 * getpid - получить ID текущего процесса
 */
__attribute__((weak))
int _getpid(){
	return 1;
}

/*!****************************************************************************
 * write - запись в файл - у нас есть только stderr/stdout
 */
__attribute__((weak))
int _write(int file, char *ptr, int len){
	return 0;
}

/*!****************************************************************************
 * close - закрытие файла - возвращаем ошибку
 */
__attribute__((weak))
int _close(int file){
	return -1;
}

/*!****************************************************************************
 * fstat - состояние открытого файла
 */
__attribute__((weak))
int _fstat(int file, struct stat *st){
	st->st_mode = S_IFCHR;
	return 0;
}

/*!****************************************************************************
 * isatty - является ли файл терминалом.
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

/*!****************************************************************************
 * lseek - установить позицию в файле
 */
__attribute__((weak))
int _lseek(int file, int ptr, int dir){
	return 0;
}

/*!****************************************************************************
 * read - чтение из файла, у нас пока для чтения есть только stdin
 */
__attribute__((weak))
int _read(int file, char *ptr, int len){
	return -1;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
