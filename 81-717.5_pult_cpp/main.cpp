#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>
#include "indicators_process.h"
#include "interface.h"
#include "include/SimpleSerial.h"

using namespace std;

enum {
    FREQ_HZ = 3,
    INV_FREQ_HZ = 1000000 / FREQ_HZ
};

int main()
{   
    //--- FILES INITIALISING ---

    ifstream inconfig("81-717.5_pult_cpp_config.txt");
    if (!inconfig.is_open()) {
        cerr << "Couldnt open config file!\n";
        system("pause");
        return 0;
    }
    
    string PATH;
    getline(inconfig, PATH);
    ifstream infile(PATH);
    if (!infile.is_open()) {
        cerr << "Couldnt open file!\n";
        system("pause");
        return 0;
    }
    string previous;
    getline(infile, previous); // initialize string with previous state

    //--- COM PORT INITIALISING ---

    list<int> COMports = GetCOMports();
    selectport:
    cout << "Input port number\n";

    char com_port[] = "\\\\.\\COM4";
    com_port[7] = _getch();
    DWORD COM_BAUD_RATE = CBR_9600;
    SimpleSerial Serial(com_port, COM_BAUD_RATE);

    if (!Serial.connected_) {
        cout << "Failed to connect! Try again\n\n";
        goto selectport;
    }

    //--- MAIN BODY ---

    cout << "To start press any key. To pause press 2\n";
    system("pause");

    char c = 0;
    while (c != 13)
    {
        c = 0;
        if (_kbhit()) c = _getch();
        if (c == '2') {
            cout << "Paused!\n";
            system("pause");
            continue;
        }

        using namespace chrono;
        high_resolution_clock::time_point t = high_resolution_clock::now();
        //---

        string changedstate = indicators_change(PATH, previous); 
        if(!changedstate.empty())
        {
            char* to_send = new char[changedstate.length() + 1];
            for (int i = 0; i < changedstate.length(); i++) to_send[i] = changedstate[i];
            to_send[changedstate.length()] = '\0'; // Forming char* to send to serial

            cout << "Sending to serial: " << to_send << "\n";
            if (!Serial.WriteSerialPort(to_send)) cerr << "Error writing smth to port!\n";

            delete[] to_send;
        }
        //---
        int us = duration_cast<microseconds>(high_resolution_clock::now() - t).count();
        if (us < INV_FREQ_HZ) {
            this_thread::sleep_for(microseconds(INV_FREQ_HZ - us));
        }
    }

    infile.close();
    return 0;
}