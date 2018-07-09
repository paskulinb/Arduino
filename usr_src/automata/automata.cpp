#define DEBUG 1
#define AUTOMATA_MODE_PIN 12

#include <Arduino.h>
#include <avr/pgmspace.h>
//#include "automata.h"
#include <BlockBase.h>
#include <BlockSystem.h>
#include <BCron.h>
#include <BRtc.h>
#include <BDigiOut.h>
#include <BSensorDS1820.h>
#include <BRegulator.h>
#include <BTimeValue.h>
#include <BPulseValue.h>

#include <SFloat.h>
#include <SCron.h>
#include <SRtcDate.h>
#include <SRtcTime.h>
#include <SPulseValue.h>
#include <STimeValue.h>

#define SETTINGS_EE_START_ADDRESS 0
#define CMD_BUFF_LENGTH 50

char cmd_buff[CMD_BUFF_LENGTH];
uint8_t idx_buff = 0;

BlockSystem* SYS;
BRtc* RTC;

void proc_normal_mode();
void proc_config_mode();

/*** SETTINGS ***/
Settings* settings;
/*** Settings names in PROGMEM ***/
const char sn_Date[] PROGMEM = "Date";
const char sn_Time[] PROGMEM = "Time";

const char sn_Cron1[] PROGMEM = "V1C";
const char sn_Pulse1[] PROGMEM = "V1P";
const char sn_Cron2[] PROGMEM = "V2C";
const char sn_Pulse2[] PROGMEM = "V2P";

const char sn_Temp1[] PROGMEM = "T1";
const char sn_Temp2[] PROGMEM = "T2";

float* param_T1;
float* param_T1REF;
float* param_T2;
float* param_T2REF;

void setup()
{
	Serial.begin(9600);
	Serial.println("Automata start");

    /*** BLOCKS *******************************************************/
    SYS  = new BlockSystem();
    RTC  = new BRtc(SYS);             //sending B_EVT_RTC_MINUTE events

    //Irrigation automat 1
    BCron*       CRON1  = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PV1    = new BPulseValue(SYS, 4);   //called by CRON through BlockSpider
    BDigiOut*    VALVE1 = new BDigiOut(2, true);
    /***                    PV.output -----> VALVE
     *** CRON -->[trigg]--- PV -->[trigg]--- VALVE
    ****/
    VALVE1->input = &PV1->output[0];
    CRON1->job_follow = PV1;
    PV1->job_follow = VALVE1;

    //Irrigation automat 2
    BCron*       CRON2  = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PV2    = new BPulseValue(SYS, 4);   //called by CRON through BlockSpider
    BDigiOut*    VALVE2 = new BDigiOut(3, true);
    /***                    PV.output -----> VALVE
     *** CRON -->[trigg]--- PV -->[trigg]--- VALVE
    ****/
    VALVE2->input = &PV2->output[0];
    CRON2->job_follow = PV2;
    PV2->job_follow = VALVE2;


    //Temperature scheduler/regualtor 1
    BTimeValue*    SDL1     = new BTimeValue(SYS, RTC, 4); //catching B_EVT_RTC_SECOND events
    BSensorDS1820* DS1820_1 = new BSensorDS1820(8, SYS, 5);
    BRegulator*    REG1     = new BRegulator();
    BDigiOut*      HEATER1  = new BDigiOut(4, true);
    /*** DS1820.output -----> REG.input       
    **** SDL.output    -----> REG.setpoint       
    ****                      REG.output -----> HEATER.input
    **** DS1820 -->[trigg]--- REG -->[trigg]--- HEATER
    ****/
    REG1->setpoint = &SDL1->output[0];
    REG1->input = &DS1820_1->output[0];
    HEATER1->input = &REG1->output[0];
    DS1820_1->job_follow = REG1;
    REG1->job_follow = HEATER1;

    //Temperature scheduler/regualtor 2
    BTimeValue*    SDL2     = new BTimeValue(SYS, RTC, 4); //catching B_EVT_RTC_SECOND events
    BSensorDS1820* DS1820_2 = new BSensorDS1820(9, SYS, 5);
    BRegulator*    REG2     = new BRegulator();
    BDigiOut*      HEATER2  = new BDigiOut(5, true);
    /*** DS1820.output -----> REG.input       
    **** SDL.output    -----> REG.setpoint       
    ****                      REG.output -----> HEATER.input
    **** DS1820 -->[trigg]--- REG -->[trigg]--- HEATER
    ****/
    REG2->setpoint = &SDL2->output[0];
    REG2->input = &DS1820_2->output[0];
    HEATER2->input = &REG2->output[0];
    DS1820_2->job_follow = REG2;
    REG2->job_follow = HEATER2;

    /*** Block's SETTINGS *********************************************/
    SRtcDate* sDate     = new SRtcDate(sn_Date, RTC, SETTINGS_EE_START_ADDRESS);
    SRtcTime* sTime     = new SRtcTime(sn_Time, RTC, sDate->nextEEAddr());

    SCron* sCron1        = new SCron(sn_Cron1, CRON1, sTime->nextEEAddr());
    SPulseValue* sPulse1 = new SPulseValue(sn_Pulse1, PV1, sCron1->nextEEAddr());
    SCron* sCron2        = new SCron(sn_Cron2, CRON2, sPulse1->nextEEAddr());
    SPulseValue* sPulse2 = new SPulseValue(sn_Pulse2, PV2, sCron2->nextEEAddr());

    STimeValue* sTVal1   = new STimeValue(sn_Temp1, SDL1, sPulse2->nextEEAddr());
    STimeValue* sTVal2   = new STimeValue(sn_Temp2, SDL2, sTVal1->nextEEAddr());


    settings = new Settings(8);

    settings->plugin(sDate,   0);
    settings->plugin(sTime,   1);
    settings->plugin(sCron1,  2);
    settings->plugin(sPulse1, 3);
    settings->plugin(sCron2,  4);
    settings->plugin(sPulse2, 5);
    settings->plugin(sTVal1,  6);
    settings->plugin(sTVal2,  7);
    
    
    param_T1 = &DS1820_1->output[0];
    param_T1REF = &SDL1->output[0];
    param_T2 = &DS1820_2->output[0];
    param_T2REF = &SDL2->output[0];

    
    //settings->setDefault();
    settings->restore();
    //settings->print();
    /******************************************************************/
    

	//SWITCH to proper automata MODE
	pinMode(AUTOMATA_MODE_PIN, INPUT);
	digitalWrite(AUTOMATA_MODE_PIN, HIGH); //pullup
	delay(1);

	if (digitalRead(AUTOMATA_MODE_PIN) == HIGH) {

		//NORMAL MODE
//		Serial.println("Enter normal mode");
		proc_normal_mode();
	
	} else {

		//CONFIG MODE
//		Serial.println("Enter config mode");
		proc_config_mode();
	}
}

