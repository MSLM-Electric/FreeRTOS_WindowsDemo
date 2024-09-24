
/*
	FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
	All rights reserved

	VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License (version 2) as published by the
	Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
	>>!   NOTE: The modification to the GPL is included to allow you to     !<<
	>>!   distribute a combined work that includes FreeRTOS without being   !<<
	>>!   obliged to provide the source code for proprietary components     !<<
	>>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

	FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE.  Full license text is available on the following
	link: http://www.freertos.org/a00114.html

	http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

	http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
	including FreeRTOS+Trace - an indispensable productivity tool, a DOS
	compatible FAT file system, and our tiny thread aware UDP/IP stack.

	http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
	Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

	http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
	Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
	licenses offer ticketed support, indemnification and commercial middleware.

	http://www.SafeRTOS.com - High Integrity Systems also provide a safety
	engineered and independently SIL3 certified version for use in safety and
	mission critical applications that require provable dependability.

	1 tab == 4 spaces!
*/

/* FreeRTOS.org includes. */
#include "../../Win32-simulator-MSVC/FreeRTOS_Source/include/FreeRTOS.h"
#include "../../Win32-simulator-MSVC/FreeRTOS_Source/include/task.h"
#include "../../Win32-simulator-MSVC/FreeRTOS_Source/include/timers.h"

/* Demo includes. */
#include "../../Win32-simulator-MSVC/Supporting_Functions/supporting_functions.h"

#include "../../ExternalLibs/cmsis_os/cmsis_os.h"
#include "../../ExternalLibs/BitLogger/BitLogger.h"
#include "../../ExternalLibs/SimpleTimer/SimpleTimerWP.h"
#include "../../ExternalLibs/type_def.h"
#include <stdio.h>

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* The number of the simulated interrupt used in this example.  Numbers 0 to 2
are used by the FreeRTOS Windows port itself, so 3 is the first number available
to the application. */
#define mainINTERRUPT_NUMBER	3
#define timerINTERRUPT_NUMBER   mainINTERRUPT_NUMBER+1

#define ONE_SHOT_TIMER 0
#define PERIODIC_TIMER 1

/* The task functions. */
void vSomeAnotherTask(void* pvParameters);
void HardwareTimerInterruption_Immitate(void* pvParameters);
void xPortSysTickHandler(void);
void MultipleTasksByOneFunction(void* pvParameters);
#ifndef CMSIS_OS_ENABLE
static U32_ms osKernelSysTick(void);
#endif // CMSIS_OS_ENABLE


static /*or extern*/ BitLoggerList_t BugsBitList;
static Timerwp_t BugScannerTimer;
static Timerwp_t BugBitReportTimer;
static u16 BitPos(u16 Bit);
static Timerwp_t TimersArray[6];
static u32 crudeDelay[6];
static u32 crudeCntr[6];

/* The service routine for the (simulated) interrupt.  This is the interrupt
that the task will be synchronized with. */
static uint32_t ulExampleInterruptHandler(void);
static uint32_t ulTimerInterruptHandler(void);

/*-----------------------------------------------------------*/

int main(int argc, char** argv)
{
	/* Create one of the two tasks. */
	xTaskCreate(vSomeAnotherTask,		/* Pointer to the function that implements the task. */
		"Some Task",	/* Text name for the task.  This is to facilitate debugging only. */
		1000,		/* Stack depth - most small microcontrollers will use much less stack than this. */
		NULL,		/* We are not using the task parameter. */
		1,			/* This task will run at priority 1. */
		NULL);		/* We are not using the task handle. */
/* Create the array of tasks in exactly the same way. */
	char taskNo[6] = { '0','1','2','3','4','5' };
	for (uint8_t u = 0; u < 6; u++) {
		char taskName[] = "Buggy task N";
		taskName[strlen(taskName) - 1] = taskNo[u];//0x30 + u;
		xTaskCreate(MultipleTasksByOneFunction, taskName, 100, &taskNo[u], 1, NULL);
		/*The created array of tasks uses only single field vars of functional code/referenced task code*/
	}
	InitTimerGroup(TimersArray, (tickptr_fn*)osKernelSysTick, 6, (U32_ms)50);
	memset(crudeDelay, 0, sizeof(crudeDelay));
	memset(crudeCntr, 0, sizeof(crudeCntr));

	xTaskCreate(HardwareTimerInterruption_Immitate, "Timer Interrupt", 100, NULL, 1, NULL);
	init_simulatePROCESSOR_MODES(); //!for using cmsis_os funcs
	/* Install the handler for the software interrupt.  The syntax necessary
		to do this is dependent on the FreeRTOS port being used.  The syntax
		shown here can only be used with the FreeRTOS Windows port, where such
		interrupts are only simulated. */
	vPortSetInterruptHandler(mainINTERRUPT_NUMBER, ulExampleInterruptHandler);
	vPortSetInterruptHandler(timerINTERRUPT_NUMBER, ulTimerInterruptHandler);

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for (;; );
	return 0;
}
/*-----------------------------------------------------------*/

