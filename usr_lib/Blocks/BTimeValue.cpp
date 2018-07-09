#include "BTimeValue.h"

BTimeValue::BTimeValue(BlockSystem* system, BRtc* rtc, uint8_t timeslots_count, uint8_t mode_flags)
: system(system), timeslots_count(timeslots_count), timeslots(NULL), last_output_value(0), flags(mode_flags)
{
    this->rtc = rtc;
    output[0] = B_PARAM_VALUE_BAD;
    this->timeslots = (timeslots_count > 0) ? new Timeslot[timeslots_count] : NULL;
    
#if B_TIMEVALUE_RESOLUTION_SECOND
    system->addEventListener(this, B_EVT_RTC_SECOND);
#else
    system->addEventListener(this, B_EVT_RTC_MINUTE);
#endif
}


void BTimeValue::onEvent(event_t event_flags)
{
    if (!(event_flags & B_EVT_RTC_SECOND)) return;

    if (this->timeslots == NULL) return;

#if B_TIMEVALUE_RESOLUTION_SECOND
    uint32_t now = rtc->hour * 3600 + rtc->minute * 60 + rtc->second;
#else
    uint16_t now = rtc->hour * 60 + rtc->minute;
#endif
    
    //If 'now' is less than first 'timeslot' than output[0] is value of the last 'timeslot'
    if (now < this->timeslots[0].time) {
        output[0] = this->timeslots[timeslots_count-1].value;
        goto out;
    }
    //Otherwise find the right timeslot.
    for (int8_t i = this->timeslots_count-1; i >= 0; i--) {
        if (now >= this->timeslots[i].time) {
            output[0] = this->timeslots[i].value;
            goto out;
        }
    }
    return;

out:
    if (output[0] != last_output_value) {
        last_output_value = output[0];
//Serial.print("BTimeValue output change ");
//Serial.println(output[0]);
        if (flags & B_TIMEVALUE_SEND_EVENT) system->setEvent(B_EVT_TIMEVALUE_CHANGE);
        if (job_follow != NULL) BlockSpider(job_follow);
    }

    return;
}
