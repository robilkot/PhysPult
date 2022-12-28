#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>
#include "indicators_process.h"
#include "interface.h"
#include "include/SimpleSerial.h"

using namespace std;

// Инициализация программы по данным из конфиг-файла
void init(string& PATH, string& previous, short& FREQ_HZ, DWORD& COM_BAUD_RATE) {
    ifstream config("81-717.5_pult_cpp_config.txt");
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit;
    }
    getline(config, PATH); // get path to file with indicators state

    ifstream infile(PATH);
    if (!infile.is_open()) {
        cerr << "Couldn't open file with indicators state!\n";
        system("pause");
        exit;
    }
    getline(infile, previous); // create string with initial state

    string temp;
    getline(config, temp);
    FREQ_HZ = stoi(temp);
    if (FREQ_HZ > 60) cout << "High frequence is set (>60 Hz). Are you sure you need this much?\n";

    getline(config, temp); 
    COM_BAUD_RATE = stoi(temp); // set BAUD rate

    cout << "Initialized succesfully!\n" << "Path to indicators state file: " << PATH << "\nFrequency: " << FREQ_HZ << "\nBaud rate: " << COM_BAUD_RATE << "\n\n";

    infile.close();
    config.close();
}

int main()
{   
    //--- INITIALISING ---

    string PATH = "lamps.txt", previous;
    short FREQ_HZ = 10;
    DWORD COM_BAUD_RATE = 9600;
    init(PATH, previous, FREQ_HZ, COM_BAUD_RATE);

    //--- COM PORT INITIALISING ---

    selectport:
    list<int> COMports = GetCOMports();

    char com_port[] = "\\\\.\\COM4";
    if (COMports.empty()) {
        cerr << "No COM ports available! Press q to exit or any other key to retry.\n\n";
        if (_getch() == 'q') exit(EXIT_FAILURE);
        goto selectport;
    }
    if(COMports.size()==1) {
        cout << "Only one COM port found. Using it as output.\n\n";
        com_port[7] = (char)(*COMports.begin() + '0');
    }
    else {
        cout << "Input COM port number\n";
        com_port[7] = _getch();
    }
    SimpleSerial Serial(com_port, COM_BAUD_RATE);

    if (!Serial.connected_) {
        cout << "Failed to connect! Press q to exit or any other key to retry.\n\n";
        if (_getch() == 'q') exit(EXIT_FAILURE);
        goto selectport;
    }

    //--- BODY ---

    cout << "Press any key to start. Press 2 to pause.\n";
    system("pause");

    using namespace chrono;
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

        high_resolution_clock::time_point t = high_resolution_clock::now();
        //---

        string changedstate = indicators_change(PATH, previous); 
        if(!changedstate.empty())
        {
            char* to_send = new char[changedstate.length() + 1];
            for (int i = 0; i < changedstate.length(); i++) to_send[i] = changedstate[i];
            to_send[changedstate.length()] = '\0'; // Forming char* to send to serial

            cout << "Writing to serial: " << to_send << "\n";
            if (!Serial.WriteSerialPort(to_send)) cerr << "Error writing string to serial!\n";

            delete[] to_send;
        }

        //---
        int us = duration_cast<microseconds>(high_resolution_clock::now() - t).count();
        if (us < 1000000/FREQ_HZ) this_thread::sleep_for(microseconds(1000000 / FREQ_HZ - us));
    }

    return 0;
}