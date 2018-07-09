/* 
 * 
*/

#ifndef BPulseValue_h
#define BPulseValue_h

#include <stddef.h>
#include <inttypes.h>
#include "Arduino.h"
#include "BlockBase.h"
#include "BlockSystem.h"

#define B_PULSE_FLAG_CONTINUOUS  0x01
#define B_PULSE_FLAG_SEND_EVENT  0x02
#define B_PULSE_FLAG_RUNNING     0x80

#define B_PULSE_FINAL_LEVEL           NAN  //stay same as last Pulse value

/** Output \b value for a specific \b duration (single pulse or a series of pulses).
 * It can run in \b single-shot or \b continuous mode.\n
 * Choose whether the \c B_EVT_PULSE_CHANGE event will be signaled on every chage of output value.\n
 * If \c job_follow is set, the \c BlockSpider(job_follow) will be called on every change of output value.
 */
class BPulseValue : public BlockBase
{
public:
    struct SPulse
    {
        uint16_t duration; //seconds
        float level;
    };
private:
    BlockSystem* system;
    BlockTimer* timer;
public:
    /** Don't touch! Used by SPulseValue.*/
    SPulse* pulses;
    /** Don't touch! Used by SPulseValue.*/
    uint8_t pulses_count;
private:
    uint8_t p_idx;  //current pulse index
    float final_level;
    uint8_t flags;

public:
    /** Current value is stored in \c output[0].*/
    float output[1];
    
    /** Constructor.
	 *  @param [in] system Pointer to BlockSystem object.
	 *  @param [in] pulses_count Number of different pulses (value/duration) in series.
	 *  @param [in] mode_flags If \c B_PULSE_FLAG_CONTINUOUS is set, the series of pulses will be repeating automatically.
     * If \c B_PULSE_FLAG_SEND_EVENT is set, BPulseValue will signal the \c B_EVT_PULSE_CHANGE event on every chage of output value.
	 *  @param [in] final_level Specifies the output value after pulse series is finished (single-shot mode only).
     */
    BPulseValue(BlockSystem* system, uint8_t pulses_count = 1, uint8_t mode_flags = 0, float final_level = 0);
    /** Start a series of pulses.*/
    bool trigger(uint8_t type = B_TRIGG_JOB);
    /** Used by BlockSystem timer. It controls duration of pulses.*/
    void onTimer(void);
private:
    void outputChange(float level, uint16_t duration = 0);
};

#endif
