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

#define LOCALHOST "127.0.0.1"

using namespace std;

// Initializing based on config file
void init(string configPath, int& socketPort, short& frequency, DWORD& baudRate, short& indicatorsCount, short& switchesCount) {
    ifstream config(configPath);
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }

    config >> socketPort >> frequency >> baudRate >> indicatorsCount >> switchesCount;

    if (frequency > 60) cout << "High frequency is set (>60 Hz). Are you sure you need this much?\n";

    cout << "Initialized!" << "\nPort: " << socketPort << "; Frequency: " << frequency << "; Baud rate: " << baudRate
    << "; Indicators: " << indicatorsCount << "; Switches: " << switchesCount << "\n\n";
}

int main(int argc, char* argv[])
{   
    string configPath = "physpult_config.txt";
    int socketPort = 61000;
    short frequency = 10, indicatorsCount = 64, switchesCount = 64;
    DWORD baudRate = 9600; // Set default values for all variables
    if (argv[1] != NULL) configPath = argv[1];

    Init:

    system("CLS 2> nul");
    init(configPath, socketPort, frequency, baudRate, indicatorsCount, switchesCount); // Initialize based on config file

    InitCOMPort:

    SimpleSerial serial(&SelectCOMport()[0], baudRate); // Create serial object

    InitSocket:

    TcpSocket::Initialize();
    TcpClient client = *new TcpClient(LOCALHOST, socketPort); // Create socket client

    try {
        client.Connect();
    }
    catch (SocketException& ex) {
        cout << "Failed to connect to socket: error code " << ex.GetWSErrorCode() << "\n"
            << "Press 'q' to exit, 'r' to reload config or any other key to retry.\n";
        switch (_getch()) {
        case 'q': exit(EXIT_SUCCESS);
        case 'r': {
            TcpSocket::Dispose();
            goto Init;
        }
        default: goto InitSocket;
        }
    }

    do {
        if (serial.connected_) break;
        else {
            cout << "Failed to connect to COM port! Press 'q' to exit, '2' to select another COM port or any other key to retry.\n";
            switch (_getch()) {
            case 'q': exit(EXIT_SUCCESS);
            case '2': goto InitCOMPort;
            default: continue;
            }
        }
    } while (true);
   
    //--- BODY ---

    for (int i = 0; i < 5; i++) { // Это проклято.
        this_thread::sleep_for(chrono::milliseconds(50));
        serial.WriteSerialPort(&string("wake up, neo!\0")[0]);
    }

    cout << "Starting. Press '2' to pause, 'r' to reload config or 'q' to exit.\n\n";

    while (true)
    {
        if (_kbhit()) {
            switch (_getch()) {
            case '2': {
                cout << "Paused! Press any key to continue or 'r' to reload config or 'q' to exit.\n";
                switch (_getch()) {
                case 'r': goto Init;
                case 'q': exit(EXIT_SUCCESS);
                }

                client.~client();
                TcpSocket::Dispose();
                goto InitSocket;
            }
            case 'r': goto Init;
            case 'q': exit(EXIT_SUCCESS);
            }
        }

        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        updateControls(serial, client, indicatorsCount, switchesCount);

        //---
        int us = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < 1000 / frequency) this_thread::sleep_for(chrono::milliseconds(1000 / frequency - us));
    }
}