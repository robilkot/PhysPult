#define WIN32_LEAN_AND_MEAN

#include "process.h"

//#define DEBUG_SERIAL

using namespace std;

string updateIndicators(TcpClient& client, short indicatorsCount)
{
    static string indicatorsprevious;
    string indicators = ReceiveFromSocket(client, indicatorsCount + 1);

    //if (indicatorsprevious == indicators) return {};

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
    string indicators = "{" + updateIndicators(client, indicatorsCount) + "}";
    if (indicators.size() > 2) {
       cout << "Serial wrt " << indicators << Serial.WriteSerialPort(&indicators[0]) << "\n";
    }

    this_thread::sleep_for(chrono::milliseconds(15));

    string switches = Serial.ReadSerialPort(1, "json");

    cout << "Serial rec {" << switches << "}\n";

    if (!switches.empty()) {
        updateSwitches(client, switches);
    }
    else updateSwitches(client, string(switchesCount, '0'));
}
