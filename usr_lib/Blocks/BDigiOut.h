#ifndef BDigiOut_h
#define BDigiOut_h

#include <Arduino.h>
#include "BlockBase.h"

/** Arduino Digital Output.
*/
class BDigiOut : public BlockBase
{
    uint8_t arduino_pin;

public:
    /** Pointer \c input will be mapped to digital output.
     * @see trigger() method */
    float* input;
    bool invert;

    /** Constructor.
     * @param [in] arduino_pin The Arduino pin number to be set to output mode and to corresponding value/state.
     * @param [in] invert Invert the output.*/
    BDigiOut(uint8_t arduino_pin, bool invert = false);
    /** Read the input value and set output port.
     * \code{.cpp} arduino_pin = (*input > 0) ? HIGH : LOW \endcode*/
    bool trigger(uint8_t type = B_TRIGG_JOB);
    void setOut(bool state);
};
#endif
