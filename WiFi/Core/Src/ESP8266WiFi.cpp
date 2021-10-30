#include <ESP8266WiFi.h>
#include <ESP8266_AT.h>
#include <stdio.h>
#include <string.h>
#include "stdarg.h"
#include "stdlib.h"
#include "main.h"

using namespace std;

#define ESP8266_DISABLE_ECHO



////////////////////
// Initialization //
////////////////////

void ESP8266Class::print_debug(const char* fmt, ...) {
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(_serial_debug, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
    va_end(args);
}

void ESP8266Class::print(const char* fmt, ...) {
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(_serial, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
    //HAL_UART_Transmit(_serial_debug, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
    va_end(args);
}

ESP8266Class::ESP8266Class(UART_HandleTypeDef *serialPort, UART_HandleTypeDef *debugPort)
{
	_serial = serialPort;
	_serial_debug = debugPort;
}

bool ESP8266Class::begin()
{
	echo(0);
	return at_test();
}

bool ESP8266Class::at_test()
{
	sendCommand(ESP8266_TEST); // Send AT
	if (readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT) > 0)
		return true;
	return false;
}

bool ESP8266Class::echo(bool enable)
{
	if (enable)
		sendCommand(ESP8266_ECHO_ENABLE);
	else
		sendCommand(ESP8266_ECHO_DISABLE);

	if (readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT) > 0)
		return true;

	return false;
}

void ESP8266Class::sendCommand(const char * cmd, enum esp8266_command_type type, const char * params)
{
	print("AT");
	print(cmd);
	if (type == ESP8266_CMD_QUERY)
		print("?");
	else if (type == ESP8266_CMD_SETUP)
	{
		print("=");
		print(params);
	}
	print("\r\n");
}

int16_t ESP8266Class::connect(const char * ssid, const char * pwd)
{
	print("AT");
	print(ESP8266_CONNECT_AP);
	print("=\"");
	print(ssid);
	print("\"");
	if (pwd != NULL)
	{
		print(",");
		print("\"");
		print(pwd);
		print("\"");
	}
	print("\r\n");

	return readForResponses(RESPONSE_OK, RESPONSE_FAIL, WIFI_CONNECT_TIMEOUT);
}

int16_t ESP8266Class::disconnect()
{
	sendCommand(ESP8266_DISCONNECT); // Send AT+CWQAP
	// Example response: \r\n\r\nOK\r\nWIFI DISCONNECT\r\n
	// "WIFI DISCONNECT" comes up to 500ms _after_ OK.
	int16_t rsp = readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT);
	if (rsp > 0)
	{
		rsp = readForResponse("WIFI DISCONNECT", COMMAND_RESPONSE_TIMEOUT);
		if (rsp > 0)
			return rsp;
		return 1;
	}
	return rsp;
}

int16_t ESP8266Class::status()
{
	sendCommand(ESP8266_TCP_STATUS); // Send AT+GMR
	int16_t rsp = (readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT) > 0);
	if (rsp > 0)
	{
		char *p, *q;
		// Look for "AT version" in the rxBuffer
		p = strstr(esp8266RxBuffer, "STATUS:");
		if (p == NULL) return ESP8266_RSP_UNKNOWN;
		p += strlen("STATUS:");
		q = strchr(p, '\r'); // Look for \r
		if (q == NULL) return ESP8266_RSP_UNKNOWN;
		char * _s = new char(q-p);
		strncpy(_s, p, q-p);
		_status = atoi(_s);
	}
	return _status;
}

// setMode()
// Input: 1, 2, 3 (ESP8266_MODE_STA, ESP8266_MODE_AP, ESP8266_MODE_STAAP)
// Output:
//    - Success: >0
//    - Fail: <0 (esp8266_cmd_rsp)
int16_t ESP8266Class::setMode(esp8266_wifi_mode mode)
{
	char modeChar[2] = {0, 0};
	sprintf(modeChar, "%d", mode);
	sendCommand(ESP8266_WIFI_MODE, ESP8266_CMD_SETUP, modeChar);

	return readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT);
}

