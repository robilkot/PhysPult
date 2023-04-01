#define WIN32_LEAN_AND_MEAN

#include <list>
#include <string>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

constexpr const char* LOCALHOST = "127.0.0.1";

//#define DEBUG

std::list<int> GetCOMports()
{
	wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
	std::list<int> portList;

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		std::wstring str = L"COM" + std::to_wstring(i); // converting to COM0, COM1, COM2

		if (QueryDosDeviceW(str.c_str(), lpTargetPath, 5000)) //QueryDosDeviceW returns zero if it didn't find an object
		{
			portList.push_back(i);
			std::cout << std::string(str.begin(), str.end()) << "\n";
		}
		//if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) NULL;
	}
	return portList;
}

std::string SelectCOMport()
{
	std::string com_port = "\\\\.\\COM";
	do {
		std::list<int> COMports = GetCOMports();
		if (COMports.empty()) {
			std::cerr << "No COM ports available! Press q to exit or any other key to retry.\n\n";
			if (_getch() == 'q') exit(EXIT_SUCCESS);
			continue;
		}
		else if (COMports.size() == 1) {
			com_port += std::to_string(*COMports.begin());
		}
		else {
			std::cout << "Input COM port number and press Enter\n";
			short number = 0;
			std::cin >> number;
			com_port += std::to_string(number);
		}
		return com_port;
	} while (true);
}

class PhysPult {
private:
	int socketPort = 61000,
		frequency = 10,
		interval = 100,
		serialIndicatorsMessageLength = 9,
		socketIndicatorsMessageLength = 64,
		serialSwitchesMessageLength = 64,
		socketSwitchesMessageLength = 64;
	DWORD baudRate = 9600;

	std::string configPath = "physpult_config.txt",
				COMport = "\\\\.\\COM1",
				indicators = std::string(socketIndicatorsMessageLength, '0'),
				switches = std::string(socketSwitchesMessageLength, '0');

	SimpleSerial serial;
	TcpClient socket = *new TcpClient(LOCALHOST, socketPort);

	void serialConnect() {
		COMport = SelectCOMport();

		do {
			serial.~SimpleSerial(); // Close previous connection

			serial = *new SimpleSerial(&COMport[0], baudRate, "json");

			if (serial.connected_) {
				break;
			}
			else {
				std::cerr << "Failed to connect to COM port! Press 'q' to exit or any other key to select another COM port.\n";
				switch (_getch()) {
				case 'q': exit(EXIT_SUCCESS);
				default: COMport = SelectCOMport();
				}
			}
		} while (true);
	}

	void socketConnect() {
		socket = *new TcpClient(LOCALHOST, socketPort);

#ifndef DEBUG
		do {
			try
			{
				socket.Connect();
				break;
			}
			catch (SocketException& ex)
			{
				std::cerr << "Failed to connect to socket: error code " << ex.GetWSErrorCode() << "\n"
					<< "Press 'q' to exit, 'r' to reload config or any other key to retry.\n";
				switch (_getch())
				{
				case 'q': exit(EXIT_SUCCESS);
				case 'r': reloadConfig(configPath);
				}
			}
		} while (true);
#endif // !DEBUG 
	}

// Check for socket error codes here: https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

	void SendToSocket(std::string msg)
	{
		try {
			socket.Send(&msg[0], msg.length());
			std::cout << "Socket wrt [" << msg << "]\n";
		}
		catch (SocketException& ex) {
			std::cout << "Socket wrt: error code " << ex.GetWSErrorCode() << "\n";
			socketReconnect();
		}
	}

	std::string ReceiveFromSocket()
	{
		std::string out = std::string(socketIndicatorsMessageLength, '0') + '\0';

		try {
			socket.Recv(&out[0], socketIndicatorsMessageLength + 1);
			std::cout << "Socket rec [" << out << "]\n";
		}
		catch (SocketException& ex) {
			std::cout << "Socket rec: error code " << ex.GetWSErrorCode() << "\n";

			this->socketReconnect();
		}

		return out;
	}

	std::string convertToBytes(std::string source) { // string of 2 digits speed and row of 0 and 1 converted to MSB first binary notation
		std::string output;
		
		unsigned char speed = atoi(source.substr(0, 2).c_str());
		output += speed;
		
		unsigned char currentRegister = 0;
		for (int i = 0; i < source.length() - 2; i++)
		{
			currentRegister |= source[i + 2] - '0' << 7 - (i % 8);
			if ((i + 1) % 8 == 0) {
				//std::cout << (unsigned)currentRegister << '\t';
				output += currentRegister;
				currentRegister = 0;
			}
		}
		output += currentRegister;

		return output;
	}

public:

