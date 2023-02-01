#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>

#include "process.h"
#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

#define NOCONNECTION

constexpr const char* LOCALHOST = "127.0.0.1";

void init(std::string configPath, int& socketPort, short& frequency, DWORD& baudRate, short& indicatorsCount, short& switchesCount)
{
    std::ifstream config(configPath);
    if (!config.is_open()) {
        std::cerr << "Couldn't open config file! Using old parameters.\n";
        return;
    }

    config >> socketPort >> frequency >> baudRate >> indicatorsCount >> switchesCount; // Get values from config

    std::string metrostroiDataPath;
    getline(config, metrostroiDataPath);
    getline(config, metrostroiDataPath);
    
    std::ofstream luaConfig(metrostroiDataPath + "\\physpult.txt");
    if (!config.is_open())
        std::cerr << "Couldn't save config file to data!\n";

    luaConfig << socketPort << " " << frequency << " " << indicatorsCount << " " << switchesCount; // Write values to lua config
}

int main(int argc, char* argv[])
{   
    Init:
    std::string configPath = argv[1] == NULL ? "physpult_config.txt" : argv[1];
    int socketPort = 61000;
    short frequency = 10, indicatorsCount = 64, switchesCount = 64;
    DWORD baudRate = 9600; // Set default values for all variables

    system("CLS 2> nul");
    init(configPath, socketPort, frequency, baudRate, indicatorsCount, switchesCount); // Initialize based on config file
    short interval = 1000 / frequency; // in milliseconds

    std::cout << "Initialized!" << "\nPort: " << socketPort << "; Frequency: " << frequency << "; Baud rate: " << baudRate
        << "; Indicators: " << indicatorsCount << "; Switches: " << switchesCount << "\n\n";


    InitCOMPort:
    SimpleSerial serial(&SelectCOMport()[0], baudRate, "json"); // Create serial object


    InitSocket:
    TcpSocket::Initialize();
    TcpClient client = *new TcpClient(LOCALHOST, socketPort); // Create socket client

#ifndef NOCONNECTION
    try {
        client.Connect();
    }
    catch (SocketException& ex) {
        cerr << "Failed to connect to socket: error code " << ex.GetWSErrorCode() << "\n"
            << "Press 'q' to exit, 'r' to reload config or any other key to retry.\n";
        switch (_getch()) {
        case 'q': exit(EXIT_SUCCESS);
        case 'r': goto Init;
        default: goto InitSocket;
        }
    }

    do {
        if (serial.connected_) break;
        else {
            cerr << "Failed to connect to COM port! Press 'q' to exit, '2' to select another COM port or any other key to retry.\n";
            switch (_getch()) {
            case 'q': exit(EXIT_SUCCESS);
            case '2': goto InitCOMPort;
            default: continue;
            }
        }
    } while (true);
#endif

    std::cout << "Starting. Press 'space' to pause or 'q' to exit.\n\n";

    std::string indicators = std::string(indicatorsCount, '0'),
             switches = std::string(switchesCount, '0');
    bool pause = 0;

    std::thread socketThread(updateSocket, std::ref(client), std::ref(indicators), std::ref(switches), interval, std::ref(pause)),
            serialThread(updateSerial, std::ref(serial), std::ref(indicators), std::ref(switches), interval, std::ref(pause));
    
    while (true)
    {
        std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

        if (_kbhit())
        {
            switch (_getch())
            {
            case ' ': {
                pause = 1;
                socketThread.join();
                serialThread.join();
                std::cout << "Paused! Press any key to continue, 'r' to reload config or 'q' to exit.\n";

                switch (_getch()) {
                case 'r': goto Init;
                case 'q': break;
                default: goto InitSocket;
                }
            }
            case 'r': {
                pause = 1;
                socketThread.join();
                serialThread.join();
                goto Init;
            }
            case 'q': break;
            }
        }

        int us = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count();
        if (us < 100) std::this_thread::sleep_for(std::chrono::milliseconds(100 - us)); // Interval for pause check is 100 ms, but could be anything else. Needed for optimisation
    }
}