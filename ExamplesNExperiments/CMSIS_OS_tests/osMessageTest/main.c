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
#include "../../../Win32-simulator-MSVC/FreeRTOSv10_Source/include/FreeRTOS.h"
#include "../../../Win32-simulator-MSVC/FreeRTOSv10_Source/include/task.h"
#include "../../../Win32-simulator-MSVC/FreeRTOSv10_Source/include/timers.h"

/* Demo includes. */
#include "../../../Win32-simulator-MSVC/Supporting_Functions/supporting_functions.h"

#include "../../../ExternalLibs/cmsis_os/cmsis_os.h"
#include "../../../ExternalLibs/BitLogger/BitLogger.h"
#include "../../../ExternalLibs/SimpleTimer/SimpleTimerWP.h"
#include "../../../ExternalLibs/type_def.h"
#include "debug_print.h"
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
void HardwareTimerInterruption_Immitate(void* pvParameters);
void xPortSysTickHandler(void);
#ifndef CMSIS_OS_ENABLE
static U32_ms osKernelSysTick(void);
#endif // CMSIS_OS_ENABLE


/* The service routine for the (simulated) interrupt.  This is the interrupt
that the task will be synchronized with. */
static uint32_t ulExampleInterruptHandler(void);
static uint32_t ulTimerInterruptHandler(void);

osThreadId tid_thread1;                          // ID for thread 1
osThreadId tid_thread2;                          // for thread 2

typedef struct {                                 // Message object structure
	float    voltage;                              // AD result of measured voltage
	float    current;                              // AD result of measured current
	int      counter;                              // A counter value
} T_MEAS;

osPoolDef(mpool, 16, T_MEAS);                    // Define memory pool
osPoolId  mpool;
osMessageQDef(MsgBox, 16, T_MEAS);              // Define message queue
osMessageQId  MsgBox;

static void send_thread(void const* argument);         // forward reference
static void recv_thread(void const* argument);         // forward reference
												 // Thread definitions
/*-----------------------------------------------------------*/

int main( int argc, char **argv  )
{
	/* Create one of the two tasks. */
	xTaskCreate(HardwareTimerInterruption_Immitate, "Timer Interrupt", 100, NULL, 1, NULL);
	init_simulatePROCESSOR_MODES(); //!for using cmsis_os funcs
	/* Install the handler for the software interrupt.  The syntax necessary
		to do this is dependent on the FreeRTOS port being used.  The syntax
		shown here can only be used with the FreeRTOS Windows port, where such
		interrupts are only simulated. */
	vPortSetInterruptHandler(mainINTERRUPT_NUMBER, ulExampleInterruptHandler);
	vPortSetInterruptHandler(timerINTERRUPT_NUMBER, ulTimerInterruptHandler);

	mpool = osPoolCreate(osPool(mpool));                 // create memory pool
	MsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);  // create msg queue

	osThreadDef(send, send_thread, osPriorityNormal, 1, 400);
	osThreadDef(recv, recv_thread, osPriorityNormal, 1, 2000);
	tid_thread1 = osThreadCreate(osThread(send), NULL);
	tid_thread2 = osThreadCreate(osThread(recv), NULL);

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();	

	/* The following line should never be reached because vTaskStartScheduler() 
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return;
}
/*-----------------------------------------------------------*/

//
//  Thread 1: Send thread
//
static void send_thread(void const* argument) {
	T_MEAS* mptr;

	for (;;) {
		mptr = osPoolAlloc(mpool);                     // Allocate memory for the message
		mptr->voltage = 223.72;                        // Set the message content
		mptr->current = 17.54;
		mptr->counter = 120786;
		osMessagePut(MsgBox, (uint32_t)mptr, osWaitForever);  // Send Message
		osDelay(100);

		mptr = osPoolAlloc(mpool);                     // Allocate memory for the message
		mptr->voltage = 227.23;                        // Prepare a 2nd message
		mptr->current = 12.41;
		mptr->counter = 170823;
		osMessagePut(MsgBox, (uint32_t)mptr, osWaitForever);  // Send Message
	}
	//osThreadYield();                               // Cooperative multitasking
												   // We are done here, exit this thread
}

//
//  Thread 2: Receive thread
//
static void recv_thread(void const* argument) {
	T_MEAS* rptr;
	osEvent  evt;

	for (;;) {
		evt = osMessageGet(MsgBox, osWaitForever);  // wait for message
		if (evt.status == osEventMessage) {
			rptr = evt.value.p;
			printf("\nVoltage: %.2f V\n", rptr->voltage);
			printf("Current: %.2f A\n", rptr->current);
			printf("Number of cycles: %d\n", rptr->counter);
			osPoolFree(mpool, rptr);                  // free memory allocated for message
		}
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

extern void vApplicationIdleHook(void)
{
	//vPrintString("Idle task\n");
	return;
}

extern void vApplicationDaemonTaskStartupHook(void)
{
	return;
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