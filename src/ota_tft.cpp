/**
 * @file ota_tft.cpp
 *
 * The implementation of uploading tft file for nextion displays. 
 * 
 * Original version (a part of https://github.com/itead/ITEADLIB_Arduino_Nextion)
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

// #define DEBUG_SERIAL_ENABLE
#include "ota_tft.h"
#include "NexConfig.h"
#include "autopool.h"

#ifndef NEXT_RX
#define NEXT_RX 16 // Nextion RX pin | Default 16
#define NEXT_TX 17 // Nextion TX pin | Default 17
#endif
#ifndef nexSerial
#define nexSerial Serial2
#endif

#ifndef nexSerialBegin
#define nexSerialBegin(a) nexSerial.begin(a, SERIAL_8N1, NEXT_RX, NEXT_TX)
#endif




ota_tft::ota_tft(uint32_t upload_baudrate)
{
	_upload_baudrate = upload_baudrate;
}

bool ota_tft::connect()
{
#if defined ESP8266
	yield();
#endif
	delay(10);
	dbSerialBegin(115200);
	printlnA(F("serial tests & connect"));

	while (nexSerial.available())
	{
		nexSerial.read();
	}

	if (_getBaudrate() == 0)
	{
		statusMessage = F("get baudrate error");
		printlnA(statusMessage);
		return false;
	}

	_setRunningMode();

	if (!_echoTest("mystop_yesABC"))
	{
		statusMessage = F("echo test failed");
		printlnA(statusMessage);
		return false;
	}

	if (!_handlingSleepAndDim())
	{
		statusMessage = F("handling sleep and dim settings failed");
		printlnA(statusMessage);
		return false;
	}

	if (!_setPrepareForFirmwareUpdate(_upload_baudrate))
	{
		statusMessage = F("modifybaudrate error");
		printlnA(statusMessage);
		return false;
	}
	printlnA("Upload Connect Done");

	return true;
}

bool ota_tft::prepareUpload(uint32_t file_size)
{
	_undownloadByte = file_size;
	return this->connect();
}

uint16_t ota_tft::_getBaudrate(void)
{

	_baudrate = 0;
	uint32_t baudrate_array[7] = {115200, 19200, 9600, 57600, 38400, 4800, 2400};
	for (uint8_t i = 0; i < 7; i++)
	{
		if (_searchBaudrate(baudrate_array[i]))
		{
			_baudrate = baudrate_array[i];
			printlnA(F("baudrate determined"));
			break;
		}
		delay(1500); // wait for 1500 ms
	}
	return _baudrate;
}

bool ota_tft::_searchBaudrate(uint32_t baudrate)
{

#if defined ESP8266
	yield();
#endif

	String response = String("");
	printA(F("init nextion serial interface on baudrate: "));
	printlnA(baudrate);
	//nexSerial.end();
	//nexSerial.begin(baudrate);

	//nexSerialBegin(baudrate);
	printlnA(F("ESP baudrate established, try to connect to display"));
	const char _nextion_FF_FF[3] = {0xFF, 0xFF, 0x00};

	this->sendCommand("DRAKJHSUYDGBNCJHGJKSHBDN");
	this->sendCommand("", true, true); // 0x00 0xFF 0xFF 0xFF

	this->recvRetString(response);
	if (response[0] != 0x1A)
	{
		printlnA(F("first indication that baudrate is wrong"));
	}
	else
	{
		printlnA(F("first respone from display, first indication that baudrate is correct"));
	}

	this->sendCommand("connect"); // first connect attempt

	this->recvRetString(response);
	if (response.indexOf(F("comok")) == -1)
	{
		printlnA(F("display doesn't accept the first connect request"));
	}
	else
	{
		printlnA(F("display accept the first connect request"));
	}

	response = String("");
	delay(110); // based on serial analyser from Nextion editor V0.58 to Nextion display NX4024T032_011R
	this->sendCommand(_nextion_FF_FF, false);

	this->sendCommand("connect"); // second attempt
	this->recvRetString(response);
	if (response.indexOf(F("comok")) == -1 && response[0] != 0x1A)
	{
		printlnA(F("display doesn't accept the second connect request"));
		printlnA(F("conclusion, wrong baudrate"));
		return 0;
	}
	else
	{
		printlnA(F("display accept the second connect request"));
		printlnA(F("conclusion, correct baudrate"));
	}

	return 1;
}

void ota_tft::sendCommand(const char *cmd, bool tail, bool null_head)
{

#if defined ESP8266
	yield();
#endif

	if (null_head)
	{
		nexSerial.write(0x00);
	}

	while (nexSerial.available())
	{
		nexSerial.read();
	}

	nexSerial.print(cmd);
	if (tail)
	{
		nexSerial.write(0xFF);
		nexSerial.write(0xFF);
		nexSerial.write(0xFF);
	}
	_printSerialData(true, cmd);
}

uint16_t ota_tft::recvRetString(String &response, uint32_t timeout, bool recv_flag)
{

#if defined ESP8266
	yield();
#endif

	uint16_t ret = 0;
	uint8_t c = 0;
	uint8_t nr_of_FF_bytes = 0;
	long start;
	bool exit_flag = false;
	bool ff_flag = false;
	//if (timeout != 500)
	printlnA("timeout setting serial read: " + String(timeout));

	start = millis();

	while ((millis() - start) <= timeout)
	{
		while (nexSerial.available())
		{
			c = nexSerial.read();
			if (c == 0)
			{
				continue;
			}
			if (c == 0xFF)
			{
				nr_of_FF_bytes++;
			}
			else
			{
				nr_of_FF_bytes = 0;
				ff_flag = false;
			}

			if (nr_of_FF_bytes >= 3)
			{
				ff_flag = true;
			}
			response += (char)c;
			if (recv_flag)
			{
				if (response.indexOf(0x05) != -1)
				{
					exit_flag = true;
				}
			}
		}
		if (exit_flag || ff_flag)
		{
			break;
		}
	}
	_printSerialData(false, response);

	// if the exit flag and the ff flag are both not found, than there is a timeout
	// if(!exit_flag && !ff_flag)
	// _printInfoLine(F("recvRetString: timeout"));

	if (ff_flag)
	{
		response = response.substring(0, response.length() - 3); // Remove last 3 0xFF
	}
	ret = response.length();
	return ret;
}

bool ota_tft::_setPrepareForFirmwareUpdate(uint32_t upload_baudrate)
{

#if defined ESP8266
	yield();
#endif

	String response = String("");
	String cmd = String("");

	cmd = F("00");
	this->sendCommand(cmd.c_str());
	delay(0.1);

	this->recvRetString(response, 800, true); // normal response time is 400ms

	String filesize_str = String(_undownloadByte, 10);
	String baudrate_str = String(upload_baudrate);
	cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";

	this->sendCommand(cmd.c_str());

	// Without flush, the whmi command will NOT transmitted by the ESP in the current baudrate
	// because switching to another baudrate (nexSerialBegin command) has an higher prio.
	// The ESP will first jump to the new 'upload_baudrate' and than process the serial 'transmit buffer'
	// The flush command forced the ESP to wait until the 'transmit buffer' is empty
	//
	//nexSerial.flush();
	printA(F("changing upload baudrate : "));
	printlnA(upload_baudrate);

	//nexSerialBegin(upload_baudrate);
	printlnA(F("changed upload baudrate : Skipped ..."));
	printlnA(String(upload_baudrate));

	this->recvRetString(response, 800, true); // normal response time is 400ms

	// The Nextion display will, if it's ready to accept data, send a 0x05 byte.
	if (response.indexOf(0x05) != -1)
	{
		printlnA(F("preparation for firmware update done"));
		return 1;
	}
	else
	{
		printlnA(F("preparation for firmware update failed"));
		return 0;
	}
}

void ota_tft::setUpdateProgressCallback(THandlerFunction value)
{
	_updateProgressCallback = value;
}

bool ota_tft::upload(const uint8_t *file_buf, size_t buf_size)
{

#if defined ESP8266
	yield();
#endif

	uint8_t c;
	uint8_t timeout = 0;
	String string = String("");
	printA("Buf Size : ");
	printlnA(buf_size);

	for (uint16_t i = 0; i < buf_size; i++)
	{

		// Users must split the .tft file contents into 4096 byte sized packets with the final partial packet size equal to the last remaining bytes (<4096 bytes).
		if (_sent_packets == 4096)
		{

			// wait for the Nextion to return its 0x05 byte confirming reception and readiness to receive the next packets
			this->recvRetString(string, 600, true);
			if (string.indexOf(0x05) != -1)
			{

				// reset sent packets counter
				_sent_packets = 0;

				// reset receive String
				string = "";
			}
			else
			{
				if (timeout >= 8)
				{
					statusMessage = F("serial connection lost");
					printlnA(statusMessage);
					return false;
				}

				timeout++;
			}

			// delay current byte
			i--;
		}
		else
		{

			// read buffer
			c = file_buf[i];

			// write byte to nextion over serial
			nexSerial.write(c);

			// update sent packets counter
			_sent_packets++;
		}
	}

	return true;
}

bool ota_tft::upload(Stream &myFile)
{
#if defined ESP8266
	yield();
#endif

	// create buffer for read
	uint8_t buff[2048] = {0};

	// read all data from server
	while (_undownloadByte > 0 || _undownloadByte == -1)
	{

		// get available data size
		size_t size = myFile.available();

		if (size)
		{
			// read up to 2048 byte into the buffer
			int c = myFile.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

			// Write the buffered bytes to the nextion. If this fails, return false.
			if (!this->upload(buff, c))
			{
				return false;
			}
			else
			{
				if (_updateProgressCallback)
				{
					_updateProgressCallback();
				}
			}

			if (_undownloadByte > 0)
			{
				_undownloadByte -= c;
			}
		}
		delay(1);
	}

	return true;
}

void ota_tft::softReset(void)
{
	// soft reset nextion device
	this->sendCommand("rest");
}

void ota_tft::end()
{

	// wait for the nextion to finish internal processes
	delay(1600);

	// soft reset the nextion
	this->softReset();

	// end Serial connection
	nexSerial.end();

	// reset sent packets counter
	_sent_packets = 0;

	statusMessage = F("upload ok");
	printlnA(statusMessage + F("\r\n"));
}

void ota_tft::_setRunningMode(void)
{
	String cmd = String("");
	delay(100);
	cmd = F("runmod=2");
	this->sendCommand(cmd.c_str());
	delay(60);
}

bool ota_tft::_echoTest(String input)
{
	String cmd = String("");
	String response = String("");

	cmd = "print \"" + input + "\"";
	this->sendCommand(cmd.c_str());

	uint32_t duration_ms = calculateTransmissionTimeMs(cmd) * 2 + 10; // times 2  (send + receive) and 10 ms extra
	this->recvRetString(response, duration_ms);

	return (response.indexOf(input) != -1);
}

bool ota_tft::_handlingSleepAndDim(void)
{
	String cmd = String("");
	String response = String("");
	bool set_sleep = false;
	bool set_dim = false;

	cmd = F("get sleep");
	this->sendCommand(cmd.c_str());

	this->recvRetString(response);

	if (response[0] != 0x71)
	{
		statusMessage = F("unknown response from 'get sleep' request");
		printlnA(statusMessage);
		return false;
	}

	if (response[1] != 0x00)
	{
		printlnA(F("sleep enabled"));
		set_sleep = true;
	}
	else
	{
		printlnA(F("sleep disabled"));
	}

	response = String("");
	cmd = F("get dim");
	this->sendCommand(cmd.c_str());

	this->recvRetString(response);

	if (response[0] != 0x71)
	{
		statusMessage = F("unknown response from 'get dim' request");
		printlnA(statusMessage);
		return false;
	}

	if (response[1] == 0x00)
	{
		printlnA(F("dim is 0%, backlight from display is turned off"));
		set_dim = true;
	}
	else
	{
		printA(F("dim "));
		printA((uint8_t)response[1]);
		printlnA(F("%"));
	}

	printlnA(F("Echo test"));
	if (!_echoTest("ABC"))
	{
		statusMessage = F("echo test in 'handling sleep and dim' failed");
		printlnA(statusMessage);
		return false;
	}

	printlnA(F("Set Sleep"));
	if (set_sleep)
	{
		cmd = F("sleep=0");
		this->sendCommand(cmd.c_str());
		// Unfortunately the display doesn't send any respone on the wake up request (sleep=0)
		// Let the ESP wait for one second, this is based on serial analyser from Nextion editor V0.58 to Nextion display NX4024T032_011R
		// This gives the Nextion display some time to wake up
		delay(1000);
	}

	printlnA(F("Set Dim if needed"));
	if (set_dim)
	{
		cmd = F("dim=100");
		this->sendCommand(cmd.c_str());
		delay(15);
	}
	printlnA(F("Handle Sleep and Dim Done"));
	return true;
}

void ota_tft::_printSerialData(bool esp_request, String input)
{

	char c;
	if (esp_request)
	{
		printlnA(F("ESP request: "));
	}
	else
	{
		printlnA(F("Nextion response: "));
	}
	if (input.length() == 0)
	{
		printlnA(F("none"));
		return;
	}

	for (int i = 0; i < input.length(); i++)
	{

		c = input[i];
		if ((uint8_t)c >= 0x20 && (uint8_t)c <= 0x7E)
		{
			printA(c);
		}
		else
		{
			printA(F("0x"));
			printA(c, HEX);
			printA(F(" "));
		}
	}
	printlnA();
}

uint32_t ota_tft::calculateTransmissionTimeMs(String message)
{
	// In general, 1 second (s) = 1000 (10^-3) millisecond (ms) or
	//             1 second (s) = 1000 000 (10^-6) microsecond (us).
	// To calculate how much microsecond one BIT of data takes with a certain baudrate you have to divide
	// the baudrate by one second.
	// For example 9600 baud = 1000 000 us / 9600 ≈ 104 us
	// The time to transmit one DATA byte (if we use default UART modulation) takes 10 bits.
	// 8 DATA bits and one START and one STOP bit makes 10 bits.
	// In this example (9600 baud) a byte will take 1041 us to send or receive.
	// Multiply this value by the length of the message (number of bytes) and the total transmit/ receive time
	// is calculated.

	uint32_t duration_one_byte_us = 10000000 / _baudrate; // 1000 000 * 10 bits / baudrate
	uint16_t nr_of_bytes = message.length() + 3;		  // 3 times 0xFF byte
	uint32_t duration_message_us = nr_of_bytes * duration_one_byte_us;
	uint32_t return_value_ms = duration_message_us / 1000;

	printA("calculated transmission time: ");
	printA(String(return_value_ms));
	printlnA(" ms");
	return return_value_ms;
}

void ota_tft::_printInfoLine(String line)
{
	return;
	dbSerialPrint(F("Status     info: "));
	if (line.length() != 0)
		dbSerialPrintln(line);
}