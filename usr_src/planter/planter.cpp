#define DEBUG 1
#define AUTOMATA_MODE_PIN 12

#include <Arduino.h>
#include <avr/pgmspace.h>

#include <BlockBase.h>
#include <BlockSystem.h>
#include <BCron.h>
#include <BRtc.h>
#include <BAnalogIn.h>
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

const char sn_Cron1[] PROGMEM = "CR1";
const char sn_Pulse1[] PROGMEM = "PU1";
const char sn_Cron2[] PROGMEM = "CR2";
const char sn_Pulse2[] PROGMEM = "PU2";
const char sn_Cron3[] PROGMEM = "CR3";
const char sn_Pulse3[] PROGMEM = "PU3";
const char sn_Cron4[] PROGMEM = "CR4";
const char sn_Pulse4[] PROGMEM = "PU4";

const char sn_Temp1[] PROGMEM = "T1";
const char sn_Temp2[] PROGMEM = "T2";

float* param_T1;
float* param_T1REF;
float* param_T2;
float* param_T2REF;

BDigiOut* RLY1;
BDigiOut* RLY2;
BDigiOut* RLY3;
BDigiOut* RLY4;



void setup()
{
	Serial.begin(9600);
	Serial.println(F("Planter init"));

    /*** BLOCKS *******************************************************/
    SYS  = new BlockSystem();
    RTC  = new BRtc(SYS);             //sending B_EVT_RTC_MINUTE events


    //Irrigation automat 1
    BCron*       CRN1 = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PLS1 = new BPulseValue(SYS, 1);   //called by CRON through BlockSpider
    BAnalogIn*   AIN1 = new BAnalogIn(A0);
    RLY1 = new BDigiOut(2, true);
    /***                   PLS.output -----> RLY
     *** CRN -->[trigg]--- PLS -->[trigg]--- RLY
    ****/
    RLY1->input = &PLS1->output[0];
    CRN1->job_follow = PLS1;
    PLS1->job_follow = RLY1;

    //Irrigation automat 2
    BCron*       CRN2 = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PLS2 = new BPulseValue(SYS, 1);   //called by CRON through BlockSpider
    RLY2 = new BDigiOut(3, true);

    RLY2->input = &PLS2->output[0];
    CRN2->job_follow = PLS2;
    PLS2->job_follow = RLY2;

    //Irrigation automat 3
    BCron*       CRN3 = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PLS3 = new BPulseValue(SYS, 1);   //called by CRON through BlockSpider
    RLY3 = new BDigiOut(4, true);

    RLY3->input = &PLS3->output[0];
    CRN3->job_follow = PLS3;
    PLS3->job_follow = RLY3;

    //Irrigation automat 4
    BCron*       CRN4 = new BCron(SYS, RTC);       //catching B_EVT_RTC_MINUTE events, sending B_EVT_CRON_JOB events (if enabled), calling BlockSpider
    BPulseValue* PLS4 = new BPulseValue(SYS, 1);   //called by CRON through BlockSpider
    RLY4 = new BDigiOut(5, true);

    RLY4->input = &PLS4->output[0];
    CRN4->job_follow = PLS4;
    PLS4->job_follow = RLY4;

    //Temperature scheduler/regualtor 1
    BTimeValue*    SCHDLR1  = new BTimeValue(SYS, RTC, 4); //catching B_EVT_RTC_SECOND events
    BSensorDS1820* DS1820_1 = new BSensorDS1820(8, SYS, 5);
    BRegulator*    REG1     = new BRegulator();
    BDigiOut*      HEATER1  = new BDigiOut(6, true);
    /*** DS1820.output  -----> REG.input       
    **** SCHDLR1.output -----> REG.setpoint       
    ****                       REG.output -----> HEATER.input
    **** DS1820 -->[trigg]---  REG -->[trigg]--- HEATER
    ****/
    REG1->setpoint = &SCHDLR1->output[0];
    REG1->input = &DS1820_1->output[0];
    HEATER1->input = &REG1->output[0];
    DS1820_1->job_follow = REG1;
    REG1->job_follow = HEATER1;

    //Temperature scheduler/regualtor 2
    BTimeValue*    SCHDLR2  = new BTimeValue(SYS, RTC, 4); //catching B_EVT_RTC_SECOND events
    BSensorDS1820* DS1820_2 = new BSensorDS1820(9, SYS, 5);
    BRegulator*    REG2     = new BRegulator();
    BDigiOut*      HEATER2  = new BDigiOut(7, true);
    /*** DS1820.output -----> REG.input       
    **** SDL.output    -----> REG.setpoint       
    ****                      REG.output -----> HEATER.input
    **** DS1820 -->[trigg]--- REG -->[trigg]--- HEATER
    ****/
    REG2->setpoint = &SCHDLR2->output[0];
    REG2->input = &DS1820_2->output[0];
    HEATER2->input = &REG2->output[0];
    DS1820_2->job_follow = REG2;
    REG2->job_follow = HEATER2;

    /*** Block's SETTINGS *********************************************/
    SRtcDate* sDate     = new SRtcDate(sn_Date, RTC, SETTINGS_EE_START_ADDRESS);
    SRtcTime* sTime     = new SRtcTime(sn_Time, RTC, sDate->nextEEAddr());

    SCron* sCron1        = new SCron(sn_Cron1, CRN1, sTime->nextEEAddr());
    SPulseValue* sPulse1 = new SPulseValue(sn_Pulse1, PLS1, sCron1->nextEEAddr());
    SCron* sCron2        = new SCron(sn_Cron2, CRN2, sPulse1->nextEEAddr());
    SPulseValue* sPulse2 = new SPulseValue(sn_Pulse2, PLS2, sCron2->nextEEAddr());
    SCron* sCron3        = new SCron(sn_Cron3, CRN3, sPulse2->nextEEAddr());
    SPulseValue* sPulse3 = new SPulseValue(sn_Pulse3, PLS3, sCron3->nextEEAddr());
    SCron* sCron4        = new SCron(sn_Cron4, CRN4, sPulse3->nextEEAddr());
    SPulseValue* sPulse4 = new SPulseValue(sn_Pulse4, PLS4, sCron4->nextEEAddr());

    STimeValue* sTVal1   = new STimeValue(sn_Temp1, SCHDLR1, sPulse4->nextEEAddr());
    STimeValue* sTVal2   = new STimeValue(sn_Temp2, SCHDLR2, sTVal1->nextEEAddr());


    settings = new Settings(12);

    settings->plugin(sDate,   0);
    settings->plugin(sTime,   1);
    settings->plugin(sCron1,  2);
    settings->plugin(sPulse1, 3);
    settings->plugin(sCron2,  4);
    settings->plugin(sPulse2, 5);
    settings->plugin(sCron3,  6);
    settings->plugin(sPulse3, 7);
    settings->plugin(sCron4,  8);
    settings->plugin(sPulse4, 9);
    settings->plugin(sTVal1,  10);
    settings->plugin(sTVal2,  11);
    
    
    param_T1 = &DS1820_1->output[0];
    param_T1REF = &SCHDLR1->output[0];
    param_T2 = &DS1820_2->output[0];
    param_T2REF = &SCHDLR2->output[0];

    
    //settings->setDefault();
    settings->restore();
    //settings->print();
    /******************************************************************/
	Serial.println(F("READY"));
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

    while (Serial.available()){
        cmd_buff[idx_buff] = (char)Serial.read();

        if (cmd_buff[idx_buff] == '\r' || cmd_buff[idx_buff] == '\n'){
            cmd_buff[idx_buff] = '\0';

            /*** execute command **************************************/
            //command starts at first char in buffer
            //parameters start after first 'blank space'
            char* cmd = strtok(cmd_buff, " ");
            char* param = strtok(NULL, "");
            
            //Base commands
            if (strcmp(cmd, "??") == 0){
                settings->print();
                goto buff_clear;
            }
            
            if (strcmp(cmd, "?") == 0){
                settings->print(0);
                settings->print(1);
                print_params();
                goto buff_clear;
            }
            
            if (strcmp_P(cmd, PSTR("reboot")) == 0){
                settings->save(0);                       //save Date
                settings->save(1);                       //save Time
                BlockSystem::reboot();
            }
            
            if (strcmp_P(cmd, PSTR("default")) == 0){
                settings->setDefault();
                settings->print();
                goto buff_clear;
            }
            
            //Try settings:
            sidx = settings->find(cmd);
            if (sidx >= 0){
                settings->set(sidx, param);
                settings->print(sidx);
            }
            
            //*** Application specific commands ***
            // Relay1..Relay4 manual switch
            if (strcmp_P(cmd, PSTR("x1")) == 0){
                RLY1->setOut((strcmp_P(param, PSTR("1")) == 0) ? true : false);
            } else if (strcmp_P(cmd, PSTR("x2")) == 0){
                RLY2->setOut((strcmp_P(param, PSTR("1")) == 0) ? true : false);
            } else if (strcmp_P(cmd, PSTR("x3")) == 0){
                RLY3->setOut((strcmp_P(param, PSTR("1")) == 0) ? true : false);
            } else if (strcmp_P(cmd, PSTR("x4")) == 0){
                RLY4->setOut((strcmp_P(param, PSTR("1")) == 0) ? true : false);
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
