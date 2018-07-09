#include "BAnalogIn.h"

BAnalogIn::BAnalogIn(uint8_t arduino_pin) : arduino_pin(arduino_pin)
{
}

bool BAnalogIn::trigger(uint8_t type)
{
//Serial.print("BAnalogIn::trigger ");

    output[0] = analogRead(this->arduino_pin);
    output[1] = output[0] - (float)this->threshold;
//Serial.println(output[0]);

    if (job_follow != NULL) {
        BlockSpider(job_follow);
    }
    return true;
}
