#include <string>
#include <iostream>
#include <fstream>
#include "include/SimpleSerial.h"
#include "include/TcpSocket.cpp"

#pragma once

using namespace std;

string updateIndicators(TcpClient& client, short INDICATORSCOUNT);
void updateSwitches(TcpClient& client, string& switches);
void updateControls(SimpleSerial& Serial, TcpClient& client, short INDICATORSCOUNT);