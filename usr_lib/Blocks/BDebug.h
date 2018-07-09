/* 
 * 
*/

#ifndef BDebug_h
#define BDebug_h

#include <Arduino.h>
#include "BlockBase.h"

class BDebug : public BlockBase
{
public:
    BDebug(void);
    bool trigger(uint8_t type = 0, BlockBase* caller = NULL);
    void onTimer(void);
};
#endif
