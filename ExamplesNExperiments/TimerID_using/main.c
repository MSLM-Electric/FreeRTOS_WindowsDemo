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

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* The number of the simulated interrupt used in this example.  Numbers 0 to 2
are used by the FreeRTOS Windows port itself, so 3 is the first number available
to the application. */
#define mainINTERRUPT_NUMBER	3

#define ONE_SHOT_TIMER 0
#define PERIODIC_TIMER 1

/* The task functions. */
void vPacketTimeoutTask( void *pvParameters );
void vPacketSendRecvStartProcessTask( void *pvParameters );
/* The software timer used to turn the backlight off. */
static TimerHandle_t xSNTP_RXTimeoutHandle = NULL;
static TimerHandle_t xTransmitHandle = NULL;
static TimerHandle_t xSomeReceiveProcHandle = NULL;
static TimerHandle_t xSomeTransmitProcHandle = NULL;

/* The service routine for the (simulated) interrupt.  This is the interrupt
that the task will be synchronized with. */
static uint32_t ulExampleInterruptHandler(void);
static void sntpRXtimer_callback(TimerHandle_t timer);
static void transmitTimer_callback(TimerHandle_t timer);
static void someAnotherRXTXprocessTimer_callback(TimerHandle_t timer);
static void someProcessInitReset(void);

typedef struct {
	uint8_t retry;
	uint8_t transmitting;
	uint8_t receiving;
}somePcb_t;
somePcb_t somePcb;

/*-----------------------------------------------------------*/

int main( void )
{
	/* Create one of the two tasks. */
	xTaskCreate(vPacketTimeoutTask,		/* Pointer to the function that implements the task. */
					"PacketTimeout Task",	/* Text name for the task.  This is to facilitate debugging only. */
					1000,		/* Stack depth - most small microcontrollers will use much less stack than this. */
					NULL,		/* We are not using the task parameter. */
					1,			/* This task will run at priority 1. */
					NULL );		/* We are not using the task handle. */

	/* Create the other task in exactly the same way. */
	xTaskCreate(vPacketSendRecvStartProcessTask, "PacketSendRecvStartProcess Task", 1000, NULL, 1, NULL );
	xSNTP_RXTimeoutHandle = xTimerCreate((char *)"SNTP_RecvTimer", 200, ONE_SHOT_TIMER, &xSNTP_RXTimeoutHandle, sntpRXtimer_callback);
	xTransmitHandle = xTimerCreate((char*)"TransmitTimer", 1000, PERIODIC_TIMER, 0, transmitTimer_callback);
	xSomeTransmitProcHandle = xTimerCreate("Some transmitt process timer", 100, PERIODIC_TIMER, &xSomeTransmitProcHandle, someAnotherRXTXprocessTimer_callback);
	xSomeReceiveProcHandle = xTimerCreate("Some receive process timer", 700, ONE_SHOT_TIMER, &xSomeReceiveProcHandle, someAnotherRXTXprocessTimer_callback);
	if (xSNTP_RXTimeoutHandle == NULL) {
		; //bad
	}else{
		xTimerStop(xSNTP_RXTimeoutHandle, portMAX_DELAY);
	}
	/* Install the handler for the software interrupt.  The syntax necessary
		to do this is dependent on the FreeRTOS port being used.  The syntax
		shown here can only be used with the FreeRTOS Windows port, where such
		interrupts are only simulated. */
	vPortSetInterruptHandler(mainINTERRUPT_NUMBER, ulExampleInterruptHandler);
	static char sntpmsg[] = "SNTP Timer ring!\n";
	vTimerSetTimerID(xSNTP_RXTimeoutHandle, sntpmsg);
	xTimerStart(xSNTP_RXTimeoutHandle, 0);
	xTimerStart(xTransmitHandle, 0);
	someProcessInitReset();

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();	

	/* The following line should never be reached because vTaskStartScheduler() 
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

void vPacketTimeoutTask( void *pvParameters )
{
const char *pcTaskName = "Task PacketTimeout is running\r\n";
volatile uint32_t ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		vPrintString( pcTaskName );

		/* Delay for a period. */
		vTaskDelay(500);
		//xTimerReset(xSNTP_RXTimeoutHandle, 0);
	}
}
/*-----------------------------------------------------------*/

void vPacketSendRecvStartProcessTask( void *pvParameters )
{
const char *pcTaskName = "Task PacketSendRecvStartProcess is running\r\n";
volatile uint32_t ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		vPrintString( pcTaskName );

		vTaskDelay(500);
		if (somePcb.transmitting == 0) {
			xTimerStart(xSomeTransmitProcHandle, 0);
			somePcb.transmitting = 1;
		}
	}
}

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

static void sntpRXtimer_callback(TimerHandle_t timer)
{
	if (timer != NULL) {
		void* timID;
		timID = pvTimerGetTimerID(timer);
		if (timID) {
			vPrintString((char*)timID);
		}
	}
	//else
		//;//bad!

	/*to see the timers ID in debug wathclist write:*/
	/* ((Timer_t*)timer)->pvTimerID*/
	/*to see the chars of that load that attached to ID write:*/
	/* (char*)(((Timer_t*)timer)->pvTimerID)*/
	/*or to see the content more detailed write:*/
	/* (char*)(((Timer_t*)timer)->pvTimerID), 10      - for Visual Studio*/
	/* (char*)(((Timer_t*)timer)->pvTimerID)[0, 10]   - for QT Creator*/
}

static void transmitTimer_callback(TimerHandle_t timer)
{
	vPrintString("transmitting!\n");
	xTimerStart(xSNTP_RXTimeoutHandle, 0);
	//xTimerReset(xSNTP_RXTimeoutHandle, 0); //alternative
}

static void someAnotherRXTXprocessTimer_callback(TimerHandle_t timer)
{
	if (timer != NULL) {		
		if (timer == xSomeTransmitProcHandle) {
			somePcb.transmitting = 1;
			somePcb.retry--;
			vPrintString("some Transmit process!\n");
			if (somePcb.retry == 0) {
				xTimerStop(xSomeTransmitProcHandle, 0);
				xTimerStart(xSomeReceiveProcHandle, 0);
			}
		}
		else if (timer == xSomeReceiveProcHandle) {
			somePcb.receiving = 1;
			somePcb.transmitting = 0;
			vPrintString("some Receiving process!\n");
			someProcessInitReset();
			//xTimerStop(xSomeTransmitProcHandle, 0);
		}
	}

}

static void someProcessInitReset(void) {
	memset(&somePcb, 0, sizeof(somePcb));
	somePcb.retry = 3;
}

int CreateSomeAnotherTasks(uint8_t tasksQnty)
{
	return 0;
}
