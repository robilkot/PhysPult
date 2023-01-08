#define WIN32_LEAN_AND_MEAN

#include <string>
#include <iostream>
#include <fstream>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

using namespace std;

string updateIndicators(TcpClient& client, short INDICATORSCOUNT)
{
    static string indicatorsprevious;
    string indicators = ReceiveFromSocket(client, INDICATORSCOUNT+1);

    if (indicatorsprevious == indicators) return {};

    indicatorsprevious = indicators;
    return indicators;
}

void updateSwitches(TcpClient& client, string& switches)
{
    static string switchesprevious;

    if (switchesprevious == switches) return;

    SendToSocket(client, switches);

    switchesprevious = switches;
}

void updateControls(SimpleSerial& Serial, TcpClient& client, short INDICATORSCOUNT)
{
    string indicators = "{" + updateIndicators(client, INDICATORSCOUNT) + "}";
    if (indicators.size() > 2) {
       cout << "wrt " << indicators << Serial.WriteSerialPort(&indicators[0]) << "\n";
        Serial.WriteSerialPort(&indicators[0]);
    }

    string switches = Serial.ReadSerialPort(1, "json") + "\0";
    if (!switches.empty()) {
        cout << "rec {" << switches << "}\n";
        updateSwitches(client, switches);
    }
}
