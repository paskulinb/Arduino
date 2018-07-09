#include <Arduino.h>
#include "mySoftwareSerial.h"
#include <EEPROM.h>

#define SMS_LIST_LENGTH 20

//ERRORs
enum {
	ERR_NOT_PRESENT = -1,
	ERR_PIN = -2,
	ERR_REG = -3,
	ERR_CGATT = -4,
	ERR_CIPSHUT = -5,
	ERR_CIPSTATUS = -6,
	ERR_CSTT = -7,
	ERR_CIICR = -8,
	ERR_CIFSR = -9,
	ERR_CIPSTART = -10,
	ERR_CIPSEND_1 = -11,
	ERR_CIPSEND_2 = -12,
	ERR_TCPCONN = -13,
	ERR_CCLK = -14,
	ERR_CNTP = -15,
	ERR_CIPCLOSE = -16
};

//Status
enum {
	STAT_PRESENT = 1,
	STAT_PIN_READY,
	STAT_GSM_REGISTERED,
	STAT_GPRS_READY,
	STAT_GPRS_DETACHED,
	STAT_HTTPGET_OK
};

struct GsmDateTime {
	uint8_t yy;
	uint8_t MM;
	uint8_t dd;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	int8_t zz;
};

class Simcom : public mySoftwareSerial
{
private:
	int8_t status;
	uint8_t sms_index_list[SMS_LIST_LENGTH];
	char sms_remote_number[20];
	char local_ip_address[16];
	char http_response[256];

public:
	Simcom(uint8_t rxpin, uint8_t txpin);
	int8_t begin(char* sim_pin);
	inline int getStatus(void) {return status;};
	uint8_t shutdown();
	uint8_t smsAvailable(void);
	uint8_t smsBegin(char* remote_number);
	uint8_t smsDelete(void);
	uint8_t smsEnd(void);
	uint8_t smsList(void);
	uint8_t smsRemoteNumber(char** remote_number);
	char smsRead(void);
	uint8_t smsFlush(void);
	//int tcp_connect(char* apn, char* user, char* password);
	int gprs_start(uint16_t eept_apn, uint16_t eept_user, uint16_t eept_password);
	int gprs_stop(void);
	int httpGET_request(uint16_t eept_host, uint16_t eept_path, uint16_t eept_port, void (*print_query_string)(Simcom*));
	//int tcp_httpPOST(Print* po);
	
	uint8_t getNTP(GsmDateTime& dt);
	uint8_t readDateTime(GsmDateTime& dt);
	
	size_t print_P(PGM_P str, bool CR = false);
	size_t print_AMP();
	size_t print_EE(uint16_t ptr);

private:
	uint8_t _sim_pin(char* sim_pin);
	uint8_t _register(void);
	uint8_t wait_response(char* response, unsigned long timeout);
	//uint8_t at_command(PGM_P str, unsigned long timeout, uint8_t retry, PGM_P response1);
	void rx_buff_dump(void);
};
