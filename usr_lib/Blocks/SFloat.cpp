#include "SFloat.h"
#include <EEPROM.h>

SFloat::SFloat(const char* s_name, float* curren_value, const float* default_value, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), curren_value(curren_value), default_value(default_value)
{
}

void SFloat::set(float new_value)
{
    *curren_value = new_value;
    save();
}

void SFloat::set(char* string)
{
    *curren_value = atof(string);
    save();
}

void SFloat::setDefault(void)
{
    *curren_value = pgm_read_float((float*)default_value);
    save();
}

uint16_t SFloat::nextEEAddr(void)
{
    return (this->ee_addr + sizeof(float));
}


void SFloat::save(void)
{
    EEPROM.put(ee_addr, *curren_value);
}

void SFloat::restore(void)
{
    EEPROM.get(ee_addr, *curren_value);
}


void SFloat::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(F(" "));
    Serial.print(*curren_value);
}


