#define WIN32_LEAN_AND_MEAN

#include "interface.h"

#define DEBUG_SOCKET

using namespace std;

list<int> GetCOMports()
{
	wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
	list<int> portList;

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		wstring str = L"COM" + to_wstring(i); // converting to COM0, COM1, COM2

		if (QueryDosDeviceW(str.c_str(), lpTargetPath, 5000)) //QueryDosDeviceW returns zero if it didn't find an object
		{
			portList.push_back(i);
			cout << string(str.begin(), str.end()) << "\n"; //" - " << lpTargetPath << endl;
		}
		//if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) NULL;
	}
	return portList;
}

string SelectCOMport()
{
	string com_port = "\\\\.\\COM";
	do {
		list<int> COMports = GetCOMports();
		if (COMports.empty()) {
			cerr << "No COM ports available! Press q to exit or any other key to retry.\n\n";
			if (_getch() == 'q') exit(EXIT_SUCCESS);
			continue;
		}
		else if (COMports.size() == 1) {
			com_port += to_string(*COMports.begin());
		}
		else {
			cout << "Input COM port number and press Enter\n";
			short number = 0;
			cin >> number;
			com_port += to_string(number);
		}
		return com_port;
	} while (true);
}

// Check for socket error codes here: https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

void SendToSocket(TcpClient& client, string msg)
{
	try {
		client.Send(&msg[0], msg.length());
#ifdef DEBUG_SOCKET
		cout << "Socket wrt [" << msg << "]\n";
#endif
	}
	catch (SocketException& ex) {
		cout << "Socket wrt: error code " << ex.GetWSErrorCode() << "\n";
	}
}

string ReceiveFromSocket(TcpClient& client, short length)
{
	try {
		string out;
		out.reserve(length);

		client.Recv(&out[0], length);

#ifdef DEBUG_SOCKET
		cout << "Socket rec [" << out << "]\n";
#endif
		return out;
	}
	catch (SocketException& ex) {
		cout << "Socket rec: error code " << ex.GetWSErrorCode() << "\n";
		return string(length - 1, '0');
	}
}