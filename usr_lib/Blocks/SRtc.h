#ifndef SRtc_h
#define SRtc_h

#include "SettingBase.h"
#include "BRtc.h"

class SRtc : public SettingBase
{
public:
    BRtc* rtc;
public:
    SRtc(const char* s_name, BRtc* rtc, uint16_t ee_addr);
    void set(char* string);
    void restore(void);
    void save(void);
    void setDefault(void);
    void setDate(char* string);
    void setTime(char* string);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
