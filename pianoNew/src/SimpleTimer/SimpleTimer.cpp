/*
 * SimpleTimer.cpp
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
 */


#include "SimpleTimer.h"


 // Select time function:
 //static inline unsigned long elapsed() { return micros(); }
static inline unsigned long elapsed() { return millis(); }


SimpleTimer::SimpleTimer() {
	unsigned long current_millis = elapsed();

	for (int i = 0; i < MAX_TIMERS; i++) {
		m_enabled[i] = false;
		m_callbacks[i] = nullptr;                   // if the callback pointer is zero, the slot is free, i.e. doesn't "contain" any g_Timer
		m_prev_millis[i] = current_millis;
		m_numRuns[i] = 0;
	}

	m_numTimers = 0;
}


void SimpleTimer::run() {
	int i;
	unsigned long current_millis;

	// get current time
	current_millis = elapsed();

	for (i = 0; i < MAX_TIMERS; i++) {

		m_toBeCalled[i] = DEFCALL_DONTRUN;

		// no callback == no g_Timer, i.e. jump over empty slots
		if (m_callbacks[i]) {

			// is it time to process this g_Timer ?
			// see http://arduino.cc/forum/index.php/topic,124048.msg932592.html#msg932592

			if (current_millis - m_prev_millis[i] >= m_delays[i]) {

				// update time
				//m_prev_millis[i] = current_millis;
				m_prev_millis[i] += m_delays[i];

				// check if the g_Timer callback has to be executed
				if (m_enabled[i]) {

					// "run forever" timers must always be executed
					if (m_maxNumRuns[i] == RUN_FOREVER) {
						m_toBeCalled[i] = DEFCALL_RUNONLY;
					}
					// other timers get executed the specified number of times
					else if (m_numRuns[i] < m_maxNumRuns[i]) {
						m_toBeCalled[i] = DEFCALL_RUNONLY;
						m_numRuns[i]++;

						// after the last run, delete the g_Timer
						if (m_numRuns[i] >= m_maxNumRuns[i]) {
							m_toBeCalled[i] = DEFCALL_RUNANDDEL;
						}
					}
				}
			}
		}
	}

	for (i = 0; i < MAX_TIMERS; i++) {
		switch (m_toBeCalled[i]) {
		case DEFCALL_DONTRUN:
			break;

		case DEFCALL_RUNONLY:
			(*m_callbacks[i])();
			break;

		case DEFCALL_RUNANDDEL:
			(*m_callbacks[i])();
			deleteTimer(i);
			break;
		}
	}
}


// find the first available slot
// return -1 if none found
int SimpleTimer::findFirstFreeSlot() {
	int i;

	// all slots are used
	if (m_numTimers >= MAX_TIMERS) {
		return -1;
	}

	// return the first slot with no callback (i.e. free)
	for (i = 0; i < MAX_TIMERS; i++) {
		if (m_callbacks[i] == nullptr) {
			return i;
		}
	}

	// no free slots found
	return -1;
}


int SimpleTimer::setTimer(long d, timer_callback f, int16_t n) {
	int freeTimer;

	freeTimer = findFirstFreeSlot();
	if (freeTimer < 0) {
		return -1;
	}

	if (f == nullptr) {
		return -1;
	}

	m_delays[freeTimer] = d;
	m_callbacks[freeTimer] = f;
	m_maxNumRuns[freeTimer] = n;
	m_enabled[freeTimer] = true;
	m_prev_millis[freeTimer] = elapsed();

	m_numTimers++;

	return freeTimer;
}


int SimpleTimer::setInterval(long d, timer_callback f) {
	return setTimer(d, f, RUN_FOREVER);
}


int SimpleTimer::setTimeout(long d, timer_callback f) {
	return setTimer(d, f, RUN_ONCE);
}

// delete timer by timer_callback function ptr
void SimpleTimer::deleteTimer(timer_callback f)
{
	for (int i = 0; i < MAX_TIMERS; i++) {
		if (m_callbacks[i] == f) {
			deleteTimer(i);
			return;
		}
	}
}


// clear timers
void SimpleTimer::clearTimers()
{
	for (int i = 0; i < MAX_TIMERS; i++) {
		deleteTimer(i);
	}
}

void SimpleTimer::deleteTimer(int timerId) {
	if (timerId >= MAX_TIMERS) {
		return;
	}

	// nothing to delete if no timers are in use
	if (m_numTimers == 0) {
		return;
	}

	// don't decrease the number of timers if the
	// specified slot is already empty
	if (m_callbacks[timerId] != nullptr) {
		m_callbacks[timerId] = nullptr;
		m_enabled[timerId] = false;
		m_toBeCalled[timerId] = DEFCALL_DONTRUN;
		m_delays[timerId] = 0;
		m_numRuns[timerId] = 0;

		// update number of timers
		m_numTimers--;
	}
}


// function contributed by code@rowansimms.com
void SimpleTimer::restartTimer(int numTimer) {
	if (numTimer >= MAX_TIMERS) {
		return;
	}

	m_prev_millis[numTimer] = elapsed();
}


boolean SimpleTimer::isEnabled(int numTimer) {
	if (numTimer >= MAX_TIMERS) {
		return false;
	}

	return m_enabled[numTimer];
}


void SimpleTimer::enable(int numTimer) {
	if (numTimer >= MAX_TIMERS) {
		return;
	}

	m_enabled[numTimer] = true;
}


void SimpleTimer::disable(int numTimer) {
	if (numTimer >= MAX_TIMERS) {
		return;
	}

	m_enabled[numTimer] = false;
}


void SimpleTimer::toggle(int numTimer) {
	if (numTimer >= MAX_TIMERS) {
		return;
	}

	m_enabled[numTimer] = !m_enabled[numTimer];
}


int SimpleTimer::getNumTimers() {
	return m_numTimers;
}
