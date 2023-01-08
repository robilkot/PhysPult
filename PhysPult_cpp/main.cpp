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

void endProgram(SimpleSerial& Serial) {
    TcpSocket::Dispose();

    if (Serial.CloseSerialPort()) {
        exit(EXIT_SUCCESS);
        cout << "Closed connection to COM port\n";
    }
    else {
        cerr << "Failed to close connection to COM port\n";
        exit(EXIT_FAILURE);
    }
}

// Initializing based on config file
void init(string& configPath, int& PORT, int& FREQ_HZ, DWORD& COM_BAUD_RATE, short& INDICATORSCOUNT, short& SWITCHESCOUNT) {
    ifstream config(configPath);
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }

    config >> PORT >> FREQ_HZ >> COM_BAUD_RATE >> INDICATORSCOUNT >> SWITCHESCOUNT;

    cout << "Initialized succesfully!"
    << "\nPort for data exchange: " << PORT
    << "\nFrequency: " << FREQ_HZ << "\nBaud rate: " << COM_BAUD_RATE
    << "\nIndicators count: " << INDICATORSCOUNT << "\nSwitches count: " << SWITCHESCOUNT
    << "\n\n";
}

int main(int argc, char* argv[])
{   
    //--- INITIALISING ---

    string configPath = "physpult_config.txt";
    int PORT = 61000, FREQ_HZ = 10;
    short INDICATORSCOUNT = 64, SWITCHESCOUNT = 64;
    DWORD COM_BAUD_RATE = 9600; // Set default values for all variables
    if (argv[1] != NULL) configPath = argv[1];

    Init:

    system("CLS 2> nul");
    init(configPath, PORT, FREQ_HZ, COM_BAUD_RATE, INDICATORSCOUNT, SWITCHESCOUNT); // Initialize based on config file

    InitCOMPort:

    SimpleSerial Serial(&SelectCOMport()[0], COM_BAUD_RATE); // Create Serial object

    InitSocket:

    TcpSocket::Initialize();
    TcpClient client = *new TcpClient(LOCALHOST, PORT); // Create socket client

    try {
        client.Connect();
    }
    catch (SocketException& ex) {
        cout << "Failed to connect to socket: error code " << ex.GetWSErrorCode() << "\n"
            << "Press 'q' to exit, '2' to reload config or any other key to retry.\n";
        switch (_getch()) {
        case 'q': {
            endProgram(Serial);
        }
        case '2': {
            TcpSocket::Dispose();
            goto Init;
        }
        default: goto InitSocket;
        }
    }

    do {
        if (!Serial.connected_) {
            cout << "Failed to connect to COM port! Press 'q' to exit, '2' to select another COM port or any other key to retry.\n";
            switch (_getch()) {
            case 'q': endProgram(Serial);
            case '2': goto InitCOMPort;
            default: continue;
            }
        }
        else cout << "Connected to COM port.\n";

       
        break;
    } while (true);
   
    //--- BODY ---

    cout << "Starting. Press '2' to pause, 'r' to reload config or 'q' to exit.\n\n";
    system("timeout 1 > nul 2> nul");

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
                case 'q': endProgram(Serial);
                }
                continue;
            }
            case 'r': goto Init;
            case 'q': endProgram(Serial);
            }
        }

        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        updateControls(Serial, client, SWITCHESCOUNT);

        //---
        int us = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < 1000000/FREQ_HZ) this_thread::sleep_for(chrono::microseconds(1000000 / FREQ_HZ - us));
    }

    endProgram(Serial);
}