	PhysPult(std::string configPath) {
		if (!configPath.empty()) this->configPath = configPath;

		reloadConfig(this->configPath);

		socketConnect();
		serialConnect();
	};

	void reloadConfig(std::string configPath)
	{
		std::ifstream config(configPath);
		if (config.is_open()) {
			config >> socketPort >> frequency >> baudRate >>
				serialIndicatorsMessageLength >> socketIndicatorsMessageLength >>
				serialSwitchesMessageLength >> socketSwitchesMessageLength; // Get values from config

			interval = 1000 / frequency;

			indicators = std::string(socketIndicatorsMessageLength, '0'),
			switches = std::string(socketSwitchesMessageLength, '0');
		}
		else std::cerr << "Couldn't open config file! Using previously set parameters.\n";

		std::string metrostroiDataPath;
		getline(config, metrostroiDataPath);
		getline(config, metrostroiDataPath);

		std::ofstream luaConfig(metrostroiDataPath + "\\physpult.txt");
		if (!config.is_open())
			std::cerr << "Couldn't save config file to data!\n";

		luaConfig << socketPort << " " << frequency << " " << socketIndicatorsMessageLength << " " << socketSwitchesMessageLength; // Write values to lua config

		//std::cout << "Initialized!" << "\nPort: " << socketPort << "; Frequency: " << frequency << "; Baud rate: " << baudRate
		//	<< "; Indicators: " << indicatorsMessageLength << "; Switches: " << switchesMessageLength << "\n\n";
	}

	void serialReconnect() {
		serial.~SimpleSerial(); // Close previous connection

		serial = *new SimpleSerial(&COMport[0], baudRate, "json");

		if (!serial.connected_) {
			std::cerr << "Failed to reconnect to COM port! Select another COM port.\n";
			serialConnect();
		}
	}
	
	void socketReconnect() {
		socketConnect();
	}

	void updateSerial(bool& stop, bool& pause)
	{
		while (!stop)
		{
			auto beginTime = std::chrono::high_resolution_clock::now();
			//---
			if (!pause)
			{
				std::string indicators_t = /* '{' + */ convertToBytes(indicators) + '}';
				std::cout << "Serial wrt " /*<< indicators_t << " "*/ << serial.WriteSerialPort(indicators_t /*+ '\0'*/) << "\n";


				//std::string switches_t = serial.ReadSerialPort(10);
				//std::cout << "Serial rec {" << switches_t << "}\n";

				//if (switches_t.length() == serialSwitchesMessageLength) switches = switches_t;
			}
			//---
			int deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
			if (deltaTime < interval) std::this_thread::sleep_for(std::chrono::milliseconds(interval - deltaTime));
		}
	}

	void updateSocket(bool& stop, bool& pause)
	{
		while (!stop)
		{
			auto beginTime = std::chrono::high_resolution_clock::now();
			//---
			if (!pause) {

				std::string indicators_t = ReceiveFromSocket();

				if (indicators_t.length() == socketIndicatorsMessageLength + 1) indicators = indicators_t;

				SendToSocket(switches + '\0');
			}
			//---
			int deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
			if (deltaTime < interval) std::this_thread::sleep_for(std::chrono::milliseconds(interval - deltaTime));
		}
	}
};


int main(int argc, char* argv[])
{   
	std::string arg = argv[1] == NULL ? "physpult_config.txt" : argv[1];
	PhysPult physpult(arg);

	std::cout << "Starting. Press 'space' to stop or 'q' to exit.\n\n";

	bool stop = 0, pause = 0, reload = 0;

	std::thread socketThread([&]() { physpult.updateSocket(stop, pause); }),
				serialThread([&]() { physpult.updateSerial(stop, pause); });
   
	socketThread.detach();
	serialThread.detach();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(250)); // Interval for stop check is 100 ms, but could be anything else. Needed for optimisation

		if (_kbhit()) {
			switch (_getch()) {
			case ' ': {
				pause = 1;
				std::cout << "Paused! Press any key to continue, 'r' to reload config or 'q' to exit.\n";

				switch (_getch()) { // While paused
				case 'r': reload = 1; break;
				case 'q': stop = 1; break;
				default: pause = 0; break;
				}
				break;
			}
			case 'r': reload = 1; break;
			case 'q': stop = 1; break;
			}

			if (reload) {
				reload = 0;
				pause = 1;
				physpult.reloadConfig(arg);
				physpult.serialReconnect();
				physpult.socketReconnect();
				pause = 0;
			}
			if (stop) break; // Break from while(true) loop
		} 
	}
}