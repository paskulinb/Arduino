#include "ds18b20.h"


byte ds18b20_getTemperature(float &temp, byte pin)
{
	byte i;
	byte present = 0;
	byte data[12];
	
	OneWire OW(pin);

	present = OW.reset();
	
	if (present == 0) {
		//Serial.print("1-Wire device not present");
		temp = (float) -999;
		return DS18B20_NOT_PRESENT;
	}

	OW.skip();
	OW.write(0x44);         // start conversion, with parasite power on at the end

	delay(800);     // maybe 750ms is enough, maybe not
	// we might do a ds.depower() here, but the reset will take care of it.

	OW.reset();
	OW.skip();
	OW.write(0xBE);         // Read Scratchpad

	
	for ( i = 0; i < 9; i++) {           // we need 9 bytes
		data[i] = OW.read();
	}
	byte crc = OneWire::crc8( data, 8);
	
	if (crc != data[8]) {
		temp = (float) -999;
		return DS18B20_CRC_ERROR;
	}
	
	temp = (float) ( (data[1] << 8) + data[0] )*0.0625;
	//Serial.println(temp);	
	return DS18B20_OK;
}
