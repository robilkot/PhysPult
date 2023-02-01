#include <list>
#include <iostream>
#include <string>
#include <conio.h>
#include <Windows.h>

#include "include/TcpSocket.cpp"

#pragma once

std::list<int> GetCOMports();
std::string SelectCOMport();
void SendToSocket(TcpClient&, std::string msg);
std::string ReceiveFromSocket(TcpClient&, short length);