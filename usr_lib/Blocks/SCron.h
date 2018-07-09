#ifndef SCron_h
#define SCron_h

#include "SettingBase.h"
#include "BCron.h"

#define S_CRON_STRING_MAX_LENGTH 30

class SCron : public SettingBase
{
public:
    BCron* cron;
public:
    SCron(const char* s_name, BCron* cron, uint16_t ee_addr);
    void set(char* string);
    void restore(void);
    void save(void);
    void setDefault(void);
    uint16_t nextEEAddr(void);
    void print(void);
};
#endif
