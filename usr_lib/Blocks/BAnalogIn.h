#ifndef BAnalogIn_h
#define BAnalogIn_h

#include <Arduino.h>
#include "BlockBase.h"

/** Arduino Analog Input.
*/
class BAnalogIn : public BlockBase
{
    uint8_t arduino_pin;

public:
    /** Analog \c input value (output[0]) and difference from threshold (output[1])
     * @see trigger() method */
    float output[2];
    int threshold;

    /** Constructor.
     * @param [in] arduino_pin The Arduino pin number to be set to analog input mode.*/
    BAnalogIn(uint8_t arduino_pin);
    /** Read the analog input value and store to output[0].*/
    bool trigger(uint8_t type = B_TRIGG_JOB);
};
#endif
