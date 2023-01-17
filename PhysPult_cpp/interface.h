#include <list>
#include <iostream>
#include <string>
#include <list>
#include <conio.h>
#include <Windows.h>

#include "include/TcpSocket.cpp"

#pragma once

using namespace std;

list<int> GetCOMports();
string SelectCOMport();
void SendToSocket(TcpClient&, string msg);
string ReceiveFromSocket(TcpClient&, short length);