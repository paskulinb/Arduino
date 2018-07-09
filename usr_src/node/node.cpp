#define DEBUG 1

#define SERIAL_TX_BUFFER_SIZE 256
#include <Arduino.h>

#include "node.h"

#include "EventManager.h"
#include <EEPROM.h>

#if NODE_TEMP_SENSORS_COUNT
#include "ds18b20.h"
#endif

#if NODE_GSM
Simcom gsm(2,3);

#if NODE_GSM_SMS
char* sms_remote_number;
char sms_message[161];
#endif
#endif


///////////// SETUP ////////////////////////////////////////////////////

void setup() {                

	Serial.begin(38400);

	mode = getMode(); //CONFIG/APP
	
	//// COFIG MODE ////
	if (mode == MODE_CONFIG) {
		
		print_P(PSTR("Config mode"), true);
		inbuff = new char[100];
		inx = 0;
	}
	//// APP MODE ////
	else if (mode == MODE_APP) {
		
		print_P(PSTR("App mode"), true);
		

#if NODE_GSM
#if DEBUG
		print_P(PSTR("GSM init:"));
#endif
		
		int8_t res = gsm.begin("");
		
#if DEBUG
		Serial.println(res);
		if (res > 0) print_P(PSTR("GSM ready"), true);
#endif
#endif
		onRelaysUpdate();
		timer_RelaysUpdate.setInterval(10000, onRelaysUpdate); //10sec
		
		onCheckSMS();
		timer_CheckSMS.setInterval(30000, onCheckSMS); //30sec
		
		onSendToCloud();
		timer_SendToCloud.setInterval(1800000, onSendToCloud); //30min
		//timer_SendToCloud.setInterval(60000, onSendToCloud); //1min
	}
}
///////////// END SETUP ///////////////////////////////////////////////




///////////// LOOP ////////////////////////////////////////////////////

void loop()
{
	//// COFIG MODE ////
	if (mode == MODE_CONFIG) {
		
		loop_config();
	}
	//// APP MODE ////
	else if (mode == MODE_APP) {
		
		timer_RelaysUpdate.update();
		timer_CheckSMS.update();
		timer_SendToCloud.update();
	}
}


void loop_config(void)
{
	while (Serial.available()) {
	
		char c = Serial.read();

		if (c != '\r' && c != '\n') {
			*(inbuff+inx) = c;
			inx++;
			Serial.print(c); //echo
		}
		else {
			*(inbuff+inx) = '\0';
			inx = 0;
			Serial.println(); //echo
			cmd_shell(inbuff);
		}
	}
}

void cmd_shell(char* cmd_line)
{
	//parse & execute config command line
	char* set = strtok(cmd_line, "=");
	char* value = strtok(NULL, "");
	
	if (set == NULL) return;

	if      (strcmp(set, "?") == 0) config_display();
	else if (strcmp_P(set, PSTR("reset")) == 0) config_reset();
	else if (strcmp_P(set, PSTR("setNodeId")) == 0) config_w_str(CFG_NODE_ID, value);
	else if (strcmp_P(set, PSTR("pin")) == 0) config_w_str(CFG_GSM_PIN, value);
	else if (strcmp_P(set, PSTR("sa0")) == 0) config_w_str(CFG_SMS_AUTH_NUM_0, value);
	else if (strcmp_P(set, PSTR("sa1")) == 0) config_w_str(CFG_SMS_AUTH_NUM_1, value);
	else if (strcmp_P(set, PSTR("sa2")) == 0) config_w_str(CFG_SMS_AUTH_NUM_2, value);
	else if (strcmp_P(set, PSTR("sa3")) == 0) config_w_str(CFG_SMS_AUTH_NUM_3, value);
	else if (strcmp_P(set, PSTR("sa4")) == 0) config_w_str(CFG_SMS_AUTH_NUM_4, value);
	else if (strcmp_P(set, PSTR("apn")) == 0) config_w_str(CFG_GPRS_APN, value);
	else if (strcmp_P(set, PSTR("user")) == 0) config_w_str(CFG_GPRS_USERNAME, value);
	else if (strcmp_P(set, PSTR("pass")) == 0) config_w_str(CFG_GPRS_PASSWORD, value);
	else if (strcmp_P(set, PSTR("host")) == 0) config_w_str(CFG_HOST_NAME_IP, value);
	else if (strcmp_P(set, PSTR("path")) == 0) config_w_str(CFG_HOST_PATH, value);
	else if (strcmp_P(set, PSTR("port")) == 0) config_w_str(CFG_HOST_PORT, value);
	else Serial.println("!");
}
///////////////// END LOOP ////////////////////////////////////////////