int16_t ESP8266Class::getMode()
{
	sendCommand(ESP8266_WIFI_MODE, ESP8266_CMD_QUERY);

	// Example response: \r\nAT+CWMODE_CUR?\r+CWMODE_CUR:2\r\n\r\nOK\r\n
	// Look for the OK:
	int16_t rsp = readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT);
	if (rsp > 0)
	{
		// Then get the number after ':':
		char * p = strchr(esp8266RxBuffer, ':');
		if (p != NULL)
		{
			char mode = *(p+1);
			if ((mode >= '1') && (mode <= '3'))
				return (mode - 48); // Convert ASCII to decimal
		}

		return ESP8266_RSP_UNKNOWN;
	}

	return rsp;
}

int16_t ESP8266Class::getVersion(char * ATversion, char * SDKversion, char * compileTime, char * Binversion)
{
	sendCommand(ESP8266_VERSION); // Send AT+GMR
	// Example Response: AT version:0.30.0.0(Jul  3 2015 19:35:49)\r\n (43 chars)
	//                   SDK version:1.2.0\r\n (19 chars)
	//                   compile time:Jul  7 2015 18:34:26\r\n (36 chars)
	//                   OK\r\n
	// (~101 characters)
	// Look for "OK":
	int16_t rsp = (readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT) > 0);
	print_debug("%s\r\n", esp8266RxBuffer);
	if (rsp > 0)
	{
		char *p;
		char *q;
		// Look for "AT version" in the rxBuffer
		p = strstr(esp8266RxBuffer, "AT version:");
		if (p == NULL) return ESP8266_RSP_UNKNOWN;
		p += strlen("AT version:");
		q = strchr(p, '\r'); // Look for \r
		if (q == NULL) return ESP8266_RSP_UNKNOWN;
		strncpy(ATversion, p, q-p);

		// Look for "SDK version:" in the rxBuffer
		p = strstr(esp8266RxBuffer, "SDK version:");
		if (p == NULL) return ESP8266_RSP_UNKNOWN;
		p += strlen("SDK version:");
		q = strchr(p, '\r'); // Look for \r
		if (q == NULL) return ESP8266_RSP_UNKNOWN;
		strncpy(SDKversion, p, q-p);

		// Look for "compile time:" in the rxBuffer
		p = strstr(esp8266RxBuffer, "compile time:");
		if (p == NULL) return ESP8266_RSP_UNKNOWN;
		p += strlen("compile time:");
		q = strchr(p, '\r'); // Look for \r
		if (q == NULL) return ESP8266_RSP_UNKNOWN;
		strncpy(compileTime, p, q-p);

		// Look for "compile time:" in the rxBuffer
		p = strstr(esp8266RxBuffer, "Bin version(Wroom 02):");
		if (p == NULL) return ESP8266_RSP_UNKNOWN;
		p += strlen("Bin version(Wroom 02):");
		q = strchr(p, '\r'); // Look for \r
		if (q == NULL) return ESP8266_RSP_UNKNOWN;
		strncpy(Binversion, p, q-p);
	}

	return rsp;
}

uint8_t ESP8266Class::localIP()
{
	sendCommand(ESP8266_GET_LOCAL_IP); // Send AT+CIFSR\r\n
	// Example Response: +CIFSR:STAIP,"192.168.0.114"\r\n
	//                   +CIFSR:STAMAC,"18:fe:34:9d:b7:d9"\r\n
	//                   \r\n
	//                   OK\r\n
	// Look for the OK:
	int16_t rsp = readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT);
	if (rsp > 0)
	{
		// Look for "STAIP" in the rxBuffer
		char * p = strstr(esp8266RxBuffer, "STAIP");
		if (p != NULL)
		{
			p += 7; // Move p seven places. (skip STAIP,")
			for (uint8_t i = 0; i < 4; i++)
			{
				char tempOctet[4];
				memset(tempOctet, 0, 4); // Clear tempOctet

				size_t octetLength = strspn(p, "0123456789"); // Find length of numerical string:
				if (octetLength >= 4) // If it's too big, return an error
					return ESP8266_RSP_UNKNOWN;

				strncpy(tempOctet, p, octetLength); // Copy string to temp char array:
				ip_address[i] = atoi(tempOctet); // Move the temp char into IP Address octet
				p += (octetLength + 1); // Increment p to next octet
			}
			print_debug("%d.%d.%d.%d\r\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
			return ESP8266_RSP_SUCCESS;
		}
	}

	return rsp;
}

