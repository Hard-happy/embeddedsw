/******************************************************************************
*
* Copyright (C) 2015-2017 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMANGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file usleep.c
*
* This function provides a microsecond delay using the Generic counter register in
* the ARM Cortex A53 MPcore.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.2	pkp  	 28/05/15 First release
* 5.4	pkp		 28/01/16 Modified the usleep API to configure Time Stamp
*						  generator only when disable using frequency from
*						  xparamters.h instead of hardcoding
* 5.05	pkp		 13/04/16 Modified usleep routine to call XTime_StartTimer
*						  which enables timer only when it is disabled and
*						  read counter value directly from register instead
*						  of calling XTime_GetTime for optimization
* 6.0   mus      08/18/16 Updated the usleep signature. Fix for CR#956899
* 6.6	srm      10/18/17 Updated sleep routines to support user configurable
*	                  implementation. Now sleep routines will use Timer
*                         specified by the user (i.e. Global timer/TTC timer)
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xpseudo_asm.h"

#if defined (SLEEP_TIMER_BASEADDR)
#include "xil_sleeptimer.h"
#endif

/****************************  Constant Definitions  ************************/
#if defined (SLEEP_TIMER_BASEADDR)
#define COUNTS_PER_USECOND  (COUNTS_PER_SECOND / 1000000)
#else
/* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_USECOND  (COUNTS_PER_SECOND/1000000 )
#endif

/*****************************************************************************/
/**
*
* This API gives a delay in microseconds
*
* @param	useconds requested
*
* @return	0.
*
* @note		None.
*
****************************************************************************/
int usleep_A53(unsigned long useconds)
{

#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(useconds, COUNTS_PER_USECOND);
#else
	XTime tEnd, tCur;
	/* Start global timer counter, it will only be enabled if it is disabled */
	XTime_StartTimer();

	tCur = arch_counter_get_cntvct();
	tEnd = tCur + (((XTime) useconds) * COUNTS_PER_USECOND);
	do
	{
		tCur = arch_counter_get_cntvct();
	} while (tCur < tEnd);
#endif

	return 0;
}
