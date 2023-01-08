#include <list>

#pragma once

using namespace std;

list<int> GetCOMports();
string SelectCOMport();
void SendToSocket(TcpClient& client, string& msg);
string ReceiveFromSocket(TcpClient& client, short length);