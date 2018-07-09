#ifndef SRtcTime_h
#define SRtcTime_h

#include "SettingBase.h"
#include "BRtc.h"

class SRtcTime : public SettingBase
{
public:
    BRtc* rtc;
public:
    SRtcTime(const char* s_name, BRtc* rtc, uint16_t ee_addr);
    void set(char* string);
    void restore(void);
    void save(void);
    void setDefault(void);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
