#ifndef SPulseValue_h
#define SPulseValue_h

#include "SettingBase.h"
#include "BPulseValue.h"

class SPulseValue : public SettingBase
{
public:
    BPulseValue* bpv;
public:
    SPulseValue(const char* s_name, BPulseValue* bpv, uint16_t ee_addr);
    void restore(void);
    void save(void);
    void setDefault(void);
    void set(uint8_t p_idx, uint16_t duration, float level);
    void set(char* string);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
