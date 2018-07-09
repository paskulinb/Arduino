#include <Arduino.h>
#include <avr/wdt.h>
#include "BlockSystem.h"


BlockTimer::BlockTimer(BlockBase* listener, uint8_t type)
: flags(0), listener(listener), callback(NULL), next(NULL)
{
    if (type == B_TMR_TYPE_JOB) {
        flags |= B_TMR_FLAGS_JOB;
    }
    if (type == B_TMR_TYPE_JOB_FOLLOW) {
        flags |= B_TMR_FLAGS_JOB_FOLLOW;
    }
}

BlockTimer::BlockTimer(void (* callback)(void))
: flags(0), listener(NULL), callback(callback), next(NULL)
{
}

void BlockTimer::start(unsigned long interval_ms, uint8_t mode)
{
//Serial.print("Timer::start ");
//Serial.println(interval_ms);

    if (mode == B_TMR_MODE_SINGLESHOT)
        flags |= B_TMR_FLAGS_SINGLESHOT;

	millis_interval = interval_ms;
	millis_start = millis();
	
	flags |= B_TMR_FLAGS_RUNNING;
}

void BlockTimer::stop(void)
{
	flags &= ~B_TMR_FLAGS_RUNNING;
}

inline bool BlockTimer::isJob(void)
{
    return (flags & B_TMR_FLAGS_JOB);
}

inline bool BlockTimer::isJobFollow(void)
{
    return (flags & B_TMR_FLAGS_JOB_FOLLOW);
}
/*********************************************************************/


BlockSystem::BlockSystem(void)
: timers_llst(NULL), event_flags(0), event_flags_waiting(0), event_listener_first(NULL)
{
    millis_last = millis();
}

/*BlockSystem::~BlockSystem(void)
{
}*/

void BlockSystem::addEventListener(BlockBase* listener, event_t catching_events)
{
    //no event listener yet
    if (this->event_listener_first == NULL) {
        this->event_listener_first = listener;
        listener->catching_events = catching_events;
        return;
    }
    //find last event listener in the chain
    BlockBase* last = this->event_listener_first;
    while (last->next_event_listener != NULL) last = last->next_event_listener;
    last->next_event_listener = listener;
    listener->next_event_listener = NULL; //make sure to be 'next_event_listener' of the last one equal to NULL
    listener->catching_events = catching_events;
    return;
}


BlockTimer* BlockSystem::newTimer(BlockBase* listener, uint8_t type)
{
    //no timer yet
    if (this->timers_llst == NULL) {
        this->timers_llst = new BlockTimer(listener, type);
        return this->timers_llst; //this is first timer in the chain
    }
    //find last timer in the chain
    BlockTimer* last = this->timers_llst;
    while (last->next != NULL) last = last->next;
    last->next = new BlockTimer(listener, type);
    return last->next;
}

BlockTimer* BlockSystem::newTimer(void (* callback)(void))
{
    //no timer yet
    if (this->timers_llst == NULL) {
        this->timers_llst = new BlockTimer(callback);
        return this->timers_llst; //this is first timer in the chain
    }
    //find last timer in the chain
    BlockTimer* last = this->timers_llst;
    while (last->next != NULL) last = last->next;
    last->next = new BlockTimer(callback);
    return last->next;
}

void BlockSystem::setEvent(event_t event_flag)
{
    this->event_flags_waiting |= event_flag;
}


void BlockSystem::update(void)
{
    /* TIMERS */
    BlockTimer* tmr = this->timers_llst; //First timer in the chain
    if (tmr != NULL) {
        do {
            if (!(tmr->flags & B_TMR_FLAGS_RUNNING)) continue; //next timer

            unsigned long now = millis();

            if ((now < tmr->millis_start /*millis overflow*/) && (((ULONG_MAX - tmr->millis_start) + now) >= tmr->millis_interval)
                || ((now - tmr->millis_start) >= tmr->millis_interval)
            ) {
                //Timeout occured (interval finished)
                if (tmr->flags & B_TMR_FLAGS_SINGLESHOT) {   //if Singleshot?
                    tmr->stop();
                } else {
                    tmr->millis_start = now; //restart
                }
//Serial.print("timer::Timeout "); Serial.println(tmr->millis_downcounter);
                
                //Call corresponding job (event handler)
                if (tmr->listener != NULL) {  //Have owner block
                    
                    if (tmr->isJob()) { //Enter JOB only mode; call trigger() instead onTimer()

                        tmr->listener->trigger(B_TRIGG_JOB);
                        
                    } else if (tmr->isJobFollow()) { //Enter JOB-FOLLOWING mode; call trigger() instead onTimer()
                        
                        BlockSpider(tmr->listener);
                        
                    } else {
                        tmr->listener->onTimer();
                    }
                } else if (tmr->callback != NULL) { //Have callback registered
                    tmr->callback();
                }
            }
            
        } while ((tmr = tmr->next) != NULL);
    }
    
    /* EVENTS */
    BlockBase* current;

    if (this->event_flags_waiting != 0) {
        this->event_flags = this->event_flags_waiting;
        this->event_flags_waiting = 0;
//Serial.print("Sys proc events "); Serial.println(this->event_flags, BIN);
        current = this->event_listener_first;
        while (current != NULL) {
            current->onEvent(event_flags);
            current = current->next_event_listener;
        }
        this->event_flags = 0; //reset all events
    }

}

void BlockSystem::reboot(void)
{
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (true) {}
}
