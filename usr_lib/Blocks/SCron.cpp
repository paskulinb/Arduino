#include "SCron.h"
#include <EEPROM.h>

SCron::SCron(const char* s_name, BCron* cron, uint16_t ee_addr)
: SettingBase(s_name, ee_addr), cron(cron)
{
}

void SCron::setDefault(void)
{
    cron->set(PSTR("* * * * *"));
}

uint16_t SCron::nextEEAddr(void)
{
    return (this->ee_addr + S_CRON_STRING_MAX_LENGTH);
}

void SCron::set(char* string)
{
    for (uint16_t i = 0; i < S_CRON_STRING_MAX_LENGTH; i++) {
        eeprom_write_byte(this->ee_addr + i, *(string + i));
        if (*(string + i) == '\0') break;
    }
    eeprom_write_byte(this->ee_addr + S_CRON_STRING_MAX_LENGTH - 1, '\0'); //make sure the string ends with '\0' if lenght excedes MAX_LENGTH
}


void SCron::save(void)
{
}

void SCron::restore(void)
{
    char string[S_CRON_STRING_MAX_LENGTH];

    for (uint16_t i = 0; i < S_CRON_STRING_MAX_LENGTH; i++) {
        string[i] = eeprom_read_byte(this->ee_addr + i);
        if (*(string + i) == '\0') break;
    }
    
    cron->set(string);
}


void SCron::print(void)
{
    char buffer[S_CRON_STRING_MAX_LENGTH];
    strcpy_P(buffer, (PGM_P)name);
    Serial.print(buffer);
    Serial.print(" ");

    for (uint16_t i = 0; i < S_CRON_STRING_MAX_LENGTH; i++) {
        buffer[i] = eeprom_read_byte(this->ee_addr + i);
        if (buffer[i] == '\0') break;
    }
	Serial.print(buffer);
}

