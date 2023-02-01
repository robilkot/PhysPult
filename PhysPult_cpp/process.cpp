#define WIN32_LEAN_AND_MEAN

#include "process.h"

using namespace std;

void updateSerial(SimpleSerial& serial, std::string& indicators, std::string& switches, int interval, bool& stop)
{
    while (!stop)
    {
        std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
        //---

        std::string indicators_t = '{' + indicators + '}';
        std::cout << "Serial wrt " << indicators_t << " " << serial.WriteSerialPort(&indicators_t[0]) << "\n";

        std::string switches_t = serial.ReadSerialPort(35);
        std::cout << "Serial rec {" << switches_t << "}\n";

        if (switches_t.size()) switches = switches_t;

        //---
        int us = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count();
        if (us < interval) this_thread::sleep_for(std::chrono::milliseconds(interval - us));
    }
}

void updateSocket(TcpClient& socket, std::string& indicators, std::string& switches, int interval, bool& stop)
{
    while (!stop)
    {
        std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
        //---

        std::string indicators_t = ReceiveFromSocket(socket, indicators.length());

        if (indicators_t.length() == indicators.length()) indicators = indicators_t;

        SendToSocket(socket, switches + '\0');

        //---
        int us = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count();
        if (us < interval) this_thread::sleep_for(std::chrono::milliseconds(interval - us));
    }
}