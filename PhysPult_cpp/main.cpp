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

void endProgram(SimpleSerial& serial) {
    TcpSocket::Dispose();

    if (serial.CloseSerialPort()) {
        exit(EXIT_SUCCESS);
        cout << "Closed connection to COM port\n";
    }
    else {
        cerr << "Failed to close connection to COM port\n";
        exit(EXIT_FAILURE);
    }
}

// Initializing based on config file
void init(string& configPath, int& socketPort, short& frequency, DWORD& baudRate, short& indicatorsCount, short& switchesCount) {
    ifstream config(configPath);
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }

    config >> socketPort >> frequency >> baudRate >> indicatorsCount >> switchesCount;

    cout << "Initialized!"
    << "\nPort: " << socketPort
    << "; Frequency: " << frequency << "; Baud rate: " << baudRate
    << "; Indicators: " << indicatorsCount << "; Switches: " << switchesCount
    << "\n\n";
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
            << "Press 'q' to exit, '2' to reload config or any other key to retry.\n";
        switch (_getch()) {
        case 'q': endProgram(serial);
        case '2': {
            TcpSocket::Dispose();
            goto Init;
        }
        default: goto InitSocket;
        }
    }

    do {
        if (!serial.connected_) {
            cout << "Failed to connect to COM port! Press 'q' to exit, '2' to select another COM port or any other key to retry.\n";
            switch (_getch()) {
            case 'q': endProgram(serial);
            case '2': goto InitCOMPort;
            default: continue;
            }
        }
        else break;
    } while (true);
   
    //--- BODY ---

    cout << "Starting. Press '2' to pause, 'r' to reload config or 'q' to exit.\n\n";

    char c = 0;
    short linenumber = 0;
    while (c != 13)
    {
        if (linenumber > 30) {
            system("cls");
            linenumber = 0;
        } else linenumber++;
        c = 0;
        if (_kbhit()) {
            switch (_getch()) {
            case '2': {
                cout << "Paused! Press any key to continue or 'r' to reload config or 'q' to exit.\n";
                switch (_getch()) {
                case 'r': goto Init;
                case 'q': endProgram(serial);
                }

                TcpSocket::Dispose();
                goto InitSocket;
                //continue;
            }
            case 'r': goto Init;
            case 'q': endProgram(serial);
            }
        }

        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        updateControls(serial, client, indicatorsCount, switchesCount);

        //---
        int us = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < 1000000 / frequency) this_thread::sleep_for(chrono::microseconds(1000000 / frequency - us));
    }

    endProgram(serial);
}