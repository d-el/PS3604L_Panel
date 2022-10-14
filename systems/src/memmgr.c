/*!****************************************************************************
 * @file		memmgr.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		12.12.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2021 Storozhenko Roman
 */

#include <string.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "task.h"

void* malloc(size_t size){
	return pvPortMalloc(size);
}

void free(void *ptr){
	vPortFree(ptr);
}

void* realloc(void *ptr, size_t size){
	if(size == 0){
		vPortFree(ptr);
		return NULL;
	}

	void *p = pvPortMalloc(size);
	if(p){
		vTaskSuspendAll();
		if(ptr != NULL){
			memcpy(p, ptr, size);
			vPortFree(ptr);
		}
		xTaskResumeAll();
	}
	return p;
}

void* calloc(size_t count, size_t size){
	void *ptr = pvPortMalloc(count * size);
	if(ptr){
		// zero the memory
		memset(ptr, 0, count * size);
	}
	return ptr;
}

void* __wrap__malloc(size_t size){
	void *pointer = malloc(size);
	return pointer;
}

void __wrap__free(void *ptr){
	free(ptr);
}

void* __wrap__calloc(size_t count, size_t size){
	void *pointer = calloc(count, size);
	return pointer;
}

void* __wrap__realloc(void *ptr, size_t size){
	void *pointer = realloc(ptr, size);
	return pointer;
}

void* __wrap__malloc_r(struct _reent *r, size_t size){
	(void)r;
	void *pointer = malloc(size);
	return pointer;
}

void __wrap__free_r(struct _reent *r, void *ptr){
	(void)r;
	free(ptr);
}

void* __wrap__calloc_r(struct _reent *r, size_t count, size_t size){
	(void)r;
	void *pointer = calloc(count, size);
	return pointer;
}

void* __wrap__realloc_r(struct _reent *r, void *ptr, size_t size){
	(void)r;
	void *pointer = realloc(ptr, size);
	return pointer;
}
