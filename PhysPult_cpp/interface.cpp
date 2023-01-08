#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <list>
#include <conio.h>
#include <Windows.h>

#include "include/TcpSocket.cpp"

using namespace std;

list<int> GetCOMports()
{
    wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
    list<int> portList;

    cout << "Available ports:\n";
    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        wstring str = L"COM" + to_wstring(i); // converting to COM0, COM1, COM2
        bool res = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        if (res) //QueryDosDevice returns zero if it didn't find an object
        {
            portList.push_back(i);
            string temp(str.begin(),str.end());
            cout << temp << "\n"; //" - " << lpTargetPath << endl;
        }
        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) NULL;
    }
    return portList;
}

string SelectCOMport() {
    string com_port = "\\\\.\\COM";
    do {
        list<int> COMports = GetCOMports();
        if (COMports.empty()) {
            cerr << "No COM ports available! Press q to exit or any other key to retry.\n\n";
            if (_getch() == 'q') exit(EXIT_FAILURE);
            continue;
        }
        else if (COMports.size() == 1) {
            cout << "Only one COM port found. Using it as output.\n\n";
            com_port += to_string(*COMports.begin());
            return com_port;
        }
        else {
            cout << "Input COM port number\n";
            com_port += _getch();
            return com_port;
        }
    } while (true);
}

void SendToSocket(TcpClient& client, string& msg)
{
    try {
        client.Send(&msg[0], msg.length()+1);
    }
    catch (SocketException& ex) {
        cout << "Error sending to socket: error code " << ex.GetWSErrorCode() << "\n";
    }
}

string ReceiveFromSocket(TcpClient& client, short length)
{
    try {
        char* msg = new char[length];

        client.Recv(msg, length);

        string out(msg);
        delete[] msg;
        return out;
    }
    catch (SocketException& ex) {
        cout << "Error receiving from socket: error code " << ex.GetWSErrorCode() << "\n";
        return {};
    }
}