#include "Timer.h"
Timer timer_RelaysUpdate;
Timer timer_CheckSMS;
Timer timer_SendToCloud;

void onRelaysUpdate(void);
void onCheckSMS(void);
void onSendToCloud(void);


#define NODE_TEMP_SENSORS_COUNT 2  // max 4
#define NODE_RELAYS_COUNT 2        // max 4

#define NODE_GSM 1
#define NODE_GSM_SMS 1
#define NODE_GSM_GPRS 1

#define NODE_MAX_CMD_ARGUMENTS 3    //HEAT1 22                                 (command line)
#define NODE_MAX_FN_ARGUMENTS 2     //including fn_code: fn_code arg1 arg2 ... (EEPROM)
#define NODE_TEMP_HYSTERESIS 0.5

#define NODE_CONFIG_PIN 8
#define MODE_CONFIG 1
#define MODE_APP 2

uint8_t mode;
uint8_t getMode(void);

#define CFG_START           20	//address in EEPROM, strings null-terminated
#define CFG_NODE_ID         20	//char[20]
#define CFG_GSM_PIN         40	//char[5]
#define CFG_SMS_AUTH_NUM_0  45	//char[15]
#define CFG_SMS_AUTH_NUM_1  60	//char[15]
#define CFG_SMS_AUTH_NUM_2  75	//char[15]
#define CFG_SMS_AUTH_NUM_3  90	//char[15]
#define CFG_SMS_AUTH_NUM_4 105	//char[15]
#define CFG_GPRS_APN       120	//char[15]
#define CFG_GPRS_USERNAME  135	//char[15]
#define CFG_GPRS_PASSWORD  150	//char[15]
#define CFG_HOST_NAME_IP   165	//char[20]
#define CFG_HOST_PATH      185	//char[20]
#define CFG_HOST_PORT      205	//char[6]
#define CFG_END            210

bool config_w_str(uint16_t pos_ee, char* str);
bool config_w_uint16(uint16_t pos_ee, uint16_t val);
void config_display(void);
void config_println(uint16_t pos_ee); //print single setting line to serial
void config_reset(void);

void loop_config(void);
char* inbuff; //command mode serial rx buffer
uint8_t inx;  //command mode serial rx buffer index

void cmd_shell(char* cmd_line);



#if NODE_TEMP_SENSORS_COUNT
struct temperature_sensor
{
	byte pin;
	float value;
};

void scan_temp_sensors();
#endif





#if NODE_RELAYS_COUNT

#define NODE_FN_OFF 0
#define NODE_FN_ON 1
#define NODE_FN_REG2LVL 2
#define NODE_FN_HEAT 3
#define NODE_FN_COOL 4
#define NODE_FN_PULSE 5

struct relay
{
	byte pin;
	unsigned long pulse_millis_downcounter;
	unsigned long pulse_millis_last;
};

void relay_update(byte idx);
void relays_update();
void relay_fn_off(byte idx);
void relay_fn_on(byte idx);
void relay_fn_heat(byte idx, float ref_temp, float current_temp);
void relay_fn_cool(byte idx, float ref_temp, float current_temp);
void relay_fn_pulse(byte idx);
//void relay_fn_reg2lvl(byte idx, byte current_value);
byte relay_get_state(byte idx);

#endif


#if NODE_GSM
#include "Simcom.h"

#if NODE_GSM_SMS
char* sms_check();
void sms_send_status();
#endif;
#endif


struct node_parameters
{

#if NODE_TEMP_SENSORS_COUNT
	temperature_sensor temp_sensor[NODE_TEMP_SENSORS_COUNT];
#endif

#if NODE_RELAYS_COUNT
	relay relays[NODE_RELAYS_COUNT];
#endif
		
} param { /* ******************* inicializacija 'node_param' strukture * *******************/
	
#if NODE_TEMP_SENSORS_COUNT
	{                                  //temp_sensor[]
#if NODE_TEMP_SENSORS_COUNT > 0
										{ 9, -999}	//temp_sensor 1 {pin,value}
#endif
#if NODE_TEMP_SENSORS_COUNT > 1
										,{10, -999}	//temp_sensor 2
#endif
#if NODE_TEMP_SENSORS_COUNT > 2
										,{11, -999}	//temp_sensor 3
#endif
#if NODE_TEMP_SENSORS_COUNT > 3
										,{12, -999}	//temp_sensor 4
#endif
	}
#endif


#if NODE_RELAYS_COUNT
	,{                                  //relay[]
#if NODE_RELAYS_COUNT > 0
										{6, 0}	//relay 1 {pin,pulse_start_milis}
#endif
#if NODE_RELAYS_COUNT > 1
										,{7, 0}	//relay 2
#endif
#if NODE_RELAYS_COUNT > 2
										,{15, 0}	//relay 3
#endif
#if NODE_RELAYS_COUNT > 3
										,{16, 0}	//relay 4
#endif
	}
#endif

};

void sms_cmd(char* cmdstr);
byte str_to_idx(char* pi);
unsigned long decode_milisec(char* time_cmd);

void print_query_string(Simcom* gsm);
void print_P(PGM_P str, bool CR = false);
void print_EE(uint16_t ptr);
bool strcmp_EE(char* str, uint16_t ee_ptr);
