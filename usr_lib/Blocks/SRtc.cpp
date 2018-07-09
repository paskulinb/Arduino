#include "SRtc.h"
#include <EEPROM.h>

SRtc::SRtc(const char* s_name, BRtc* rtc, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), rtc(rtc)
{
}

void SRtc::setDefault(void)
{
    rtc->day = 1;
    rtc->month = 1;
    rtc->year = 0;
    rtc->hour = 0;
    rtc->minute = 0;
    rtc->second = 0;
    rtc->adjustDayOfTheWeek();
    rtc->is_set |= B_RTC_ALL_SET;
}

uint16_t SRtc::nextEEAddr(void)
{
    return (this->ee_addr + 6 /* * sizeof(uint8_t)*/);
}

void SRtc::setDate(char* string)
{
    char *c = string;

    if ((c = strtok(c, "-")))    rtc->day = atoi(c);
    if ((c = strtok(NULL, "-"))) rtc->month = atoi(c);
    if ((c = strtok(NULL, "-"))) rtc->year = atoi(c);
    
    rtc->is_set |= B_RTC_DATE_SET;
    
    rtc->adjustDayOfTheWeek();

    //save();
}

void SRtc::setTime(char* string)
{
    char *c = string;

    if ((c = strtok(c, ":")))    rtc->hour = atoi(c);
    if ((c = strtok(NULL, ":"))) rtc->minute = atoi(c);
    if ((c = strtok(NULL, ":"))) rtc->second = atoi(c);

    rtc->is_set |= B_RTC_TIME_SET;

    //save();
}


void SRtc::save(void)
{
    //uint8_t i = 0;
    EEPROM.put(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->second);
    EEPROM.put(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->minute);
    EEPROM.put(ee_addr + 2 /*sizeof(uint8_t) * i++*/, rtc->hour);
    EEPROM.put(ee_addr + 3 /*sizeof(uint8_t) * i++*/, rtc->day);
    EEPROM.put(ee_addr + 4 /*sizeof(uint8_t) * i++*/, rtc->month);
    EEPROM.put(ee_addr + 5 /*sizeof(uint8_t) * i  */, rtc->year);
}

void SRtc::restore(void)
{
    //uint8_t i = 0;
    EEPROM.get(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->second);
    EEPROM.get(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->minute);
    EEPROM.get(ee_addr + 2 /*sizeof(uint8_t) * i++*/, rtc->hour);
    EEPROM.get(ee_addr + 3 /*sizeof(uint8_t) * i++*/, rtc->day);
    EEPROM.get(ee_addr + 4 /*sizeof(uint8_t) * i++*/, rtc->month);
    EEPROM.get(ee_addr + 5 /*sizeof(uint8_t) * i  */, rtc->year);
    rtc->adjustDayOfTheWeek();
    rtc->is_set |= B_RTC_ALL_SET;
}


void SRtc::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(F(" "));

	Serial.print(rtc->wday);
	Serial.print(", ");
	Serial.print(rtc->year);
	Serial.print("-");
	Serial.print(rtc->month);
	Serial.print("-");
	Serial.print(rtc->day);
	Serial.print(" ");
	Serial.print(rtc->hour);
	Serial.print("-");
	Serial.print(rtc->minute);
	Serial.print("-");
	Serial.print(rtc->second);
}

