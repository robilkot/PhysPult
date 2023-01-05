#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>
#include "process.h"
#include "interface.h"
#include "include/SimpleSerial.h"

using namespace std;

void endProgram(SimpleSerial& Serial) {
    if (Serial.CloseSerialPort()) {
        exit(EXIT_SUCCESS);
        cout << "Closed connection to COM port\n";
    }
    else {
        exit(EXIT_FAILURE);
        cerr << "Failed to close connection to COM port\n";
    }
}

// Initializing based on config file
void init(string& configPath, string& indicatorsPath, string& switchesPath, short& FREQ_HZ, DWORD& COM_BAUD_RATE, short& TOTALINDICATORS, short& TOTALSWITCHES) {
    ifstream config(configPath);
    if (!config.is_open()) {
        cerr << "Couldn't open config file!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }
    getline(config, indicatorsPath); // set path to file with indicators state
    getline(config, switchesPath); // set path to file with switches state

    ifstream infile(indicatorsPath); // check if indicators file is valid
    if (!infile.is_open()) {
        cerr << "Couldn't open file with indicators state!\n";
        system("pause");
        exit(EXIT_FAILURE);
    }

    string temp;
    getline(config, temp);
    if(!temp.empty()) FREQ_HZ = stoi(temp); // set frequency
    if (FREQ_HZ > 60) cout << "High frequence is set (>60 Hz). Are you sure you need this much?\n";

    getline(config, temp); 
    if (!temp.empty()) COM_BAUD_RATE = stoi(temp); // set baud rate

    getline(config, temp);
    if (!temp.empty()) TOTALINDICATORS = stoi(temp); // set indicators count (string length)

    getline(config, temp);
    if (!temp.empty()) TOTALSWITCHES = stoi(temp); // set switches count (string length)

    cout << "Initialized succesfully!\nIndicators state file: " << indicatorsPath << "\nSwitches state file: " << switchesPath
    << "\nFrequency: " << FREQ_HZ << "\nBaud rate: " << COM_BAUD_RATE
    << "\nIndicators number: " << TOTALINDICATORS << "\nSwitches number: " << TOTALSWITCHES << "\n\n";
}

int main(int argc, char* argv[])
{   
    //--- INITIALISING ---

    string indicatorsPath = "lamps.txt", switchesPath = "switches.txt", configPath = "physpult_config.txt";
    short FREQ_HZ = 10, TOTALINDICATORS = 32, TOTALSWITCHES = 64;
    DWORD COM_BAUD_RATE = 9600; // Set default values for all variables
    if (argv[1] != NULL) configPath = argv[1];

    InitCOMPort:

    system("cls");
    init(configPath, indicatorsPath, switchesPath, FREQ_HZ, COM_BAUD_RATE, TOTALINDICATORS, TOTALSWITCHES); // Initialize based on config file
    string indicatorsPrevious(TOTALINDICATORS, '0'), switchesPrevious(TOTALSWITCHES, '0');

    SimpleSerial Serial(&SelectCOMport()[0], COM_BAUD_RATE);

    do {
        if (!Serial.connected_) {
            cout << "Failed to connect! Press 'q' to exit, '2' to select another COM port or any other key to retry.\n";
            switch (_getch()) {
            case 'q': exit(EXIT_FAILURE); break;
            case '2': goto InitCOMPort;
            default: continue;
            } 
        }
        break;
        /*else {
            bool init = 0;
            for (int i = 0; i < 10; i++) {
                if (init) break;

                cout << "wrt {PhysPultInit} " << Serial.WriteSerialPort((char*)"{PhysPultInit}") << "\n";
                string rec = Serial.ReadSerialPort(1, "json");
                cout << "rec " << rec << "\n";
                if (rec == "PhysPultInitOK") init = 1;
            }
            if (init) {
                cout << "Arduino succesfully connected.\n\n";
                break;
            }
            cerr << "Selected port is not Arduino set up for PhysPult. Press q to exit or any key to select other COM port.\n\n";
            switch (_getch()) {
                case 'q': endProgram(Serial); break;
            }
            goto InitCOMPort;
        }*/
    } while (true);

    //--- BODY ---

    cout << "Starting. Press '2' to pause or 'r' to reload config.\n\n";
    system("timeout 1 > nul");

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
                cout << "Paused! Press any key to continue or 'r' to reload config.\n";
                switch (_getch()) {
                case 'r': goto InitCOMPort;
                }
                continue;
            }
            case 'r': goto InitCOMPort;
            }
        }

        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        //string sent = "{" + to_string(linenumber) + (string)"}";
        //cout << "wrt " << sent << " " << Serial.WriteSerialPort(&sent[0]) << "\n";
        //cout << "rec {" << Serial.ReadSerialPort(1, "json") << "}\n";

        updateControls(Serial, indicatorsPath, indicatorsPrevious, switchesPath, switchesPrevious);

        //---
        int us = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < 1000000/FREQ_HZ) this_thread::sleep_for(chrono::microseconds(1000000 / FREQ_HZ - us));
    }

    endProgram(Serial);
}