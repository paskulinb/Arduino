#include "BRtc.h"

BRtc::BRtc(BlockSystem* system)
: is_set(0)
{
    this->system = system;
}

uint16_t BRtc::date2days(uint8_t y, uint8_t m, uint8_t d)
{
    //if (y >= 2000)
    //    y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

void BRtc::adjustDayOfTheWeek(void)
{    
    wday = (date2days(year, month, day) + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
    return;
}

bool BRtc::isSet()
{
    return (this->is_set == B_RTC_ALL_SET) ? true : false;
}


void BRtc::update()
{
    if (!(this->is_set == B_RTC_ALL_SET)) return;

	static unsigned long millis_last;
	static unsigned long millis_remaind;
	unsigned long now = millis();
	long millis_dif = now - millis_last;
	if (millis_dif < 0) millis_dif = ULONG_MAX + millis_dif; //time overflow handling (arduino millis() overflows on every ~49 days)
	millis_last = now;
	millis_remaind += millis_dif;

	while (millis_remaind > 1000) {
		millis_remaind -= 1000;

//print_timestamp();
        system->setEvent(B_EVT_RTC_SECOND);
		if (++second > 59) {
			second = 0;
            system->setEvent(B_EVT_RTC_MINUTE);
			if (++minute > 59) {
				minute = 0;
                system->setEvent(B_EVT_RTC_HOUR);
				if (++hour > 23) {
					hour = 0;
                    system->setEvent(B_EVT_RTC_DAY);
					if (++wday > 6) wday = 0;
                    uint8_t days_in_month = pgm_read_byte(daysInMonth + month - 1);
                    if (month == 2 && year % 4 == 0) days_in_month++;
					if (++day > days_in_month) {
						day = 1;
						if (++month > 12) {
							month = 1;
							year++;
						}
					}
				}
			}
		}
	}
}

#if INCLUDE_SETTERS

void BRtc::set(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year, uint8_t wday)
{
    setTime(hour, minute, second);
    setDate(year, month, day, wday);
}

void BRtc::setDate(uint8_t year, uint8_t month, uint16_t day, uint8_t wday)
{
	this->day = day;
	this->month = month;
    if (year >= 2000)
        year -= 2000;
	this->year = year;
    this->wday = (wday < 7) ? wday : dayOfTheWeek(year, month, day);
    
    this->is_set |= B_RTC_DATE_SET;
}

void BRtc::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	this->second = second;
	this->minute = minute;
	this->hour = hour;

    this->is_set |= B_RTC_TIME_SET;
}
#endif
/*
void BRtc::print_timestamp()
{
	Serial.print(wday);
	Serial.print(", ");
	Serial.print(day);
	Serial.print("-");
	Serial.print(month);
	Serial.print("-");
	Serial.print(year);
	Serial.print(" ");
	Serial.print(hour);
	Serial.print("-");
	Serial.print(minute);
	Serial.print("-");
	Serial.println(second);
}
*/
