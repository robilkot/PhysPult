#include <string>
#include <iostream>
#include <thread>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"
#include "interface.h"

#pragma once

using namespace std;

string updateIndicators(TcpClient&, short indicatorsCount);
void updateSwitches(TcpClient&, string switches);
void updateControls(SimpleSerial&, TcpClient&, short indicatorsCount, short switchesCount);