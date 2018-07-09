#include "SRtcTime.h"
#include <EEPROM.h>

SRtcTime::SRtcTime(const char* s_name, BRtc* rtc, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), rtc(rtc)
{
}
void SRtcTime::setDefault(void)
{
    rtc->hour = 0;
    rtc->minute = 0;
    rtc->second = 0;
    rtc->is_set |= B_RTC_TIME_SET;
}

uint16_t SRtcTime::nextEEAddr(void)
{
    return (this->ee_addr + 3 /* * sizeof(uint8_t)*/);
}


void SRtcTime::set(char* string)
{
    char *c = string;

    if ((c = strtok(c, ":")))    rtc->hour = atoi(c);
    if ((c = strtok(NULL, ":"))) rtc->minute = atoi(c);
    if ((c = strtok(NULL, ":"))) rtc->second = atoi(c);

    rtc->is_set |= B_RTC_TIME_SET;
}


void SRtcTime::save(void)
{
    //uint8_t i = 0;
    EEPROM.put(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->second);
    EEPROM.put(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->minute);
    EEPROM.put(ee_addr + 2 /*sizeof(uint8_t) * i  */,   rtc->hour);
}

void SRtcTime::restore(void)
{
    //uint8_t i = 0;
    EEPROM.get(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->second);
    EEPROM.get(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->minute);
    EEPROM.get(ee_addr + 2 /*sizeof(uint8_t) * i  */, rtc->hour);
    rtc->is_set |= B_RTC_TIME_SET;
}


void SRtcTime::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(F(" "));

	Serial.print(rtc->hour);
	Serial.print(":");
	Serial.print(rtc->minute);
	Serial.print(":");
	Serial.print(rtc->second);
	Serial.print(F(" (h.m.s)"));
}

