#include "port.h"
#include "mb.h"
#include "mbport.h"
#include <FreeRTOS.h>
#include <task.h>

/* ----------------------- Static variables ---------------------------------*/
TickType_t ulTimeOut;
TickType_t xTimeLast;
BOOL bTimeoutEnable;


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us){
	ulTimeOut = usTim1Timerout50us * 50 / 1000;
	if(ulTimeOut == 0){
		ulTimeOut = 1;
	}
	return TRUE;
}

void xMBPortTimersClose(){
	/* Does not use any hardware resources. */
}

void vMBPortTimersEnable(){
	xTimeLast = xTaskGetTickCount();
	bTimeoutEnable = TRUE;
}

void vMBPortTimersDisable(){
	bTimeoutEnable = FALSE;
}
