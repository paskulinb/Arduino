/*
  BCron.cpp - BCron-like functionality class
  Copyright (c) Borut Paskulin 2017

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  1.0  20 Dec 2017 - initial release
*/

//#include <Arduino.h> 

#include "BCron.h"

BCron::Range::Range() : r1(0), r2(-1), step(0) {}

BCron::BCron(BlockSystem* system, BRtc* rtc, bool send_event)
: system(system), rtc(rtc), send_event(send_event)
{
    system->addEventListener(this, B_EVT_RTC_MINUTE);
    //system->addEventListener(this, B_EVT_RTC_SECOND);
}

void BCron::onEvent(event_t event_flags)
{
//Serial.println("BCron::onEvent ");

    if (event_flags & catching_events) {
//Serial.print("BCron event catched, output=");
        if (checkNow()) {
            output[0] = 1;
            if (send_event) system->setEvent(B_EVT_CRON_JOB);
            if (job_follow != NULL) BlockSpider(job_follow);
        } else {
            output[0] = 0;
        }
//Serial.println(output[0]);
    }
    
    return;
}

int8_t BCron::checkNow(void)
{
    for (uint8_t t_unit = MINUTE; t_unit <= WDAY; t_unit++) {
        
        uint8_t uval;
        switch (t_unit) {
            case MINUTE:
                uval = this->rtc->minute;
                break;
            case HOUR:
                uval = this->rtc->hour;
                break;
            case DAY:
                uval = this->rtc->day;
                break;
            case MONTH:
                uval = this->rtc->month;
                break;
            case WDAY:
                uval = this->rtc->wday;
                break;
        }

        if (Units.RListsLength[t_unit] == 0) {
            //This means cron '*' character (any value) -> goto next CronUnit
            continue;
        }

        bool comply = false;
        
        for (uint8_t range_idx = 0; range_idx < Units.RListsLength[t_unit]; range_idx++) {

            //Does the value fit to unit? That is a big question :o

            Range r = Units.RLists[t_unit][range_idx];

            //upper range NO
            if (r.r2 == -1) {
                if (uval == r.r1) {
                    comply = true;
                } else { //uval > r.r1 and step defined ?
                    //step DEFINED
                    if (r.step > 0) {
                         if (uval > r.r1 && ((uval - r.r1) % r.step) == 0) { //prevent division by zero
                            comply = true;
                        }
                    }
                }
            //upper range YES
            } else {
                //value IN range
                if (uval == r.r1) {
                    comply = true;
                } else if (uval > r.r1 && uval <= r.r2) {
                    //step NOT DEFINED
                    if (r.step == 0) {
                        comply = true;
                    //step DEFINED
                    } else {
                         if (/*value > r.r1 && */((uval - r.r1) % r.step) == 0) { //prevent division by zero
                            comply = true;
                        }
                    }
                }
            }

            if (comply) break; //already fits; ignore following ranges
        }
        
        if (!comply) {
            return 0;//false; //Value do not fit into any Range of this Unit. The given time does not comply crontab defs.
        }
    }

    return 1;//true;
}

/* Initialize new object from Cron style string: "15,25,40,45-59 6-10/2 * * *"
 * Implement 'list','range','step','*'
 */
bool BCron::set(char* cron_str)
{
//Serial.println("BCron::set");            
    const char* _SPACE_ = " ";
    const char* _COMMA_ = ",";
    
    //Split initialization cron string to 'cron units'
    char* unit_str[5];
    unit_str[MINUTE] = strtok(cron_str, _SPACE_);
    unit_str[HOUR]   = strtok(NULL, _SPACE_);
    unit_str[DAY]    = strtok(NULL, _SPACE_);
    unit_str[MONTH]  = strtok(NULL, _SPACE_);
    unit_str[WDAY]   = strtok(NULL, _SPACE_);

    //Split each c_unit to list items (unknown number of list items in advance)
    for (uint8_t t_unit = MINUTE; t_unit <= WDAY; t_unit++) {
        
        //First check if c_unit is a character '*'.
        //In this case we will not create a new RangeList -
        //the corresponding CronUnit will reamin *NULL instead.
        if (*(unit_str[t_unit]) == '*') continue;
        
        uint8_t idx=0; //=0

        //Count ',' (comma character occurences) to find number of list items (comma separated ',')
        uint8_t items_count = 1; //there is at least one value for this cron unit
        do {
            if (*(unit_str[t_unit] + idx) == ',') items_count++;
        } while (*(unit_str[t_unit] + (++idx)) != '\0');
        
        //Create New RangeList: Now we know how many items (Range) will be in the list
        this->Units.RLists[t_unit] = new Range[items_count];
        this->Units.RListsLength[t_unit] = items_count;

        //Fill the RageList with Range-s
        //and Resolve Range parts: "r1-r2/step"
        idx = 0;
        char* range_str = strtok(unit_str[t_unit], _COMMA_);
        do {
            //r1: Resolve Range.r1
            this->Units.RLists[t_unit][idx].r1 = abs(atoi(range_str));
            char* sig;
            //r2: If character '-' exists, resolve Range.r2
            if ((sig = strpbrk(range_str, "-"))) {
                this->Units.RLists[t_unit][idx].r2 = abs(atoi(sig+1));
                //If r2 < r1 swap them
                if (this->Units.RLists[t_unit][idx].r2 < this->Units.RLists[t_unit][idx].r1) {
                    int8_t swap = this->Units.RLists[t_unit][idx].r1;
                    this->Units.RLists[t_unit][idx].r1 = this->Units.RLists[t_unit][idx].r2;
                    this->Units.RLists[t_unit][idx].r2 = swap;
                }
            }
            //step: If character '/' exists, resolve Range.step
            if ((sig = strpbrk(range_str, "/"))) {
                this->Units.RLists[t_unit][idx].step = abs(atoi(sig+1));
            }
/*
Serial.print(this->Units.RLists[t_unit][idx].r1);            
Serial.print("-");            
Serial.print(this->Units.RLists[t_unit][idx].r2);            
Serial.print("/");            
Serial.println(this->Units.RLists[t_unit][idx].step);            
*/
            idx++;
        } while ((range_str = strtok(NULL, _COMMA_))); //next Range
        

    }
    return true;
}
