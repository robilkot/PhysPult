#define WIN32_LEAN_AND_MEAN

#pragma once

#include <list>
#include <string>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

constexpr const char* LOCALHOST = "127.0.0.1";

#define DEBUG

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
        indicatorsCount = 64,
        switchesCount = 64;
    DWORD baudRate = 9600;

    std::string configPath = "physpult_config.txt";
    std::string COMport = "\\\\.\\COM1";

    void serialConnect() {
        COMport = SelectCOMport();

        do {
            serial = SimpleSerial(&COMport[0], baudRate, "json");

            if (serial.connected_) break;
            else {
                std::cerr << "Failed to connect to COM port! Press 'q' to exit or any other key to select another COM port.\n";
                switch (_getch()) {
                case 'q': exit(EXIT_SUCCESS);
                default: COMport = SelectCOMport()[0];
                }
            }
        } while (true);

        serialConnected = 1;
    }

    void socketConnect() {
        socket = TcpClient(LOCALHOST, socketPort);

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

        socketConnected = 1;
    }

public:
    std::string indicators = std::string(indicatorsCount, '0'),
        switches = std::string(switchesCount, '0');

    int interval = 100;

    SimpleSerial serial;
    TcpClient socket = TcpClient(LOCALHOST, socketPort);

    bool serialConnected = 0,
        socketConnected = 0;


    PhysPult(std::string path) {
        if (!path.empty()) configPath = path;

        reloadConfig(configPath);
        interval = 1000 / frequency;
        indicators = std::string(indicatorsCount, '0'),
        switches = std::string(switchesCount, '0');

        if (!socketConnected) socketConnect();
        if (!serialConnected) serialConnect();
    };

    void reloadConfig(std::string configPath)
    {
        std::ifstream config(configPath);
        if (config.is_open()) {
            config >> socketPort >> frequency >> baudRate >> indicatorsCount >> switchesCount; // Get values from config
        }
        else {
            std::cerr << "Couldn't open config file! Using default parameters.\n";
        }

        std::string metrostroiDataPath;
        getline(config, metrostroiDataPath);
        getline(config, metrostroiDataPath);

        std::ofstream luaConfig(metrostroiDataPath + "\\physpult.txt");
        if (!config.is_open())
            std::cerr << "Couldn't save config file to data!\n";

        luaConfig << socketPort << " " << frequency << " " << indicatorsCount << " " << switchesCount; // Write values to lua config

        std::cout << "Initialized!" << "\nPort: " << socketPort << "; Frequency: " << frequency << "; Baud rate: " << baudRate
            << "; Indicators: " << indicatorsCount << "; Switches: " << switchesCount << "\n\n";
    }

    void serialReconnect() {
        serialConnected = 0;
        serial = SimpleSerial(&COMport[0], baudRate, "json");

        if (!serial.connected_) {
            std::cerr << "Failed to reconnect to COM port! Select another COM port.\n";
            serialConnect();
        }
        else serialConnected = 1;
    }

    void socketReconnect() {
        socketConnected = 0;

        socketConnect();
    }
};

// Check for socket error codes here: https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

void SendToSocket(PhysPult& physpult, std::string msg)
{
    try {
        physpult.socket.Send(&msg[0], msg.length());
        std::cout << "Socket wrt [" << msg << "]\n";
    }
    catch (SocketException& ex) {
        std::cout << "Socket wrt: error code " << ex.GetWSErrorCode() << "\n";

        physpult.socketReconnect();
    }
}

std::string ReceiveFromSocket(PhysPult& physpult)
{
    size_t length = physpult.indicators.length() + 1;

    std::string out;
    out.reserve(length);

    try {
        physpult.socket.Recv(&out[0], length);
        std::cout << "Socket rec [" << out << "]\n";
    }
    catch (SocketException& ex) {
        std::cout << "Socket rec: error code " << ex.GetWSErrorCode() << "\n";
        out = std::string(length - 1, '0');

        physpult.socketReconnect();
    }

    return out;
}

// Update functions

void updateSerial(PhysPult& physpult, bool& stop, bool& pause)
{
    while (!stop)
    {
        auto beginTime = std::chrono::high_resolution_clock::now();
        //---
        if (!pause)
        {
            std::string indicators_t = '{' + physpult.indicators + '}';
            std::cout << "Serial wrt " << indicators_t << " " << physpult.serial.WriteSerialPort(indicators_t) << "\n";

            std::string switches_t = physpult.serial.ReadSerialPort(35);
            std::cout << "Serial rec {" << switches_t << "}\n";

            if (switches_t.length()) physpult.switches = switches_t;
        }
        //---
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
        if (deltaTime < physpult.interval) std::this_thread::sleep_for(std::chrono::milliseconds(physpult.interval - deltaTime));
    }
}

void updateSocket(PhysPult& physpult, bool& stop, bool& pause)
{
    while (!stop)
    {
        auto beginTime = std::chrono::high_resolution_clock::now();
        //---
        if (!pause) {

            std::string indicators_t = ReceiveFromSocket(physpult);

            if (indicators_t.length() == physpult.indicators.length()) physpult.indicators = indicators_t;

            SendToSocket(physpult, physpult.switches + '\0');
        }
        //---
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginTime).count();
        if (deltaTime < physpult.interval) std::this_thread::sleep_for(std::chrono::milliseconds(physpult.interval - deltaTime));
    }
}



int main(int argc, char* argv[])
{   
    std::string arg = argv[1] == NULL ? "" : argv[1];
    PhysPult physpult(arg);

    std::cout << "Starting. Press 'space' to stop or 'q' to exit.\n\n";

    bool stop = 0, pause = 0, reload = 0;

    std::thread socketThread([&]() { updateSocket(physpult, stop, pause); }),
                serialThread([&]() { updateSerial(physpult, stop, pause); });
    
    socketThread.detach();
    serialThread.detach();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Interval for stop check is 100 ms, but could be anything else. Needed for optimisation

        if (_kbhit()) {
            switch (_getch()) {
            case ' ': {
                pause = 1;
                std::cout << "Paused! Press any key to continue, 'r' to reload config or 'q' to exit.\n";

                switch (_getch()) { // While paused
                case 'r': {
                    reload = 1;
                    break;
                }
                case 'q': {
                    stop = 1;
                    break;
                }
                default: {
                    pause = 0;
                    break;
                }
                }
                break;
            }
            case 'r': {
                reload = 1;
                break;
            }
            case 'q': {
                stop = 1;
                break;
            }
            }

            if (reload) {
                pause = 1;
                physpult = PhysPult(arg);
                pause = 0;
            }
            if (stop) break; // Break from while(true) loop
        } 
    }
}