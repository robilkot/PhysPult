#include <string>
#include <iostream>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

#pragma once

using namespace std;

//void updateControls(SimpleSerial&, TcpClient&, short indicatorsCount, short switchesCount);

void updateSocket(TcpClient& client, string& indicators, string& switches, int interval, bool& stop);
void updateSerial(SimpleSerial& serial, string& indicators, string& switches, int interval, bool& stop);

//void pingArduino(SimpleSerial&, TcpClient&, short switchesCount);