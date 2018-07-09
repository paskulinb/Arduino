#include "BDigiOut.h"

BDigiOut::BDigiOut(uint8_t arduino_pin, bool invert) : arduino_pin(arduino_pin), invert(invert)
{
    pinMode(arduino_pin, OUTPUT);
    //Set initial output state
    digitalWrite(this->arduino_pin, (invert) ? HIGH : LOW);
}

bool BDigiOut::trigger(uint8_t type)
{
//Serial.print("BDigiOut::trigger "); Serial.println(((*input > 0) ? "HIGH" : "LOW"));

    digitalWrite(this->arduino_pin, ((*input > 0) ? (invert ? LOW : HIGH) : (invert ? HIGH : LOW)));
    return true;
}

void BDigiOut::setOut(bool state)
{
    digitalWrite(this->arduino_pin, ((state) ? (invert ? LOW : HIGH) : (invert ? HIGH : LOW)));
}
