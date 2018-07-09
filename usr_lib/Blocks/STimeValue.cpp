#include "STimeValue.h"
#include <EEPROM.h>

STimeValue::STimeValue(const char* s_name, BTimeValue* btv, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), btv(btv)
{
}

void STimeValue::set(uint8_t p_idx, uint8_t hour, uint8_t minute, uint8_t second, float value)
{
#if B_TIMEVALUE_RESOLUTION_SECOND
    btv->timeslots[p_idx].time = hour * 3600 + minute * 60 + second;
#else
    btv->timeslots[p_idx].time = hour * 60 + minute;
#endif
    btv->timeslots[p_idx].value = value;
    save();
}

void STimeValue::setDefault(void)
{
    for (uint8_t i = 0; i < btv->timeslots_count; i++) {
#if B_TIMEVALUE_RESOLUTION_SECOND
        btv->timeslots[i].time = i*3600;
#else
        btv->timeslots[i].time = i*60;
#endif
        btv->timeslots[i].value = 0;
    }
    save();
}

uint16_t STimeValue::nextEEAddr(void)
{
    return (this->ee_addr + btv->timeslots_count*sizeof(BTimeValue::Timeslot));
}

void STimeValue::set(char* string)
{
    char *c = string;
    uint8_t i = 0;
#if B_TIMEVALUE_RESOLUTION_SECOND
    uint32_t hour;
    uint16_t minute;
    uint8_t second;
#else
    uint16_t hour;
    uint8_t minute;
#endif
    c = strtok(c, "#");
    while (c) {
        i = atoi(c);
        if (i < btv->timeslots_count) {
            if ((c = strtok(NULL, ":"))) hour = atoi(c);
#if B_TIMEVALUE_RESOLUTION_SECOND
            if ((c = strtok(NULL, ":"))) minute = atoi(c);
            if ((c = strtok(NULL, "/"))) second = atoi(c);
            btv->timeslots[i].time = hour * 3600 + minute * 60 + second;
#else
            if ((c = strtok(NULL, "/"))) minute = atoi(c);
            btv->timeslots[i].time = hour * 60 + minute;
#endif
            if ((c = strtok(NULL, " "))) btv->timeslots[i].value = atof(c);
/*
 * Serial.println(hour);
Serial.println(minute);
#if B_TIMEVALUE_RESOLUTION_SECOND
Serial.println(second);
#endif
Serial.println(btv->timeslots[i].time);
Serial.println(btv->timeslots[i].value);
*/
        }
        c = strtok(NULL, "#");
    }
    save();
}

void STimeValue::save(void)
{
    for (uint8_t i = 0; i < btv->timeslots_count; i++) {
        EEPROM.put(ee_addr + i*sizeof(BTimeValue::Timeslot), btv->timeslots[i]);
    }
}

void STimeValue::restore(void)
{
    for (uint8_t i = 0; i < btv->timeslots_count; i++) {
        EEPROM.get(ee_addr + i*sizeof(BTimeValue::Timeslot), btv->timeslots[i]);
    }
}


void STimeValue::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(F(" "));
    for (uint8_t i = 0; i < btv->timeslots_count; i++) {
        Serial.print(i);
        Serial.print("#");
#if B_TIMEVALUE_RESOLUTION_SECOND
        uint8_t hour = btv->timeslots[i].time / 3600;
        uint8_t minute = (btv->timeslots[i].time / 60) % 60;
        uint8_t second = btv->timeslots[i].time % 60;
        Serial.print(hour);
        Serial.print(":");
        Serial.print(minute);
        Serial.print(":");
        Serial.print(second);
#else
        uint8_t hour = btv->timeslots[i].time / 60;
        uint8_t minute = btv->timeslots[i].time % 60;
        Serial.print(hour);
        Serial.print(":");
        Serial.print(minute);
#endif
        Serial.print("/");
        Serial.print(btv->timeslots[i].value);
        if (i < (btv->timeslots_count - 1)) Serial.print(" ");
    }
}