void onRelaysUpdate(void)
{
#if NODE_TEMP_SENSORS_COUNT
	scan_temp_sensors();
#endif

#if NODE_RELAYS_COUNT
	relays_update();
#endif
}

void onCheckSMS(void)
{
#if NODE_GSM_SMS
	char* sms_text = sms_check();
	sms_cmd(sms_text);
#endif
}

void onSendToCloud(void)
{
	int res;

#if NODE_GSM_GPRS
	res = gsm.gprs_start(CFG_GPRS_APN, CFG_GPRS_USERNAME, CFG_GPRS_PASSWORD);
#if DEBUG
	print_P(PSTR("GPRS start:"));
	Serial.println(res);
#endif
#if DEBUG
	print_P(PSTR("HTTP-GET request:"));
#endif
	res = gsm.httpGET_request(CFG_HOST_NAME_IP, CFG_HOST_PATH, CFG_HOST_PORT, print_query_string);
#if DEBUG
	Serial.println(res);
#endif
	res = gsm.gprs_stop();
#if DEBUG
	print_P(PSTR("GPRS stop:"));
	Serial.println(res);
#endif
#endif
}

bool config_w_str(uint16_t pos_ee, char* str)
{
	while (true) {
		EEPROM[pos_ee] = *str;
		if (*str == '\0') break;
		str++;
		pos_ee++;
	}

	Serial.println("+");
	return true;
}

bool config_w_uint16(uint16_t pos_ee, uint16_t val)
{
}

void config_display(void)
{
	print_P(PSTR("nid:")); config_println(CFG_NODE_ID);
	print_P(PSTR("pin:")); config_println(CFG_GSM_PIN);
	print_P(PSTR("sa0:")); config_println(CFG_SMS_AUTH_NUM_0);
	print_P(PSTR("sa1:")); config_println(CFG_SMS_AUTH_NUM_1);
	print_P(PSTR("sa2:")); config_println(CFG_SMS_AUTH_NUM_2);
	print_P(PSTR("sa3:")); config_println(CFG_SMS_AUTH_NUM_3);
	print_P(PSTR("sa4:")); config_println(CFG_SMS_AUTH_NUM_4);
	print_P(PSTR("apn:")); config_println(CFG_GPRS_APN);
	print_P(PSTR("user:")); config_println(CFG_GPRS_USERNAME);
	print_P(PSTR("pass:")); config_println(CFG_GPRS_PASSWORD);
	print_P(PSTR("host:")); config_println(CFG_HOST_NAME_IP);
	print_P(PSTR("path:")); config_println(CFG_HOST_PATH);
	print_P(PSTR("port:")); config_println(CFG_HOST_PORT);
}

void config_println(uint16_t pos_ee) //print single setting line to Serial
{
	char c;
	while ((c = EEPROM[pos_ee++]) != '\0') Serial.write(c);
	Serial.println();
}

void config_reset(void)
{
	for (uint16_t i = CFG_START; i <= CFG_END; i++) EEPROM[i] = '\0';

	config_w_str(CFG_HOST_NAME_IP, "shub.com");
	config_w_str(CFG_HOST_PORT, "80");
}


uint8_t getMode(void)
{
	pinMode(NODE_CONFIG_PIN, INPUT_PULLUP);
	if (digitalRead(NODE_CONFIG_PIN) == HIGH)
		return MODE_APP;
	else
		return MODE_CONFIG;
}



#if NODE_TEMP_SENSORS_COUNT

