#include "Simcom.h"

Simcom::Simcom(uint8_t rxpin, uint8_t txpin) : mySoftwareSerial(rxpin, txpin, false)
{
	mySoftwareSerial::begin(9600);
}

int8_t Simcom::begin(char* sim_pin=NULL)
{
	uint8_t res;
	uint8_t retry;
	
	delay(15000); //Počakaj 15 sekund, da se modul po powerOn vzpostavi in 'izpljuva' sporočila "Call Ready" in "SMS Ready"
	
	retry = 10;
present:
	flush();
	print_P(PSTR("AT"), true);
	if (wait_response(NULL, 500)) {
		status = STAT_PRESENT;
	} else {
		if (--retry) goto present;
		status = ERR_NOT_PRESENT;
		return status;
	}

//	wait_response("SMS Ready", 30000); //Modul sam od sebe sporoči "Call Ready" in "SMS Ready". To počakaj, potem nadaljuj.
	
	flush();
	print_P(PSTR("ATE0"), true);
	wait_response(NULL, 1000);
	
	res = _sim_pin(sim_pin);
	
	if (res < 0) return res;

	res = _register();

	return res;
}

uint8_t Simcom::_sim_pin(char* sim_pin = NULL)
{
	flush();
	print_P(PSTR("AT+CPIN=?"), true);
	if (!wait_response("READY", 1000)) {
	
		if (sim_pin != NULL) {

			flush();
			print_P(PSTR("AT+CPIN=\"")); print(sim_pin); print_P(PSTR("\""), true);
			if (wait_response("READY", 10000)) status = STAT_PIN_READY;
			else status = ERR_PIN;
		}
	}

	return status;
}

uint8_t Simcom::_register(void)
{
	uint8_t retry;

	retry = 30;
creg:
	flush();
	print_P(PSTR("AT+CREG?"), true);
	if (wait_response("0,1", 1000)) status = STAT_GSM_REGISTERED;
	else {
		if (--retry) goto creg;
		status = ERR_REG;
	}
	
	return status;
}

uint8_t Simcom::smsAvailable(void)
{
	byte count = smsList();
	if (count == 0) return 0;

	//Find remote_number (first SMS only)
	print_P(PSTR("AT+CMGR=")); print(sms_index_list[0]); print_P(PSTR(""), true);
	setTimeout(3000);
	if (!find(",\"")) return 0;
	byte n = readBytesUntil('"', sms_remote_number, 20);
	if (n == 0) return 0;
	sms_remote_number[n] = '\0';
	//and move to message content
	find("\r\n");
	
	return count;
}

uint8_t Simcom::smsBegin(char* remote_number)
{
	print_P(PSTR("AT+CMGF=1"), true);
	delay(100);
	print_P(PSTR("AT+CMGS=\"")); print(remote_number); print_P(PSTR("\""), true);
	delay(100);
}

uint8_t Simcom::smsEnd(void)
{
	print((char)26);
}

uint8_t Simcom::smsList(void)
{
	print_P(PSTR("AT+CMGF=1"), true);
	delay(100);
	print_P(PSTR("AT+CMGL=\"ALL\""), true);
	
	uint8_t i;
	
	for (i = 0; i < SMS_LIST_LENGTH; i++) sms_index_list[i] = 0;
	
	i = 0;
	
	while (1) {
		setTimeout(1000);
		if (!find("+CMGL: ")) break;
		sms_index_list[i] = parseInt();
		i++;
		if (i == SMS_LIST_LENGTH) break;
	}

	return i; //return total number of stored SMSs in GSM module
}

uint8_t Simcom::smsDelete(void)
{
	print_P(PSTR("AT+CMGD=")); print(sms_index_list[0]); print_P(PSTR(""), true);
}

uint8_t Simcom::smsFlush(void)
{
}

char Simcom::smsRead(void)
{
	char c = timedRead();
	if (c == '\r') return 0;
	return c;
}

uint8_t Simcom::smsRemoteNumber(char** remote_number)
{
	*remote_number = sms_remote_number;
	return 0;
}

size_t Simcom::print_P(PGM_P str, bool CR)
{
	int i=0;
	char c;
	
	do
	{	
		c=pgm_read_byte_near(str + i); 
		if(c!=0)
			write(c);
		i++;
	} while (c!=0);
	if(CR)
		print_P(PSTR("\r\n"), false);
		
	return 1;
}

size_t Simcom::print_EE(uint16_t ptr)
{
	char c;
	while (0 != (c=EEPROM[ptr++])) write(c);
	return 1;
}

