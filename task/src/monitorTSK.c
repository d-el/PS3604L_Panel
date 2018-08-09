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
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"
#include "plog.h"
#include "sysTimeMeas.h"

/*!****************************************************************************
 * MEMORY
 */
#define LOG_LOCAL_LEVEL P_LOG_ERROR
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
__attribute__((used)) const int uxTopUsedPriority = configMAX_PRIORITIES - 1;
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
uint32_t load;
/*!****************************************************************************
 * @brief
 */
void monitorTSK(void *pPrm){
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

				P_LOGI(logTag, "%20s: %9s, %u, %6u, %u us\n",
				buffer[task].pcTaskName,
				stateToChar[buffer[task].eCurrentState],
				buffer[task].uxCurrentPriority,
				buffer[task].usStackHighWaterMark,
				buffer[task].ulRunTimeCounter);
			}

			P_LOGI(logTag, "Current Heap Free Size: %u\n", xPortGetFreeHeapSize());
			P_LOGI(logTag, "Total RunTime: %u us\n", totalRuntime);
			P_LOGI(logTag, "System Uptime: %u ms\n", xTaskGetTickCount() * portTICK_PERIOD_MS);

			P_LOGI(logTag, "All task PeriodTime:  %u us\n", allTaskPeriodTime);
			P_LOGI(logTag, "Idle task PeriodTime: %u us\n", idleTaskPeriodTime);


			if(allTaskPeriodTime >= idleTaskPeriodTime){
				uint64_t effectiveTaskPeriodTime = allTaskPeriodTime - idleTaskPeriodTime;
				load = (effectiveTaskPeriodTime * 100000) / allTaskPeriodTime;
				P_LOGI(logTag, "OS load: %u.%03u %%\n", load / 1000, load % 1000);
			}
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(monitorPeriod));
	}
}

/*!****************************************************************************
 *
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName){
	P_LOGE(logTag, "Stack Overflow on %s\n", pcTaskName);
	while(1);
}

/*!****************************************************************************
 *
 */
void vApplicationMallocFailedHook(void){
	P_LOGE(logTag, "Malloc Failed\n");
	while(1);
}

/******************************** END OF FILE ********************************/
