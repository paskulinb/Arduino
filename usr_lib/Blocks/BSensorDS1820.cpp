#include "BSensorDS1820.h"

BSensorDS1820::BSensorDS1820(uint8_t arduino_pin, BlockSystem* system, uint16_t repeat_seconds) : system(system)
{
    output[0] = B_PARAM_VALUE_BAD; //temperature
    status = 0x00;

    DS1820 = new OneWire(arduino_pin);
    
    my_timer = (system == NULL) ? NULL : system->newTimer(this);
    
    conv_restart_sec = repeat_seconds;

    if (conv_restart_sec > 0) //if automatic sampling required, start conversion
        startConversion();
}

int8_t BSensorDS1820::startConversion(void)
{
	if (DS1820->reset() == 0) {           //Is device present?
                                          //Device NOT present:
        if (/*conv_restart_sec > 0
            &&*/ my_timer != NULL) {        //restart again if required
            my_timer->start(2000);        //retry after 2 seconds
        }

        status &= ~DS1820_STAT_DATA_OK; //clear bit
		return 0;
	}

	DS1820->skip();
	DS1820->write(0x44);

    conv_start_millis = millis();

    if (my_timer != NULL) {
        my_timer->start(DS1820_CONVERSION_DURATION_MILLIS); //singleshot, conversion timeout
    }

    status |= DS1820_STAT_PROGRESS;
    return 1;
}

int8_t BSensorDS1820::dataReady(void)
{
    if ((millis() - conv_start_millis) < DS1820_CONVERSION_DURATION_MILLIS) {
        return 0;//DS1820_STAT_PROGRESS;  //=0x02 conversion not finished yet
    }

	if (DS1820->reset() == 0) {
        goto error_reading;                  //NOT_PRESENT ERROR
    }
    DS1820->skip();
    DS1820->write(0xBE);

    uint8_t data[9];
    
    for (uint8_t i = 0; i < 9; i++) {
        data[i] = DS1820->read();
    }
    
    if (OneWire::crc8(data, 8) != data[8]) {
        goto error_reading;                   //CRC ERROR
    }
    
    output[0] = ((data[1] << 8) + data[0]) * 0.0625;
    status |= DS1820_STAT_DATA_OK;
    status &= ~DS1820_STAT_PROGRESS;

    return 1;

error_reading:

    status &= ~DS1820_STAT_DATA_OK;

    return 0;
}


bool BSensorDS1820::trigger(uint8_t type)
{
    if (!(status & DS1820_STAT_PROGRESS)) startConversion();

    return false; //job not finished - just started or already in progress
}


void BSensorDS1820::onTimer(void)
{
    if (status & DS1820_STAT_PROGRESS) { //conversion timeout occured
        
        dataReady();
        
        if (conv_restart_sec > 0) { //auto restart conversion required
            my_timer->start(conv_restart_sec * 1000 - DS1820_CONVERSION_DURATION_MILLIS); //time to next conversion
        }
        
        if ((status & DS1820_STAT_DATA_OK)) {
            system->setEvent(B_EVT_TEMPERATURE);
            if (job_follow != NULL) BlockSpider(job_follow);
        }
        
    } else { //IDLE -> start next conversion
      
        startConversion();
    }

    return;
}
