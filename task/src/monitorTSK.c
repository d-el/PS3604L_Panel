/*!****************************************************************************
 * @file		monitorTSK.c
 * @author		d_el
 * @version		V1.0
 * @date		05.04.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <portable.h>
#include "plog.h"
#include "sysTimeMeas.h"
#include <lwip/stats.h>

/*!****************************************************************************
 * MEMORY
 */
#define LOG_LOCAL_LEVEL P_LOG_VERBOSE
static char *logTag = "MONITOR TSK";

/*
 * Since at least FreeRTOS V7.5.3 uxTopUsedPriority is no longer
 * present in the kernel, so it has to be supplied by other means for
 * OpenOCD's threads awareness.
 *
 * Add this file to your project, and, if you're using --gc-sections,
 * ``--undefined=uxTopUsedPriority'' (or
 * ``-Wl,--undefined=uxTopUsedPriority'' when using gcc for final
 * linking) to your LDFLAGS; same with all the other symbols you need.
 */
uint32_t monitorPeriod = pdMS_TO_TICKS(1000);

/*!****************************************************************************
 * @brief
 */
void vConfigureTimerForRunTimeStats(void){
	sysTimeMeasEnable();
}

/*!****************************************************************************
 * @brief
 */
unsigned long vGetTimerForRunTimeStats(void){
	static uint64_t counter = 0;
	static uint32_t oldcnt = 0;
	uint32_t cnt = sysTimeMeasGetCnt();

	counter += sysTimeMeasTo_us(cnt - oldcnt);
	oldcnt = cnt;

	// Return time in us
	return counter;
}

/*!****************************************************************************
 * @brief
 */
void monitorTSK(void *pPrm){
	(void)pPrm;
	static const char *stateToChar[] = {
		"Running",		/* A task is querying the state of itself, so must be running. */
		"Ready",		/* The task being queried is in a read or pending ready list. */
		"Blocked",		/* The task being queried is in the Blocked state. */
		"Suspended",	/* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
		"Deleted",		/* The task being queried has been deleted, but its TCB has not yet been freed. */
		"Invalid"		/* Used as an 'invalid state' value. */
	};

	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t maxTask = 10;
	uint32_t taskTimePrev[maxTask];
	memset(taskTimePrev, 0, sizeof(taskTimePrev));

	P_LOGI(logTag, "Started monitorTSK");

	while(1){
		UBaseType_t taskCount = uxTaskGetNumberOfTasks();

		if(taskCount <= maxTask){
			unsigned long totalRuntime;
			TaskStatus_t buffer[maxTask];

			taskCount = uxTaskGetSystemState(buffer, taskCount, &totalRuntime);

			uint32_t allTaskPeriodTime = 0;
			uint32_t idleTaskPeriodTime = 0;
			for (UBaseType_t task = 0; task < taskCount; task++){
				allTaskPeriodTime += buffer[task].ulRunTimeCounter - taskTimePrev[task];

				if(buffer[task].uxCurrentPriority == 0){
					idleTaskPeriodTime = buffer[task].ulRunTimeCounter - taskTimePrev[task];
				}

				taskTimePrev[task] = buffer[task].ulRunTimeCounter;

				P_LOGI(logTag, "%20s: %9s, %"PRIu32", %6"PRIu16" B, %"PRIu32" us",
				buffer[task].pcTaskName,
				stateToChar[buffer[task].eCurrentState],
				buffer[task].uxCurrentPriority,
				buffer[task].usStackHighWaterMark,
				buffer[task].ulRunTimeCounter);
			}

			P_LOGI(logTag, "Current Heap Free Size: %u", xPortGetFreeHeapSize());
			P_LOGI(logTag, "Minimal Heap Size: %u", xPortGetMinimumEverFreeHeapSize());
			P_LOGI(logTag, "Total RunTime: %"PRIu32" us", totalRuntime);
			P_LOGI(logTag, "System Uptime: %"PRIu32" ms", xTaskGetTickCount() * portTICK_PERIOD_MS);

			P_LOGI(logTag, "All task PeriodTime:  %"PRIu32" us", allTaskPeriodTime);
			P_LOGI(logTag, "Idle task PeriodTime: %"PRIu32" us", idleTaskPeriodTime);


			if(allTaskPeriodTime >= idleTaskPeriodTime){
				uint64_t effectiveTaskPeriodTime = allTaskPeriodTime - idleTaskPeriodTime;
				uint32_t load = (effectiveTaskPeriodTime * 100000) / allTaskPeriodTime;
				P_LOGI(logTag, "OS load: %"PRIu32".%03"PRIu32" %%", load / 1000, load % 1000);
			}
		}

		stats_display();

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(monitorPeriod));
	}
}

/*!****************************************************************************
 *
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
	(void)xTask;
	P_LOGE(logTag, "Stack Overflow on %s", pcTaskName);
	while(1);
}

/*!****************************************************************************
 *
 */
void vApplicationMallocFailedHook(void){
	P_LOGE(logTag, "Malloc Failed");
	while(1);
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static – otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task’s stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/******************************** END OF FILE ********************************/
