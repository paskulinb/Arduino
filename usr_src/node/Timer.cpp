#include "Timer.h"

Timer::Timer(void) : flags(0), callback(NULL)
{
}

void Timer::setInterval(unsigned long interval, void (* callback)(void))
{
	millis_interval = interval;
	millis_downcounter = interval;
	millis_last = millis();
	
	if (callback != NULL) {
		this->callback = callback;
		flags |= TMRFLAGS_ENABLE_CALLBACK;
	}
	
	flags |= TMRFLAGS_RUNNING;
}

void Timer::setTimeout(unsigned long timeout, void (* callback)(void))
{
	flags |= TMRFLAGS_SINGLESHOT;
	
	setInterval(timeout, callback);
}

void Timer::update(void)
{
	if (!(flags & TMRFLAGS_RUNNING)) return;
	
	unsigned long now = millis();
	
	long tdif = now - millis_last;
	
	if (tdif < 0) tdif = ULONG_MAX + tdif; //time overflow handling (arduino millis() overflows on every ~50 days)
	
	millis_last = now;
	
	if (millis_downcounter > tdif) millis_downcounter -= tdif;
	else {                                   //interval finished

		if (flags & TMRFLAGS_SINGLESHOT) {   //if Singleshot?

			stop();
			return;
		}

		millis_downcounter = millis_interval; //reset counter

		if (flags & TMRFLAGS_ENABLE_CALLBACK) this->callback();
	}
}

void Timer::enableCallback(void)
{
	flags |= TMRFLAGS_ENABLE_CALLBACK;
}

void Timer::disableCallback(void)
{
	flags &= ~TMRFLAGS_ENABLE_CALLBACK;
}

void Timer::stop(void)
{
	flags &= ~TMRFLAGS_RUNNING;
}

