#include "BRegulator.h"

BRegulator::BRegulator(uint8_t regulation_mode)
:  mode(regulation_mode), input(NULL), setpoint(NULL), hysteresis(0)
{
}

bool BRegulator::trigger(uint8_t type)
{
//Serial.println("BRegulator::trigger");
    job();
    return true;
}


void BRegulator::onEvent(event_t event_flags)
{
    if (catching_events & event_flags) {
        job();
        if (job_follow != NULL) BlockSpider(job_follow);
    }
    return;
}

void BRegulator::job(void)
{
    if (input == NULL || setpoint == NULL) return;

    /* algo */
    switch (mode) {

        case B_REG_MODE_2LEVEL:
            if ((*input + hysteresis) < *setpoint) {
                output[0] = 1;
                break;
            }
            if ((*input - hysteresis) > *setpoint) {
                output[0] = 0;
                break;
            }
            //else stay unchanged

//      case BREG_MODE_PID:
//          calcPID();
    }
/*
Serial.print("s=");
Serial.print(*setpoint);
Serial.print(" i=");
Serial.print(*input);
Serial.print(" o=");
Serial.println(output[0]);
*/
    return;
}


/*void BRegulator::calcPID(void)
{
    return;
}*/

