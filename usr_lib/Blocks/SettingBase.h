#ifndef SettingBase_h
#define SettingBase_h

#include <Arduino.h>

class SettingBase {
public:
    const char* name;           //PROGMEM
    uint16_t ee_addr;
    //const void* default_value;  //PROGMEM
    //derived class: type_t* value;
public:
    SettingBase(const char* s_name, uint16_t ee_addr);
    virtual void set(char* string) =0;
    virtual void restore(void) =0;
    virtual void save(void) =0;
    virtual void setDefault(void) =0;
    virtual uint16_t nextEEAddr(void) =0;
    virtual void print(void) =0;
};



#define SETTINGS_ALL -1

class Settings {
    SettingBase** settings;
    int8_t count;
public:
    Settings(uint8_t count);
    int8_t find(char* sname);
    void plugin(SettingBase* setting, uint8_t idx);
    void set(int8_t idx, char* string);
    void restore(int8_t idx = SETTINGS_ALL);
    void save(int8_t idx = SETTINGS_ALL);
    void setDefault(int8_t idx = SETTINGS_ALL);
    void print(int8_t idx = SETTINGS_ALL);
};

#endif
