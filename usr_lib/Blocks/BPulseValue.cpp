#include "BPulseValue.h"

BPulseValue::BPulseValue(BlockSystem* system, uint8_t pulses_count, uint8_t mode_flags, float final_level)
: system(system), pulses_count(pulses_count), p_idx(0), final_level(final_level), flags(mode_flags)
{
    output[0] = 0; //initial value
    timer = system->newTimer(this, B_TMR_TYPE_ONTIMER);
    this->pulses = (pulses_count > 0) ? new SPulse[pulses_count] : NULL;
}


bool BPulseValue::trigger(uint8_t type)
{
//Serial.print("Pulse trigger ");

    if ((flags & B_PULSE_FLAG_RUNNING)) return false; //prevent restart through trigger
    if (pulses == NULL) return true;  //will not start -> job finished
    if ((pulses /*+0*/)->duration == 0)  return true; //prevent runnig zero duration

    //Start waveform
    p_idx = 0;
    
    outputChange((pulses /*+0*/)->level, (pulses /*+0*/)->duration);

    flags |= B_PULSE_FLAG_RUNNING;

//Serial.print((pulses /*+0*/)->duration);
//Serial.print("/");
//Serial.println(output[0],2);
    
    return false; //job not finished
}


void BPulseValue::onTimer(void)
{
//Serial.print("Pulse onTimer ");

next_pulse:

    if (++p_idx == pulses_count) { //IF END OF SEQUENCE
        
        if ((flags & B_PULSE_FLAG_CONTINUOUS)) {
            //repeat sequence
            p_idx = 0;
        } else {
            //stop
            flags &= ~B_PULSE_FLAG_RUNNING;
            timer->stop();
            if (final_level != B_PULSE_FINAL_LEVEL) {
                
                outputChange(final_level, 0);
            }
            return;
        }
    }
    
    if ((pulses + p_idx)->duration == 0) goto next_pulse; //prevent running zero duration

    outputChange((pulses + p_idx)->level, (pulses + p_idx)->duration);
    
//Serial.print((pulses + p_idx)->duration);
//Serial.print("/");
//Serial.println(output[0],2);

    return;
}

void BPulseValue::outputChange(float level, uint16_t duration)
{
    output[0] = level;

    if (flags & B_PULSE_FLAG_SEND_EVENT) system->setEvent(B_EVT_PULSE_CHANGE);
    if (job_follow != NULL) {
//Serial.println("pulse job follow");
        BlockSpider(job_follow);
    }
    
    if (duration > 0) timer->start(duration*1000, B_TMR_MODE_SINGLESHOT);
}
