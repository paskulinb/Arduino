#ifndef SFloat_h
#define SFloat_h

#include "SettingBase.h"

class SFloat : public SettingBase
{
public:
    float* curren_value;
    const void* default_value;  //PROGMEM
public:
    SFloat(const char* s_name, float* curren_value, const float* default_value, uint16_t ee_addr);
    void restore(void);
    void save(void);
    void setDefault(void);
    void set(float new_value);
    void set(char* string);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