void scan_temp_sensors()
{
	byte i;
	
	for (i=0; i<NODE_TEMP_SENSORS_COUNT; i++) {
		
		byte ret = ds18b20_getTemperature(param.temp_sensor[i].value, param.temp_sensor[i].pin);
	}


#if DEBUG
	for (i=0; i<NODE_TEMP_SENSORS_COUNT; i++) {

		Serial.print("T"); Serial.print(i); Serial.print("=");
		Serial.print(param.temp_sensor[i].value, 1);
		if (EEPROM[NODE_MAX_FN_ARGUMENTS * i] == NODE_FN_HEAT) {
			Serial.print("/");
			Serial.print(EEPROM[NODE_MAX_FN_ARGUMENTS * i + 1]);
			print_P(PSTR(" Heating"));
		}
		if (EEPROM[NODE_MAX_FN_ARGUMENTS * i] == NODE_FN_COOL) {
			Serial.print("/");
			Serial.print(EEPROM[NODE_MAX_FN_ARGUMENTS * i + 1]);
			print_P(PSTR(" Cooling"));
		}
		Serial.println();	
	}
#endif
}

#endif


#if NODE_GSM_SMS

char* sms_check()
{
	char c;
	
	if (gsm.smsAvailable()) //Any income SMS available?
	{
		gsm.smsRemoteNumber(&sms_remote_number);
		
		bool authorized = strcmp_EE(sms_remote_number, CFG_SMS_AUTH_NUM_0) ||
						   strcmp_EE(sms_remote_number, CFG_SMS_AUTH_NUM_1) ||
						   strcmp_EE(sms_remote_number, CFG_SMS_AUTH_NUM_2) ||
						   strcmp_EE(sms_remote_number, CFG_SMS_AUTH_NUM_3) ||
						   strcmp_EE(sms_remote_number, CFG_SMS_AUTH_NUM_4);

#if DEBUG
		print_P(PSTR("SMS from: "));
		Serial.println(sms_remote_number);
		if (authorized) Serial.println("authorized");
		else Serial.println("not authorized");
#endif
		
		if (!authorized) {
			gsm.smsDelete();
			return NULL;
		}

		// Read message byte by byte
		byte i = 0;
		while (c = gsm.smsRead()) {
			sms_message[i] = c;
			i++;
			if (i == 160) break; //max SMS length
		}
		sms_message[i] = '\0';
		gsm.smsDelete(); // delete message from modem memory

#if DEBUG
		Serial.println(sms_message);
#endif
		
		return sms_message;
	}
	
#if DEBUG
	print_P(PSTR("No SMS available"), true);
#endif
	
	return NULL;
}


void sms_send_status()
{
	gsm.smsBegin(sms_remote_number);
	delay(100);

#if NODE_TEMP_SENSORS_COUNT

	for (byte i=0; i<NODE_TEMP_SENSORS_COUNT; i++) {
		gsm.print("T"); gsm.print(i,1); gsm.print("=");
		gsm.print(param.temp_sensor[i].value);
		gsm.print("\n");
	}
#endif

#if NODE_RELAYS_COUNT

	for (byte i=0; i<NODE_RELAYS_COUNT; i++) {
		gsm.print("R"); gsm.print(i); gsm.print("=");
		gsm.print(relay_get_state(i));
		gsm.print("\n");
	}
#endif

	delay(100);
	gsm.smsEnd();
}

#endif




