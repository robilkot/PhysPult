#include <string>
#include <iostream>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

#pragma once

using namespace std;

void updateControls(SimpleSerial&, TcpClient&, short indicatorsCount, short switchesCount);
void pingArduino(SimpleSerial&, TcpClient&, short switchesCount);