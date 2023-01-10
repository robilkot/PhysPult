#include <string>
#include <iostream>

#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

#pragma once

using namespace std;

string updateIndicators(TcpClient& client, short indicatorsCount);
void updateSwitches(TcpClient& client, string switches);
void updateControls(SimpleSerial& Serial, TcpClient& client, short indicatorsCount, short switchesCount);