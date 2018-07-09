#ifndef EventManager_H
#define EventManager_H

#include <Arduino.h>

#define EVT0  0x0001
#define EVT1  0x0002
#define EVT2  0x0004
#define EVT3  0x0008
#define EVT4  0x0010
#define EVT5  0x0020
#define EVT6  0x0040
#define EVT7  0x0080
#define EVT8  0x0100
#define EVT9  0x0200
#define EVT10 0x0400
#define EVT11 0x0800
#define EVT12 0x1000
#define EVT13 0x2000
#define EVT14 0x4000
#define EVT15 0x8000


class EventManager
{
	uint16_t evtFlags;
	uint8_t evtHandlerIdx;
	void (* eventHandler[16])(void);
	
public:
	EventManager(void);
	inline void setEvent(uint16_t evt);
	inline void resetEvent(uint16_t evt);
	inline void attachEventHandler(uint8_t index, void (*EVH)(void));
	inline void detachEventHandler(uint8_t index);
	void handleEvents(void);
};

#endif
