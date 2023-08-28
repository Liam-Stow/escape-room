/*
 * SimpleTimer.h
 *
 * SimpleTimer - A g_Timer library for Arduino.
 * Author: mromani@ottotecnica.com
 * Copyright (c) 2010 OTTOTECNICA Italy
 *
 * This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser
 * General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
 /*
 
 Updated:	08/02/2016
 Author:	netdog@tut.by
 */
#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

typedef void(*timer_callback)(void);

class SimpleTimer {

public:
	// maximum number of timers
	const static int MAX_TIMERS = 20;

	// setTimer() constants
	const static int RUN_FOREVER = 0;
	const static int RUN_ONCE = 1;	


	// constructor
	SimpleTimer();

	// this function must be called inside loop()
	void run();

	// call function f every d milliseconds
	int setInterval(long d, timer_callback f);

	// call function f once after d milliseconds
	int setTimeout(long d, timer_callback f);
	
	// call function f every d milliseconds for n times
	int setTimer(long d, timer_callback f, int n);

	// destroy the specified timer  by function ptr
	void deleteTimer(timer_callback f);

	void clearTimers();
	// destroy the specified g_Timer
	void deleteTimer(int numTimer);

	// restart the specified g_Timer
	void restartTimer(int numTimer);

	// returns true if the specified g_Timer is m_enabled
	boolean isEnabled(int numTimer);

	// enables the specified g_Timer
	void enable(int numTimer);

	// disables the specified g_Timer
	void disable(int numTimer);

	// enables the specified g_Timer if it's currently disabled,
	// and vice-versa
	void toggle(int numTimer);

	// returns the number of used timers
	int getNumTimers();

	// returns the number of available timers
	int getNumAvailableTimers() { return MAX_TIMERS - m_numTimers; };

private:
	// deferred call constants
	//const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
	//const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the g_Timer
	//const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the g_Timer

	enum  DEFFERED_CALL_TYPE : uint8_t
	{
		DEFCALL_DONTRUN,	// don't call the callback function
		DEFCALL_RUNONLY,	// call the callback function but don't delete the g_Timer
		DEFCALL_RUNANDDEL	// call the callback function and delete the g_Timer
	};

	// find the first available slot
	int findFirstFreeSlot();

	// value returned by the millis() function
	// in the previous run() call
	unsigned long m_prev_millis[MAX_TIMERS];

	// pointers to the callback functions
	timer_callback m_callbacks[MAX_TIMERS];

	// delay values
	unsigned long m_delays[MAX_TIMERS];

	// number of runs to be executed for each g_Timer
	int16_t m_maxNumRuns[MAX_TIMERS];

	// number of executed runs for each g_Timer
	int m_numRuns[MAX_TIMERS];

	// which timers are m_enabled
	boolean m_enabled[MAX_TIMERS];

	// deferred function call (sort of) - N.B.: this array is only used in run()
	DEFFERED_CALL_TYPE m_toBeCalled[MAX_TIMERS];

	// actual number of timers in use
	int m_numTimers;
};

