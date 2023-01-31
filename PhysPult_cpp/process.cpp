#define WIN32_LEAN_AND_MEAN

#include "process.h"

//#define DEBUG_SERIAL

using namespace std;

void updateControls(SimpleSerial& serial, TcpClient& client, short indicatorsCount, short switchesCount)
{
    string indicators = ReceiveFromSocket(client, indicatorsCount + 1);

    if (!indicators.empty()) {
       indicators = "{" + indicators + "}";
       cout << "Serial wrt " << indicators << " " << serial.WriteSerialPort(&indicators[0]) << "\n";
    }

    string switches = serial.ReadSerialPort(35);

    cout << "Serial rec {" << switches << "}\n";

    if (!switches.empty()) SendToSocket(client, switches + '\0');
        else SendToSocket(client, string(switchesCount, '0') + '\0');

    cout << "\n";
}

void pingArduino(SimpleSerial& serial, TcpClient& client, short switchesCount) {

    cout << "Serial wrt " << "{ping}" << " " << serial.WriteSerialPort((char*)"{ping}") << "\n";

    string switches = serial.ReadSerialPort(35);

    cout << "Serial rec {" << switches << "} (ping answer)\n";

    //if (!switches.empty()) SendToSocket(client, switches + '\0');
   // else SendToSocket(client, string(switchesCount, '0') + '\0');

    cout << "\n";
}