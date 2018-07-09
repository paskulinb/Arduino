/* 
 * 
*/

#ifndef BTimeValue_h
#define BTimeValue_h

#include <stddef.h>
#include <inttypes.h>
#include "BlockBase.h"
#include "BRtc.h"

#define B_TIMEVALUE_RESOLUTION_SECOND 0

#define B_TIMEVALUE_SEND_EVENT  0x01

/** The daily scheduler for the ouput \b value.
 * User can choose (before compilation) between two different time precisions: \b seconds or \b minutes.
 * If the constant B_TIMEVALUE_RESOLUTION_SECOND == 1, class will be compiled so the \b secons will be the time precision.
 * If the constant B_TIMEVALUE_RESOLUTION_SECOND == 0, the \b minutes will be the time precision. In this case some memory and processing power will be saved.
 * */

class BTimeValue : public BlockBase
{
private:
    BlockSystem* system;
    BRtc* rtc;
public:
    struct Timeslot
    {
#if B_TIMEVALUE_RESOLUTION_SECOND
        uint32_t time; //seconds of day
#else
        uint16_t time; //minutes of day
#endif
        float value;
    };

    uint8_t timeslots_count;
    Timeslot* timeslots;
    float output[1];
private:
    float last_output_value;
    uint8_t flags;

public:
    /** Constructor.
	 *  @param [in] system Pointer to BlockSystem object.
	 *  @param [in] rtc Pointer to BRtc object.
	 *  @param [in] timeslots_count Number of time divisions.
	 *  @param [in] mode_flags If \c B_TIMEVALUE_SEND_EVENT is set, the B_EVT_TIMEVALUE_CHANGE event will be signaled on every output value change.
     * */
    BTimeValue(BlockSystem* system, BRtc* rtc, uint8_t timeslots_count, uint8_t mode_flags = 0);
    
    /** Class automatically registers to systems event table.*/
    void onEvent(event_t event_flags);
};

#endif
