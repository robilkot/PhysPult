#include <string>
#include <iostream>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

#pragma once

//void updateControls(SimpleSerial&, TcpClient&, short indicatorsCount, short switchesCount);

void updateSocket(TcpClient& socket, std::string& indicators, std::string& switches, int interval, bool& stop);
void updateSerial(SimpleSerial& serial, std::string& indicators, std::string& switches, int interval, bool& stop);

//void pingArduino(SimpleSerial&, TcpClient&, short switchesCount);