void loop()
{
    SYS->update();
	RTC->update();
}

void print_params()
{
    Serial.print(F("T1="));
    Serial.print(*param_T1);
    Serial.print(F("["));
    Serial.print(*param_T1REF);
    Serial.println(F("]"));

    Serial.print(F("T2="));
    Serial.print(*param_T2);
    Serial.print(F("["));
    Serial.print(*param_T2REF);
    Serial.println(F("]"));
}

void serialEvent()
{
    int8_t sidx;

    while (Serial.available()) {
        cmd_buff[idx_buff] = (char)Serial.read();

        if (cmd_buff[idx_buff] == '\r' || cmd_buff[idx_buff] == '\n') {
            cmd_buff[idx_buff] = '\0';

            /*** execute command **************************************/
            //command starts at first char in buffer
            //parameters start after first 'blank space'
            char* cmd = strtok(cmd_buff, " ");
            char* param = strtok(NULL, "");
            
            //Base commands
            if (strcmp(cmd, "??") == 0) {
                settings->print();
                goto buff_clear;
            }
            
            if (strcmp(cmd, "?") == 0) {
                settings->print(0);
                settings->print(1);
                print_params();
                goto buff_clear;
            }
            
            if (strcmp_P(cmd, PSTR("reboot")) == 0) {
                settings->save(0);                       //save Date
                settings->save(1);                       //save Time
                BlockSystem::reboot();
            }
            
            if (strcmp_P(cmd, PSTR("default")) == 0) {
                settings->setDefault();
                settings->print();
                goto buff_clear;
            }
            
            //Try settings:
            sidx = settings->find(cmd);
            if (sidx >= 0) {
                settings->set(sidx, param);
                settings->print(sidx);
            }
            /**********************************************************/
buff_clear:
            idx_buff = 0; //'reset' cmd_buff
            while (Serial.available()) Serial.read(); //empty Serial RX buffer
            continue;
        }

        if (idx_buff < CMD_BUFF_LENGTH - 1) idx_buff++; //prevent idx_buff overflow

    }
}


void proc_normal_mode()
{
}

void proc_config_mode()
{
}
