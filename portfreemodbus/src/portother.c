/* ----------------------- Standard includes --------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
void vMBPortLogLevel(eMBPortLogLevel eNewLevelMax){
	(void)eNewLevelMax;
}

void vMBPortLogFile(FILE *fNewLogFile){
	(void)fNewLogFile;
}

void vMBPortLog(eMBPortLogLevel eLevel, const CHAR *szModule, const CHAR *szFmt, ...){
	(void)eLevel;
	(void)szModule;
	(void)szFmt;
}

void vMBPortEnterCritical(void){

}

void vMBPortExitCritical(void){

}
