#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>

#include "process.h"
#include "interface.h"
#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

//#define NOCONNECTION

constexpr const char* LOCALHOST = "127.0.0.1";

void init(string configPath, int& socketPort, short& frequency, DWORD& baudRate, short& indicatorsCount, short& switchesCount) {
    ifstream config(configPath);
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        return;
    }

    config >> socketPort >> frequency >> baudRate >> indicatorsCount >> switchesCount; // Get values from config

    string metrostroiDataPath;
    getline(config, metrostroiDataPath);
    getline(config, metrostroiDataPath);
    
    ofstream dataConfig(metrostroiDataPath + "\\physpult.txt");
    if (!config.is_open())
        cerr << "Couldn't save config file to data!\n";

    dataConfig << socketPort << " " << frequency << " " << indicatorsCount << " " << switchesCount; // Write values to config in gmod/data

    if (frequency > 60) cout << "High frequency is set (>60 Hz). Are you sure you need this much?\n";

    cout << "Initialized!" << "\nPort: " << socketPort << "; Frequency: " << frequency << "; Baud rate: " << baudRate
    << "; Indicators: " << indicatorsCount << "; Switches: " << switchesCount << "\n\n";
}

int main(int argc, char* argv[])
{   
    Init:
    string configPath = argv[1] == NULL ? "physpult_config.txt" : argv[1];
    int socketPort = 61000;
    short frequency = 10, indicatorsCount = 64, switchesCount = 64;
    DWORD baudRate = 9600; // Set default values for all variables

    system("CLS 2> nul");
    init(configPath, socketPort, frequency, baudRate, indicatorsCount, switchesCount); // Initialize based on config file
    short interval = 1000 / frequency; // in milliseconds

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

    //--- BODY ---

    for (int i = 0; i < 5; i++) { // Это проклято.
        this_thread::sleep_for(chrono::milliseconds(50));
        serial.WriteSerialPort((char*)"{wake up, neo!}");
    }

    Start:
    cout << "Starting. Press 'space' to pause or 'q' to exit.\n\n";

    string indicators = string(indicatorsCount, '0'),
             switches = string(switchesCount, '0');
    bool pause = 0;

    thread socketThread(updateSocket, ref(client), ref(indicators), ref(switches), interval, ref(pause)),
            serialThread(updateSerial, ref(serial), ref(indicators), ref(switches), interval, ref(pause));

    while (true)
    {
        if (_kbhit()) {
            switch (_getch()) {
            case ' ': {
                pause = 1;
                socketThread.join();
                serialThread.join();
                cout << "Paused! Press any key to continue, 'r' to reload config or 'q' to exit.\n";

                switch (_getch()) {
                case 'r': goto Init;
                case 'q': exit(EXIT_SUCCESS);
                default: goto InitSocket;
                }
            }
            //case 'r': goto Init; // Нужно убивать поток перед гото
            case 'q': exit(EXIT_SUCCESS);
            }
        }
    }
}