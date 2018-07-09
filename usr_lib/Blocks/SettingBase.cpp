#include "SettingBase.h"


SettingBase::SettingBase(const char* s_name, uint16_t ee_addr)
{
    this->name = s_name;
    //this->default_value = default_value;
    this->ee_addr = ee_addr;
}

/**********************************************************************/

Settings::Settings(uint8_t count)
: count(count)
{
    this->settings = new SettingBase*[count];
}

int8_t Settings::find(char* sname)
{
    for (uint8_t i = 0; i < count; i++) {
        if (strcmp_P(sname, this->settings[i]->name) == 0) return i; //found
    }
    return -1;
}

void Settings::plugin(SettingBase* setting, uint8_t idx)
{
    if (idx < count) this->settings[idx] = setting;
}

void Settings::set(int8_t idx, char* string)
{
    this->settings[idx]->set(string);
}

void Settings::restore(int8_t idx)
{
    if (idx == SETTINGS_ALL) {
        for (uint8_t i = 0; i < count; i++) {
            this->settings[i]->restore();
        }
    } else {
        this->settings[idx]->restore();
    }
}

void Settings::save(int8_t idx)
{
    if (idx == SETTINGS_ALL) {
        for (uint8_t i = 0; i < count; i++) {
            this->settings[i]->save();
        }
    } else {
        this->settings[idx]->save();
    }
}

void Settings::setDefault(int8_t idx)
{
    if (idx == SETTINGS_ALL) {
        for (uint8_t i = 0; i < count; i++) {
            this->settings[i]->setDefault();
        }
    } else {
        this->settings[idx]->setDefault();
    }
}

void Settings::print(int8_t idx)
{
    if (idx == SETTINGS_ALL) {
        for (uint8_t i = 0; i < count; i++) {
            this->settings[i]->print();
            Serial.println();
        }
    } else {
       this->settings[idx]->print();
        Serial.println();
    }
}
