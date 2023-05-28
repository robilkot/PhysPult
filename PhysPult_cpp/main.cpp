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

//#define PERFILE // Directive for usage of data transmission per file instead of sockets
//#define NOSOCKET // Directive for disabling connection via sockets
//#define NOSERIAL // Directive for disabling connection via serial

std::list<int> GetCOMports()
{
	std::list<int> portList;
	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		std::wstring portName = L"\\\\.\\COM" + std::to_wstring(i);
		HANDLE hPort = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPort != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hPort); // close the handle to release the port
			portList.push_back(i);
			std::wcout << portName << std::endl; // print the port name
		}
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

		serialFrequency = 30,
		serialInterval = 33,
		serialIndicatorsMessageLength = 64,
		serialSwitchesMessageLength = 64,

		socketFrequency = 60,
		socketInterval = 33,
		socketIndicatorsMessageLength = 64,
		socketSwitchesMessageLength = 64;

	DWORD baudRate = 9600;

	std::string configPath = "physpult_config.txt",
		COMport = "\\\\.\\COM1",
		metrostroiDataPath = "",
		indicators = std::string(socketIndicatorsMessageLength, '0'),
		switches = std::string(socketSwitchesMessageLength, '0');

	SimpleSerial serial;
	TcpClient socket = *new TcpClient(LOCALHOST, socketPort);

	void serialConnect() {
		COMport = SelectCOMport();

		do {
			serial.~SimpleSerial(); // Close previous connection

			serial = *new SimpleSerial((char*)COMport.c_str(), baudRate, "json");

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

#ifndef NOSOCKET
#ifndef PERFILE
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
#endif // !PERFILE
#endif // !NOSOCKET 
	}

	// Check for socket error codes here: https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

	void SendToSocket(std::string msg)
	{
		try {
			socket.Send(msg.c_str(), msg.length());
			//std::cout << "Socket wrt [" << msg << "]\n";
		}
		catch (SocketException& ex) {
			std::cout << "Socket wrt: error code " << ex.GetWSErrorCode() << "\n";
			socketReconnect();
		}
	}

	std::string ReceiveFromSocket()
	{
		std::string out = std::string(socketIndicatorsMessageLength * 2 + 2, '\0'); // Чтобы два сообщения влезли (защита от переполнения)

		try {
			socket.Recv((char*)out.c_str(), socketIndicatorsMessageLength * 2 + 2);
			out = out.substr(0, socketIndicatorsMessageLength);
			//std::cout << "Socket rec [" << out << "]\n";
		}
		catch (SocketException& ex) {
			std::cout << "Socket rec: error code " << ex.GetWSErrorCode() << "\n";

			socketReconnect();
		}

		return out;
	}

	std::string convertToBytes(std::string source)
	{
		static std::string output;

		if (source.size() > 8)
		{
			output.clear();

			output += source[0]; // speed
			output += source[1]; // battery voltage
			output += source[2]; // tm
			output += source[3]; // тm
			output += source[4]; // tс
			output += (unsigned char)(0 | (source[6] - '0') << 2 | (source[5] - '0') << 3); // lkvc and lsn

			unsigned char currentRegister = 0;
			short indexInByte = 0;

			for (short i = 7; i < source.size(); i++)
			{
				if (source[i] == '1')
					currentRegister |= 1 << indexInByte;

				if (indexInByte == 7) {
					output += currentRegister;
					currentRegister = 0;
					indexInByte = 0;
				}
				else
					indexInByte++;
			}
			output += currentRegister;
		}
		return output;
	}

	// this inverts bits !
	std::string convertFromBytes(std::string source)
	{
		static std::string output;

		if (!source.empty())
		{
			output.clear();

			for (short i = 0; i < source.size(); i++)
			{
				for (short k = 0; k < 8; k++)
					output += source[i] >> k & 1 ? '0' : '1';
			}
		}
		output = output.substr(0, socketSwitchesMessageLength);
		return output;
	}

public:

	PhysPult(std::string configPath) {
		if (!configPath.empty()) this->configPath = configPath;

		reloadConfig(this->configPath);

#ifndef PERFILE
		socketConnect();
#endif
#ifndef NOSERIAL
		serialConnect();
#endif
	};

	void reloadConfig(std::string configPath)
	{
		std::ifstream config(configPath);
		if (config.is_open()) {
			config >> socketPort >> serialFrequency >> socketFrequency >> baudRate >>
				serialIndicatorsMessageLength >> socketIndicatorsMessageLength >>
				serialSwitchesMessageLength >> socketSwitchesMessageLength; // Get values from config

			serialInterval = 1000 / serialFrequency;
			socketInterval = 1000 / socketFrequency;

			indicators = std::string(socketIndicatorsMessageLength, '0'),
				switches = std::string(socketSwitchesMessageLength, '0');
		}
		else std::cerr << "Couldn't open config file! Using previously set parameters.\n";

		std::string metrostroiDataPath;
		getline(config, metrostroiDataPath);
		getline(config, metrostroiDataPath);
		this->metrostroiDataPath = metrostroiDataPath;

		std::ofstream luaConfig(metrostroiDataPath + "\\physpult.txt");
		if (!config.is_open())
			std::cerr << "Couldn't save config file to data!\n";

		luaConfig << socketPort << " " << socketInterval << " " << socketIndicatorsMessageLength << " " << socketSwitchesMessageLength
			<< " \nDo not change these parameters! There are being updated automatically."; // Write values to lua config

		//std::cout << "Initialized!" << "\nPort: " << socketPort << "; serialFrequency: " << serialFrequency << "; Baud rate: " << baudRate
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
#ifndef NOSERIAL
		while (!stop)
		{
			auto beginTime = std::chrono::high_resolution_clock::now();
			//---
			if (!pause)
			{
				std::string indicators_t = '{' + convertToBytes(indicators) + '}';
				//std::cout << "Serial wrt " << indicators_t << ' ' << serial.WriteSerialPort(indicators_t) << "\n";
				serial.WriteSerialPort(indicators_t);

				std::string switches_t = serial.ReadSerialPort(10);
				//std::cout << "Serial rec {" << switches_t << "}\n";

				switches = convertFromBytes(switches_t);
				std::cout << "Serial rec {" << switches << "}\n";
				//if (switches_t.length() == serialSwitchesMessageLength) switches = switches_t;
			}
			//---
			int deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
			if (deltaTime < serialInterval) std::this_thread::sleep_for(std::chrono::milliseconds(serialInterval - deltaTime));
		}
#endif
	}

	void updateSocket(bool& stop, bool& pause)
	{
#ifndef NOSOCKET
		while (!stop)
		{
			auto beginTime = std::chrono::high_resolution_clock::now();
			//---
			if (!pause) {
				std::string indicators_t;
#ifdef  PERFILE
				std::ifstream file1(metrostroiDataPath + "\\physpult_indicators.txt");
				if (file1.is_open()) {
					file1 >> indicators_t;
					//std::cout << "fil rec [" << indicators_t << "]\n";
				}
				else std::cout << "fil rec failed\n";
				
				/*if (indicators_t.length() == socketIndicatorsMessageLength + 1)*/ indicators = indicators_t;

				std::ofstream file2(metrostroiDataPath + "\\physpult_switches.txt");
				if (file2.is_open()) {
					file2 << switches;
					//std::cout << "fil wrt [" << switches << "]\n";
				}
				else {
					std::cout << "fil wrt failed\n";
				}
#else
				indicators_t = ReceiveFromSocket();
				/*if (indicators_t.length() == socketIndicatorsMessageLength + 1)*/
				indicators = indicators_t.substr(0, socketIndicatorsMessageLength);

				SendToSocket(switches + '\0');
#endif //  PERFILE
			}
			//---
			int deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
			if (deltaTime < socketInterval) std::this_thread::sleep_for(std::chrono::milliseconds(socketInterval - deltaTime));
		}
#endif //  NOSOCKET
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
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // serialInterval for stop check is 100 ms, but could be anything else. Needed for optimisation

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
#ifndef NOSERIAL
				physpult.serialReconnect();
#endif
#ifndef NOSOCKET
#ifndef PERFILE
				physpult.socketReconnect();
#endif
#endif
				pause = 0;
			}
			if (stop) break; // Break from while(true) loop
		}
	}
}