int16_t ESP8266Class::localMAC()
{
	sendCommand(ESP8266_GET_STA_MAC, ESP8266_CMD_QUERY); // Send "AT+CIPSTAMAC?"

	int16_t rsp = readForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIMEOUT);

	if (rsp > 0)
	{
		// Look for "+CIPSTAMAC"
		char * p = strstr(esp8266RxBuffer, ESP8266_GET_STA_MAC);
		if (p != NULL)
		{
			p += strlen(ESP8266_GET_STA_MAC) + 2;
			char * q = strchr(p, '"');
			if (q == NULL) return ESP8266_RSP_UNKNOWN;
			strncpy(mac_address, p, q - p); // Copy string to temp char array:
			print_debug("%s\r\n", mac_address);
			return ESP8266_RSP_SUCCESS;
		}
	}

	return rsp;
}

int16_t ESP8266Class::readForResponse(const char * rsp, unsigned int timeout)
{
	unsigned long timeIn = HAL_GetTick();	// Timestamp coming into function
	unsigned int received = 0; // received keeps track of number of chars read

	clearBuffer();	// Clear the class receive buffer (esp8266RxBuffer)

	while (timeIn + timeout > HAL_GetTick()) // While we haven't timed out
	{
		received += readToBuffer(timeout);

		if (searchBuffer(rsp))	// Search the buffer for goodRsp
			return received;	// Return how number of chars read
	}

	if (received > 0) // If we received any characters
		return ESP8266_RSP_UNKNOWN; // Return unkown response error code
	else // If we haven't received any characters
		return ESP8266_RSP_TIMEOUT; // Return the timeout error code
}

int16_t ESP8266Class::readForResponses(const char * pass, const char * fail, unsigned int timeout)
{
	unsigned long timeIn = HAL_GetTick();	// Timestamp coming into function
	unsigned int received = 0; // received keeps track of number of chars read

	clearBuffer();	// Clear the class receive buffer (esp8266RxBuffer)

	while (timeIn + timeout > HAL_GetTick()) // While we haven't timed out
	{
		received += readToBuffer(timeout);

		if (searchBuffer(pass))	// Search the buffer for goodRsp
			return received;	// Return how number of chars read
		if (searchBuffer(fail))
			return ESP8266_RSP_FAIL;
	}
	if (received > 0) // If we received any characters
		return ESP8266_RSP_UNKNOWN; // Return unkown response error code
	else // If we haven't received any characters
		return ESP8266_RSP_TIMEOUT; // Return the timeout error code
}

unsigned int ESP8266Class::readToBuffer(unsigned int timeout)
{
	// Read the data in
	//HAL_UART_Receive_IT(_serial, &response_data, 1);

	HAL_UART_Receive(_serial, &response_data, 1, timeout);

	// Store the data in the buffer
	//print_debug("received: %c\r\n", data);
	esp8266RxBuffer[bufferHead] = response_data;

	bufferHead = (bufferHead + 1) % ESP8266_RX_BUFFER_LEN;

	return 1;
}

char * ESP8266Class::searchBuffer(const char * test)
{
	int bufferLen = strlen((const char *)esp8266RxBuffer);
	// If our buffer isn't full, just do an strstr
	if (bufferLen < ESP8266_RX_BUFFER_LEN)
		return strstr((const char *)esp8266RxBuffer, test);
	return 0;
}

//////////////////
// Buffer Stuff //
//////////////////
void ESP8266Class::clearBuffer()
{
	memset(esp8266RxBuffer, '\0', ESP8266_RX_BUFFER_LEN);
	bufferHead = 0;
}