void sms_cmd(char* cmdstr)
{
	if (cmdstr == NULL) return;

	/* ******************************* */
	/* SPLIT command line to arguments */
	char* carg[NODE_MAX_CMD_ARGUMENTS]; //pointers to arguments
	carg[0] = strtok(cmdstr, " ");
	carg[0] = strlwr(carg[0]); //make CASE INSENSITIVE
	for (byte i=1; i<NODE_MAX_CMD_ARGUMENTS; i++) {
		carg[i] = strtok(NULL, " ");
		carg[i] = strlwr(carg[i]); //make CASE INSENSITIVE
	}
	/* ******************************* */

	
	/* CMD "?" *****************/
	if (strcmp(carg[0], "?")==0) {
		
		sms_send_status();
		return;
	}
	

#if NODE_RELAYS_COUNT

	byte i = -1; //relay index
	
	/* CMD "ON" ****************************/
	if (strncasecmp_P(carg[0], PSTR("on"), 2) == 0) {  
		
		i = str_to_idx(carg[0]+2);
		
		if (i >= 0 && i < NODE_RELAYS_COUNT) {
			EEPROM[NODE_MAX_FN_ARGUMENTS * i] = NODE_FN_ON;
			relay_update(i);
		}
	}
	else

	/* CMD "OFF" ****************************/
	if (strncasecmp_P(carg[0], PSTR("off"), 3) == 0) {

		i = str_to_idx(carg[0]+3);

		if (i >= 0 && i < NODE_RELAYS_COUNT) {
			EEPROM[NODE_MAX_FN_ARGUMENTS * i] = NODE_FN_OFF;
			relay_update(i);
		}
	}
	else
	
	/* CMD "PULSE" ****************************/
	if (strncasecmp_P(carg[0], PSTR("pulse"), 5) == 0) {

		i = str_to_idx(carg[0]+5);

		if (i >= 0 && i < NODE_RELAYS_COUNT) {
			EEPROM[NODE_MAX_FN_ARGUMENTS * i] = NODE_FN_PULSE;
			param.relays[i].pulse_millis_downcounter = decode_milisec(carg[1]);
			param.relays[i].pulse_millis_last = millis();
			relay_update(i);
		}
	}
	else

	/* CMD "HEAT" */
	if (strncasecmp_P(carg[0], PSTR("heat"), 4) == 0) {

		i = str_to_idx(carg[0]+4);

		if (i >= 0 && i < NODE_RELAYS_COUNT) {
			EEPROM[NODE_MAX_FN_ARGUMENTS * i] = NODE_FN_HEAT;
			EEPROM[NODE_MAX_FN_ARGUMENTS * i + 1] = (byte)atoi(carg[1]);
			relay_update(i);
		}
	}
	else

	/* CMD "COOL" */
	if (strncasecmp_P(carg[0], PSTR("cool"), 4) == 0) {

		i = str_to_idx(carg[0]+4);

		if (i >= 0 && i < NODE_RELAYS_COUNT) {
			EEPROM[NODE_MAX_FN_ARGUMENTS * i] = NODE_FN_COOL;
			EEPROM[NODE_MAX_FN_ARGUMENTS * i + 1] = (byte)atoi(carg[1]);
			relay_update(i);
		}
	}

#endif
}

byte str_to_idx(char* pi)
{
	//Determine port index from command string. *pi must point to digit (numeric ["0".."9"]) character.
	//accept only: "cmd" or "cmd1" or "CMD2" or "Cmd3" ...  ("cmd0" is not allowed)
	//"cmd"->0, "cmd1"->0 .. "cmd9"->8
 	//only 0..9 return value is available  ("cmd25" will return index 2, 5 will be ignored)
 	//Return: index
 	//        0..9  if valid
 	//        -1    if not valid or not existent

	byte i = -1;
	if (*pi >= '1' && *pi <= '9') i = *pi - '1';
	else if (*pi == '\0') i = 0;
	return i;
}

unsigned long decode_milisec(char* time_cmd)
{
	//return miliseconds from string "2D4h45m" or "6H" or "180m" or "1d520M"

	byte dd = 0, hh = 0, mm = 0;
	char* t = time_cmd; //find the duration and convert to miliseconds

	char* unit = strcasestr(t, "d");
	if (unit != NULL) {
		dd = atoi(t);
		t = unit+1;    //skip the 'd' character to next digit
	}
	unit = strcasestr(t, "h");
	if (unit != NULL) {
		hh = atoi(t);
		t = unit+1;
	}
	unit = strcasestr(t, "m");
	if (unit != NULL) {
		mm = atoi(t);
	}
	return ((dd*24 + hh)*60 + mm)*60000;
}

#if NODE_RELAYS_COUNT

void relays_update()
{
	for (byte i=0; i<NODE_RELAYS_COUNT; i++) relay_update(i);

}