void vSomeAnotherTask(void* pvParameters)
{
	const char* pcTaskName = "Some another task running!\r\n";
	volatile uint32_t ul;
	static char buffer[200];

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;; )
	{
		/* Print out the name of this task. */
		vPrintString(pcTaskName);

		/* Delay for a period. */
		vTaskDelay(50);
	}
}
/*-----------------------------------------------------------*/

//#include <stdio.h>

static uint32_t ulExampleInterruptHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;

	/* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
	it will get set to pdTRUE inside the interrupt safe API function if a
	context switch is required. */
	xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore to unblock the task. */
	//xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

	/* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().  If
	xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
	then calling portYIELD_FROM_ISR() will request a context switch.  If
	xHigherPriorityTaskWoken is still pdFALSE then calling
	portYIELD_FROM_ISR() will have no effect.  The implementation of
	portYIELD_FROM_ISR() used by the Windows port includes a return statement,
	which is why this function does not explicitly return a value. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static uint32_t ulTimerInterruptHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	/*user code*/

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HardwareTimerInterruption_Immitate(void* pvParameters)
{
	for (;;) {
		vTaskDelay(1);
		vPortGenerateSimulatedInterrupt(mainINTERRUPT_NUMBER);
		vPortGenerateSimulatedInterrupt(timerINTERRUPT_NUMBER);
	}
}

#define CHAR_TO_NUM(x) x-0x30
void MultipleTasksByOneFunction(void* pvParameters)
{
	char* pcTaskName = "Task N";
	uint8_t taskID = 0;

	const char* additnstr = " running";
	//const char* taskInfo[40];

	volatile uint32_t ul;
	RestartTimerWP(&TimersArray[CHAR_TO_NUM(*(uint8_t*)pvParameters)]);

	for (;;)
	{
		taskID = *(uint8_t*)pvParameters;
		if (IsTimerWPRinging(&TimersArray[CHAR_TO_NUM(taskID)])) {
			RestartTimerWP(&TimersArray[CHAR_TO_NUM(taskID)]);
			pcTaskName[strlen(pcTaskName) - 1] = taskID;
			vPrintTwoStrings(pcTaskName, additnstr);
		}
		ul = taskID;
		//vTaskDelay(2);
		if (CHAR_TO_NUM(*(uint8_t*)pvParameters) < 3) {
			for (crudeCntr[CHAR_TO_NUM(*(uint8_t*)pvParameters)] = 0; crudeCntr[CHAR_TO_NUM(*(uint8_t*)pvParameters)] < 0x3FFF0000; crudeCntr[CHAR_TO_NUM(*(uint8_t*)pvParameters)]++);
		}
		if (taskID != (*(uint8_t*)pvParameters)) {
			DEBUG_PRINTF(1, ("Bug found on Task %d, taskID not matches to pvParameter", *(uint8_t*)pvParameters)); //! Tested and never happened!!!
		}
	}
}

#ifndef CMSIS_OS_ENABLE
static U32_ms osKernelSysTick(void)
{
#ifdef DEBUG_ON_VS
	return (U32_ms)GetTickCount();
#endif
}
#endif

void xPortSysTickHandler(void)
{
#ifdef DEBUG_ON_VS
	return (U32_ms)GetTickCount();
#endif // DEBUG_ON_VS
}

static u16 BitPos(u16 Bit)
{
	u16 res = 0;
	while ((Bit >> res) > 1) {
		res++;
	}
	return res;
}
