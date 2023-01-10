#define WIN32_LEAN_AND_MEAN

#include <string>
#include <iostream>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

//#define DEBUG_SERIAL

using namespace std;

string updateIndicators(TcpClient& client, short indicatorsCount)
{
    static string indicatorsprevious;
    string indicators = ReceiveFromSocket(client, indicatorsCount + 1);

    if (indicatorsprevious == indicators) return {};

    indicatorsprevious = indicators;
    return indicators;
}

void updateSwitches(TcpClient& client, string switches)
{
    static string switchesprevious;

    if (switchesprevious == switches) return;

    SendToSocket(client, switches + "\0");

    switchesprevious = switches;
}

void updateControls(SimpleSerial& Serial, TcpClient& client, short indicatorsCount, short switchesCount)
{
    string switches = Serial.ReadSerialPort(1, "json");
    if (!switches.empty()) {
#ifdef DEBUG_SERIAL
        cout << "Serial rec {" << switches << "}\n";
#endif
        updateSwitches(client, switches);
    }
    else updateSwitches(client, string(switchesCount, '0'));

    string indicators = "{" + updateIndicators(client, indicatorsCount) + "}";
    if (indicators.size() > 2) {
#ifdef DEBUG_SERIAL
       cout << "Seial wrt " << indicators << Serial.WriteSerialPort(&indicators[0]) << "\n";
#else
       Serial.WriteSerialPort(&indicators[0]);
#endif
    }
}