int Simcom::gprs_start(uint16_t eept_apn, uint16_t eept_user, uint16_t eept_password)
//int Simcom::gprs_begin(char* apn, char* user, char* password)
{
cgatt:
	flush();

	print_P(PSTR("AT+CGATT=1"), true);
	if (!wait_response(NULL, 10000)) return ERR_CGATT;
	flush();

	print_P(PSTR("AT+CGATT?"), true);
	if (!wait_response("1", 10000)) goto cgatt;//return ERR_CGATT;
	flush();
	
	print_P(PSTR("AT+CIPSHUT"), true);
	if (!wait_response(NULL, 3000)) return ERR_CIPSHUT;
	flush();

	print_P(PSTR("AT+CIPSTATUS"), true);
	if (!wait_response("STATE: IP INITIAL", 1000)) return ERR_CIPSTATUS;
	flush();

	print_P(PSTR("AT+CSTT=\"")); print_EE(eept_apn); print("\",\""); print_EE(eept_user); print("\",\""); print_EE(eept_password); print("\"\r");
	//print_P(PSTR("AT+CSTT=\"")); print(apn); print("\",\""); print(user); print("\",\""); print(password); print("\"\r");
	if (!wait_response(NULL, 5000)) return ERR_CSTT;
	flush();

	print_P(PSTR("AT+CIICR"), true); //bring up wireless
	if (!wait_response(NULL, 10000)) return ERR_CIICR;
	flush();
	
	print_P(PSTR("AT+CIFSR"), true); //get IP
	if (!wait_response("\r\n", 5000)) return ERR_CIFSR;
	byte n = readBytesUntil('\r', local_ip_address, 16);
	local_ip_address[15] = '\0';
	flush();
	
//	Serial.println(local_ip_address);

	return STAT_GPRS_READY;
}

int Simcom::gprs_stop(void)
{
	flush();

	print_P(PSTR("AT+CGATT=0"), true);
	if (!wait_response(NULL, 10000)) return ERR_CGATT;
	
	return STAT_GPRS_DETACHED;
}


int Simcom::httpGET_request(uint16_t eept_host, uint16_t eept_path, uint16_t eept_port, void (*print_query_string)(Simcom*))
{
	print_P(PSTR("AT+CIPSTART=\"TCP\",\"")); print_EE(eept_host); print_P(PSTR("\",\"")); print_EE(eept_port); println("\"");
	if (!wait_response("CONNECT OK", 10000)) return ERR_CIPSTART;
	flush();

	print_P(PSTR("AT+CIPSEND"), true);
	if (!wait_response(">", 1000)) return ERR_CIPSEND_1;
	flush();

	print("GET "); print_EE(eept_path); print("?"); print_query_string(this); println(" HTTP/1.1");
    print("Host: "); print_EE(eept_host); println();
    println("Connection: close");
    println();
	print((char)26);

	if (!wait_response("SEND OK", 2000)) return ERR_CIPSEND_2;
	find("\n\n");
	setTimeout(1000);
	byte n = readBytes(http_response, 255);
	http_response[n] = '\0';

	flush();
	print_P(PSTR("AT+CIPCLOSE=1"), true);
	//print_P(PSTR("AT+CIPSHUT"));
	if (!wait_response(NULL, 10000)) return ERR_CIPCLOSE;

	return STAT_HTTPGET_OK;
}

uint8_t Simcom::getNTP(GsmDateTime& dt)
{
	print_P(PSTR("AT+CNTP=\"pool.ntp.org\""), true);
	if (!wait_response(NULL, 3000)) return ERR_CNTP;

	print_P(PSTR("AT+CNTP?"), true);
	if (!wait_response("+CNTP: \"", 3000)) return ERR_CNTP;

	byte n = readBytes(http_response, 255);
	http_response[n] = '\0';
}


uint8_t Simcom::readDateTime(GsmDateTime& dt)
{
	char val[4] = {0};
	print_P(PSTR("AT+CCLK?"), true);
	if (!wait_response("+CCLK: \"", 3000)) return ERR_CCLK;

	dt.yy = parseInt();
	read();
	dt.MM = parseInt();
	read();
	dt.dd = parseInt();
	read();
	dt.hh = parseInt();
	read();
	dt.mm = parseInt();
	read();
	dt.ss = parseInt();
	dt.zz = parseInt();
	
	flush();
	
	return 1;
}


uint8_t Simcom::wait_response(char* response, unsigned long timeout)
{
	setTimeout(timeout);
	if (response == NULL) response = "OK";
	//if (strlen(response) == 0) response = "OK";
	if (!find(response)) return 0;
	return 1;
}

void Simcom::rx_buff_dump(void)
{
	char* buff = get_rx_buff();

	for (int i = 0; i < 256; i++) {
		Serial.print((char)buff[(uint8_t)i]);
	}

	return;
}

size_t Simcom::print_AMP()
{
	print_P(PSTR("&"));
}
