#ifndef BRtc_h
#define BRtc_h

#define INCLUDE_SETTERS 0

#include <limits.h>
#include <Arduino.h>
#include "BlockSystem.h"

#define B_RTC_DATE_SET 0x01
#define B_RTC_TIME_SET 0x02
#define B_RTC_ALL_SET  0x03

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

/** Real Time Clock providing Date, Time and Weekday.
 *  It hold date and time value internaly (does not use any external RTC device).\n
 *  In order to count the time, the \c update() method has to be called periodically.\n
 *  There are no setters methods by default, because the SRtc class is provided to set Date and Time of this class.
 *  If setters are required, this class needs to be complied with \c INCLUDE_SETTERS set to \c 1.
 */
class BRtc
{
private:
    BlockSystem* system;

public:
    /** Don't touch! It is used by SRtc.*/
    uint8_t is_set; //public -> SRtc access

public:
    /** Read only! */
    uint8_t year;   //2000+
    /** Read only! */
    uint8_t month;
    /** Read only! */
    uint8_t day;
    /** Read only! */
    uint8_t hour;
    /** Read only! */
    uint8_t minute;
    /** Read only! */
    uint8_t second;
    /** Read only! */
    uint8_t wday;

	/** Constructor.
     *  BRTC needs the BlockSystem to use events.
	 *  @param [in] system Pointer to BlockSystem object.
	 */
    BRtc(BlockSystem* system);

private:
    uint16_t date2days(uint8_t y, uint8_t m, uint8_t d);

public:  //public -> SRtc access
    /** Calculate the day of the week from date.
     *  0-Sunday, 1-Monday, ..., 6-Saturday
     */
    void adjustDayOfTheWeek(void); 

public:
    /** Check whether Date and Time have been set.
     *  @return \c true, if both (date and time) have been set, \c false otherwise
     */
    bool isSet();

#if INCLUDE_SETTERS
    /** Set Date and Time.
     *  If \c wday is not set, it will be calculated automatically.
     */
    void set(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year, uint8_t wday = 255);
    /** Set Date.
     *  If \c wday is not set, it will be calculated automatically.
     */
    void setDate(uint8_t year, uint8_t month, uint16_t day, uint8_t wday = 255);
    /** Set Time. */
    void setTime(uint8_t hour, uint8_t minute, uint8_t second);
#endif

    /** Update RTC.
     *  This method has to be called periodically therefore needs to be included in the infinite loop.
     */
    void update();
};
#endif
