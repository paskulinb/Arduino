#ifndef Timer_H
#define Timer_H

#include <Arduino.h>

#define ULONG_MAX 0xffffffff

#define TMRFLAGS_SINGLESHOT        0x01
#define TMRFLAGS_RUNNING           0x02
#define TMRFLAGS_ENABLE_CALLBACK   0x04

class Timer
{
	uint8_t flags;
	unsigned long millis_downcounter;
	unsigned long millis_interval;
	unsigned long millis_last;
	void (* callback)(void);
	
public:
	Timer(void);
	void setInterval(unsigned long interval, void (* callback)(void) = NULL);
	void setTimeout(unsigned long timeout, void (* callback)(void) = NULL);
	inline void enableCallback(void);
	inline void disableCallback(void);
	inline void stop(void);
	void update(void);
};

#endif
