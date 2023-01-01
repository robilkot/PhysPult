#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>
#include "process.h"
#include "interface.h"
#include "include/SimpleSerial.h"

#define TOTALINDICATORS 26
#define TOTALSWITCHES 64

using namespace std;

// Инициализация программы по данным из конфиг-файла
void init(string& indicators_PATH, string& switches_PATH, string& indicators_previous, short& FREQ_HZ, DWORD& COM_BAUD_RATE) {
    ifstream config("81-717.5_pult_cpp_config.txt");
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }
    getline(config, indicators_PATH); // get path to file with indicators state
    getline(config, switches_PATH); // get path to file with switches state

    ifstream infile(indicators_PATH);
    if (!infile.is_open()) {
        cerr << "Couldn't open file with indicators state!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }
    getline(infile, indicators_previous); // create string with initial state

    string temp;
    getline(config, temp);
    if(!temp.empty()) FREQ_HZ = stoi(temp); // set frequency
    if (FREQ_HZ > 60) cout << "High frequence is set (>60 Hz). Are you sure you need this much?\n";

    getline(config, temp); 
    if (!temp.empty()) COM_BAUD_RATE = stoi(temp); // set baud rate

    cout << "Initialized succesfully!\nIndicators state file: " << indicators_PATH << "\nSwitches state file: " << switches_PATH;
    cout << "\nFrequency: " << FREQ_HZ << "\nBaud rate: " << COM_BAUD_RATE << "\n\n";
}

int main()
{   
    //--- INITIALISING ---

    string indicators_PATH = "lamps.txt", switches_PATH = "switches.txt";
    string indicators_previous(TOTALINDICATORS, '0'), switches_previous(TOTALSWITCHES, '0');
    short FREQ_HZ = 10;
    DWORD COM_BAUD_RATE = 9600; // Set default values for all variables
    init(indicators_PATH, switches_PATH, indicators_previous, FREQ_HZ, COM_BAUD_RATE); // Initialize based on config file

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
    system("pause > nul");

    using namespace chrono;
    char c = 0;
    while (c != 13)
    {
        c = 0;
        if (_kbhit()) c = _getch();
        if (c == '2') {
            cout << "Paused! Press any key to continue.\n";
            system("pause > nul");
            continue;
        }

        high_resolution_clock::time_point t = high_resolution_clock::now();
        //---

        string changedstate = indicators_process(indicators_PATH, indicators_previous);
        if(!changedstate.empty())
        {
            char* to_send = &changedstate[0]; // Forming char* to send to serial
            cout << "Writing to serial: " << to_send << "\n";
            if (!Serial.WriteSerialPort(to_send)) cerr << "Error writing string to serial!\n";
        }
        
        switches_process(switches_PATH, Serial.ReadSerialPort(1, "json"), switches_previous);

        //---
        int us = duration_cast<microseconds>(high_resolution_clock::now() - t).count();
        if (us < 1000000/FREQ_HZ) this_thread::sleep_for(microseconds(1000000 / FREQ_HZ - us));
    }

    return 0;
}