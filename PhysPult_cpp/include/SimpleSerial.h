#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;

class SimpleSerial
{
private:
	HANDLE io_handler_;
	COMSTAT status_ = { 0 };
	DWORD errors_ = 0;

	string syntax_name_;
	char front_delimiter_ = ' ';
	char end_delimiter_ = ' ';

	void CustomSyntax(string syntax_type);	

public:
	SimpleSerial(char* com_port, DWORD COM_BAUD_RATE, string syntax_type);

	string ReadSerialPort(int timeout);
	bool WriteSerialPort(char *data_sent);
	bool CloseSerialPort();
	~SimpleSerial();
	bool connected_;
};

