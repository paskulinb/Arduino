#include "EventManager.h"

EventManager::EventManager(void) : evtFlags(0), evtHandlerIdx(0), eventHandler({NULL})
{
	//evtFlags = 0;
	//evtHandlerIdx = 0;
	//for (uint8_t i = 0; i < 16; i++) eventHandler[i] = NULL;
}

void EventManager::setEvent(uint16_t evt)
{
	evtFlags |= evt;
}

void EventManager::resetEvent(uint16_t evt)
{
	evtFlags &= ~evt;
}

void EventManager::handleEvents(void)
{
	while (evtFlags) {
		
		for (evtHandlerIdx = 0; evtHandlerIdx <= 15; evtHandlerIdx++) {
			
			uint16_t evt = 1 << evtHandlerIdx;
			resetEvent(evt);
			if ((evtFlags & evt) && (eventHandler[evtHandlerIdx] != NULL)) eventHandler[evtHandlerIdx]();
		}
	}
}

void EventManager::attachEventHandler(uint8_t index, void (*EVH)(void))
{
	eventHandler[index] = EVH;
}

void EventManager::detachEventHandler(uint8_t index)
{
	eventHandler[index] = NULL;
}
