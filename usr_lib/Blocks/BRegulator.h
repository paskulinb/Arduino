/* 
 * 
*/

#ifndef BRegulator_h
#define BRegulator_h

#include <Arduino.h>
#include "BlockBase.h"
#include <float.h>

#define B_REG_MODE_2LEVEL 0
#define B_REG_MODE_PID   1

/** 2 level regulator.*/
class BRegulator : public BlockBase
{
    uint8_t mode;
public:
    /** Pointer to \c input variable (regulation process variable)*/
    float* input;
    /** Pointer to \c setpoint variable */
    float* setpoint;
    /** Differece \c hysteresis */
    float hysteresis;
    /** = \c 1 or \c 0*/
    float output[1];

    /** Constructor.
     * @param [in] regulation_mode Only \c B_REG_MODE_2LEVEL is available.*/
    BRegulator(uint8_t regulation_mode = B_REG_MODE_2LEVEL);
    
    /** Read the \c input and \c setpoint values and calculates \c output.*/
    bool trigger(uint8_t type = B_TRIGG_JOB);
    
    /** If BRegulator is bound to an event, it will do the job on \c catching_event.
     * If \c job_follow is set, the \c BlockSpider(job_follow) will be called.*/
    void onEvent(event_t event_flags);

private:
    void job(void);
//    void calcPID(void);
};
#endif
