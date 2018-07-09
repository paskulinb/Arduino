#include "SRtcDate.h"
#include <EEPROM.h>

SRtcDate::SRtcDate(const char* s_name, BRtc* rtc, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), rtc(rtc)
{
}

void SRtcDate::setDefault(void)
{
    rtc->day = 1;
    rtc->month = 1;
    rtc->year = 0;
    rtc->adjustDayOfTheWeek();
    rtc->is_set |= B_RTC_DATE_SET;
}

uint16_t SRtcDate::nextEEAddr(void)
{
    return (this->ee_addr + 3 /* * sizeof(uint8_t)*/);
}

void SRtcDate::set(char* string)
{
    char *c = string;

    if ((c = strtok(c, ".")))    rtc->day = atoi(c);
    if ((c = strtok(NULL, "."))) rtc->month = atoi(c);
    if ((c = strtok(NULL, "."))) {
        uint16_t year = atoi(c);
        if (year >= 2000) year -= 2000;
        rtc->year = (uint8_t)year;
    }
    rtc->adjustDayOfTheWeek();
    rtc->is_set |= B_RTC_DATE_SET;
}


void SRtcDate::save(void)
{
    //uint8_t i = 0;
    EEPROM.put(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->day);
    EEPROM.put(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->month);
    EEPROM.put(ee_addr + 2 /*sizeof(uint8_t) * i  */, rtc->year);
}

void SRtcDate::restore(void)
{
    //uint8_t i = 0;
    EEPROM.get(ee_addr   /*+ sizeof(uint8_t) * i++*/, rtc->day);
    EEPROM.get(ee_addr + 1 /*sizeof(uint8_t) * i++*/, rtc->month);
    EEPROM.get(ee_addr + 2 /*sizeof(uint8_t) * i  */, rtc->year);
    rtc->adjustDayOfTheWeek();
    rtc->is_set |= B_RTC_DATE_SET;
}


void SRtcDate::print(void)
{
    char buffer[20];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(" ");

	Serial.print(rtc->day);
	Serial.print(".");
	Serial.print(rtc->month);
	Serial.print(".");
	Serial.print(rtc->year);
	Serial.print(" ");
	Serial.print(rtc->wday);
	Serial.print(F(" (d.m.y wday)"));
}