void relay_update(byte idx)
{
	pinMode(param.relays[idx].pin, OUTPUT);
	
	char fn = EEPROM[NODE_MAX_FN_ARGUMENTS * idx];

	switch (fn) {
		case NODE_FN_OFF:
			relay_fn_off(idx);
			break;
		case NODE_FN_ON:
			relay_fn_on(idx);
			break;
		case NODE_FN_HEAT:
			relay_fn_heat(idx, (float) EEPROM[NODE_MAX_FN_ARGUMENTS * idx + 1], param.temp_sensor[idx].value);
			break;
		case NODE_FN_COOL:
			relay_fn_cool(idx, (float) EEPROM[NODE_MAX_FN_ARGUMENTS * idx + 1], param.temp_sensor[idx].value);
			break;
		case NODE_FN_PULSE:
			relay_fn_pulse(idx);
			break;
		default:
			relay_fn_off(idx);
			break;
	}


#if DEBUG
	Serial.print("R"); Serial.print(idx); Serial.print("=");
	Serial.println(relay_get_state(idx));
#endif
}

void relay_fn_off(byte idx)
{
	digitalWrite(param.relays[idx].pin, LOW);
}

void relay_fn_on(byte idx)
{
	digitalWrite(param.relays[idx].pin, HIGH);
}

void relay_fn_heat(byte idx, float ref_temp, float current_temp)
{
	if (current_temp < ref_temp-NODE_TEMP_HYSTERESIS) digitalWrite(param.relays[idx].pin, HIGH);
	if (current_temp > ref_temp+NODE_TEMP_HYSTERESIS) digitalWrite(param.relays[idx].pin, LOW);
}

void relay_fn_cool(byte idx, float ref_temp, float current_temp)
{
	if (current_temp < ref_temp-NODE_TEMP_HYSTERESIS) digitalWrite(param.relays[idx].pin, LOW);
	if (current_temp > ref_temp+NODE_TEMP_HYSTERESIS) digitalWrite(param.relays[idx].pin, HIGH);
}

void relay_fn_pulse(byte idx)
{
	unsigned long now = millis();
	
	long tdif = now - param.relays[idx].pulse_millis_last;
	
	param.relays[idx].pulse_millis_last = now;
	
	//time overflow handling (arduino millis() overflows on every ~50 days)
	if (tdif > 0) {  //no overflow happened, proceed downcounting
		if (param.relays[idx].pulse_millis_downcounter > tdif) { //don't miss 0 !!!
			param.relays[idx].pulse_millis_downcounter -= tdif;
			digitalWrite(param.relays[idx].pin, HIGH);
		}
		else {
			EEPROM[NODE_MAX_FN_ARGUMENTS * idx] = NODE_FN_OFF; //Finished -> turn relay to OFF function
			digitalWrite(param.relays[idx].pin, LOW);
		}
	}
}

//void relay_fn_reg2lvl(byte idx, byte current_value){}

byte relay_get_state(byte idx)
{
	return digitalRead(param.relays[idx].pin);
}

#endif

void print_query_string(Simcom* gsm)
{
	gsm->print("nid=");
	gsm->print_EE(CFG_NODE_ID);
	gsm->print_AMP();
	gsm->print("T1=");
	gsm->print(param.temp_sensor[0].value);
	gsm->print_AMP();
	gsm->print("T2=");
	gsm->print(param.temp_sensor[1].value);
}

void print_P(PGM_P str, bool CR)
{
	int i=0;
	char c;
	while (0 != (c=pgm_read_byte_near(str + i++))) Serial.write(c);
	if(CR) print_P(PSTR("\r\n"), false);
	return;
}

void print_EE(uint16_t ptr)
{
	char c;
	while (0 != (c=EEPROM[ptr++])) Serial.write(c);
}

bool strcmp_EE(char* str, uint16_t ee_ptr)
{
	//uint8_t i = 0;
	while(true) {
	
		if (*str != EEPROM[ee_ptr]) return false;
		if (*str == '\0' /*|| EEPROM[ee_ptr] == '\0'*/) return true; //iz predhodne vrstice (pogoja) sledi, da sta tukaj oba '\0'
		str++;
		ee_ptr++;
	}
}
