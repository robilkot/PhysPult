#define WIN32_LEAN_AND_MEAN

#include "process.h"

using namespace std;

//void updateControls(SimpleSerial& serial, TcpClient& client, short indicatorsCount, short switchesCount)
//{
//    string indicators = ReceiveFromSocket(client, indicatorsCount + 1);
//
//    if (!indicators.empty()) {
//       indicators = "{" + indicators + "}";
//       cout << "Serial wrt " << indicators << " " << serial.WriteSerialPort(&indicators[0]) << "\n";
//    }
//
//    string switches = serial.ReadSerialPort(35);
//
//    cout << "Serial rec {" << switches << "}\n";
//
//    if (!switches.empty()) SendToSocket(client, switches + '\0');
//        else SendToSocket(client, string(switchesCount, '0') + '\0');
//
//    cout << "\n";
//}

void updateSerial(SimpleSerial& serial, string& indicators, string& switches, int interval, bool& stop)
{
    while (!stop)
    {
        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        string indicators_t = '{' + indicators + '}';
        cout << "Serial wrt " << indicators_t << " " << serial.WriteSerialPort(&indicators_t[0]) << "\n";

        string switches_t = serial.ReadSerialPort(35);
        cout << "Serial rec {" << switches_t << "}\n";

        if (switches_t.size()) switches = switches_t;

        //---
        int us = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < interval) this_thread::sleep_for(chrono::milliseconds(interval - us));
    }
}

void updateSocket(TcpClient& client, string& indicators, string& switches, int interval, bool& stop)
{
    while (!stop)
    {
        chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
        //---

        string indicators_t = ReceiveFromSocket(client, indicators.length());

        if (indicators_t.length() == indicators.length()) indicators = indicators_t;

        SendToSocket(client, switches + '\0');

        //---
        int us = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count();
        if (us < interval) this_thread::sleep_for(chrono::milliseconds(interval - us));
    }
}