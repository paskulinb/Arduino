#ifndef STimeValue_h
#define STimeValue_h

#include "SettingBase.h"
#include "BTimeValue.h"

class STimeValue : public SettingBase
{
public:
    BTimeValue* btv;
public:
    STimeValue(const char* s_name, BTimeValue* btv, uint16_t ee_addr);
    void restore(void);
    void save(void);
    void setDefault(void);
    void set(uint8_t p_idx, uint8_t hour, uint8_t minute, uint8_t second, float value);
    void set(char* string);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
