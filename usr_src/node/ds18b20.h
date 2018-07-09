#ifndef DS18B20_H
#define DS18B20_H

#include <Arduino.h>

#define ONEWIRE_SEARCH 0
#define ONEWIRE_CRC 1
#define ONEWIRE_CRC8_TABLE 0
#define ONEWIRE_CRC16 0

#include <OneWire.h>

#define DS18B20_OK 0
#define DS18B20_NOT_PRESENT 1
#define DS18B20_CRC_ERROR 2

byte ds18b20_getTemperature(float &temp, byte pin);

#endif
