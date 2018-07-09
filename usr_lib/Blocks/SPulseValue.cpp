#include "SPulseValue.h"
#include <EEPROM.h>

SPulseValue::SPulseValue(const char* s_name, BPulseValue* bpv, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), bpv(bpv)
{
}

void SPulseValue::set(uint8_t p_idx, uint16_t duration, float level)
{
    bpv->pulses[p_idx].duration = duration;
    bpv->pulses[p_idx].level    = level;
    save();
}

void SPulseValue::set(char* string)
{
    char *c = string;
    uint8_t i = 0;
    c = strtok(c, "#");
    while (c) {
        i = atoi(c);
        if (i < bpv->pulses_count) {
            if ((c = strtok(NULL, "/"))) bpv->pulses[i].duration = atoi(c);
            if ((c = strtok(NULL, " "))) bpv->pulses[i].level = atof(c);
        }
        c = strtok(NULL, "#");
    }
    save();
}

void SPulseValue::setDefault(void)
{
    for (uint8_t i = 0; i < bpv->pulses_count; i++) {
        bpv->pulses[i].duration = 0;
        bpv->pulses[i].level    = 0;
    }
    save();
}

uint16_t SPulseValue::nextEEAddr(void)
{
    return (this->ee_addr + bpv->pulses_count*sizeof(BPulseValue::SPulse));
}


void SPulseValue::save(void)
{
    for (uint8_t i = 0; i < bpv->pulses_count; i++) {
        EEPROM.put(ee_addr + i*sizeof(BPulseValue::SPulse), bpv->pulses[i]);
    }
}

void SPulseValue::restore(void)
{
    for (uint8_t i = 0; i < bpv->pulses_count; i++) {
        EEPROM.get(ee_addr + i*sizeof(BPulseValue::SPulse), bpv->pulses[i]);
    }
}


void SPulseValue::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(F(" "));

    for (uint8_t i = 0; i < bpv->pulses_count; i++) {
        Serial.print(i);
        Serial.print("#");
        Serial.print(bpv->pulses[i].duration);
        Serial.print("/");
        Serial.print(bpv->pulses[i].level);
        if (i < (bpv->pulses_count - 1)) Serial.print(" ");
    }
}

