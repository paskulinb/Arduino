#include "BlockBase.h"
//#include <Arduino.h>

BlockBase::BlockBase(void)
: job_follow(NULL), next_event_listener(NULL), catching_events(0)
{
}


bool BlockBase::trigger(uint8_t type)
{
    /* 1. Take input parameters
     * 2. Do the job
     */
    return true;
}

void BlockBase::onTimer(void)
{
    return;
}

void BlockBase::onEvent(uint16_t event_flags)
{
    if (!(event_flags & this->catching_events)) return;
    
    
    return;
}

void BlockSpider(BlockBase* at)
{
    do {
//Serial.print("BlockSpider::at "); Serial.println((uint16_t)at);
        if (!at->trigger(B_TRIGG_JOB)) break; //If trigger method returns FALSE, the calling job was not finished. Spider STOP at this block!
        at = at->job_follow;
    } while (at);
}
