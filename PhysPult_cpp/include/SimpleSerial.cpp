#include "SimpleSerial.h"

SimpleSerial::SimpleSerial()
{
	io_handler_ = nullptr;
	connected_ = false;
}

SimpleSerial::SimpleSerial(char* com_port, DWORD COM_BAUD_RATE, std::string syntax_type)
{
	connected_ = false;

	io_handler_ = CreateFileA(static_cast<LPCSTR>(com_port),
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if (io_handler_ == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			std::cout << "Warning: Handle was not attached. Reason: " << com_port << " not available\n";
	}
	else {

		DCB dcbSerialParams = { 0 };

		if (!GetCommState(io_handler_, &dcbSerialParams)) {
			std::cout << "Warning: Failed to get current serial params\n";
		}

		else {
			dcbSerialParams.BaudRate = COM_BAUD_RATE;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if (SetCommState(io_handler_, &dcbSerialParams)) {
				connected_ = true;
				PurgeComm(io_handler_, PURGE_RXCLEAR | PURGE_TXCLEAR);
				CustomSyntax(syntax_type);
			}
			else {
				DWORD err = GetLastError();
				std::cout << "Warning: Could not set serial port params!" << err << "\n";
			}
		}
	}
}

void SimpleSerial::CustomSyntax(std::string syntax_type) {

	std::ifstream syntaxfile_exist("syntax_config.txt");

	if (!syntaxfile_exist) {		
		std::ofstream syntaxfile;
		syntaxfile.open("syntax_config.txt");

		if (syntaxfile) {
			syntaxfile << "json { }\n";
			syntaxfile << "greater_less_than < >\n";
			syntaxfile.close();
		}
	}

	syntaxfile_exist.close();
	
	std::ifstream syntaxfile_in;
	syntaxfile_in.open("syntax_config.txt");
	
	std::string line;
	bool found = false;	

	if (syntaxfile_in.is_open()) {

		while (syntaxfile_in) {			
			syntaxfile_in >> syntax_name_ >> front_delimiter_ >> end_delimiter_;
			getline(syntaxfile_in, line);			
			
			if (syntax_name_ == syntax_type) {
				found = true;
				break;
			}
		}

		syntaxfile_in.close();

		if (!found) {
			syntax_name_ = "";
			front_delimiter_ = ' ';
			end_delimiter_ = ' ';
			std::cout << "Warning: Could not find delimiters, may cause problems!\n";
		}
	}
	else
		std::cout << "Warning: No syntax file open";
}

std::string SimpleSerial::ReadSerialPort(int timeout)
{
	char inc_msg;	
	std::string complete_inc_msg;
	bool began = false;

	ClearCommError(io_handler_, &errors_, &status_);

	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() < timeout)
	{
		if (status_.cbInQue > 0) {		
			if (ReadFile(io_handler_, &inc_msg, 1, NULL, NULL))
			{
				if (inc_msg == front_delimiter_ || began) {
					began = true;

					if (inc_msg == end_delimiter_)
						return complete_inc_msg;

					if (inc_msg != front_delimiter_)
						complete_inc_msg += inc_msg;
				}				
			}
			else
				return "Warning: Failed to receive data.\n";
		}
	}
	return complete_inc_msg;		
}

bool SimpleSerial::WriteSerialPort(std::string data_sent)
{
	if (WriteFile(io_handler_, &data_sent[0], data_sent.length(), NULL, NULL)) return true;
	else {
		DWORD err = GetLastError();
		std::cout << "WriteFile error! " << err << "\n";

		ClearCommError(io_handler_, &errors_, &status_);
		return false;
	}
}

bool SimpleSerial::CloseSerialPort()
{
	if (connected_) {
		connected_ = false;
		CloseHandle(io_handler_);

		return true;
	}	
	else
		return false;
}

SimpleSerial::~SimpleSerial()
{
	//std::cout << connected_ << ' ' << io_handler_ << ' ' << "destr\n";
	CloseSerialPort();
}