#define WIN32_LEAN_AND_MEAN

#include "interface.h"

#define DEBUG_SOCKET

using namespace std;

std::list<int> GetCOMports()
{
	wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
	std::list<int> portList;

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		wstring str = L"COM" + to_wstring(i); // converting to COM0, COM1, COM2

		if (QueryDosDeviceW(str.c_str(), lpTargetPath, 5000)) //QueryDosDeviceW returns zero if it didn't find an object
		{
			portList.push_back(i);
			std::cout << std::string(str.begin(), str.end()) << "\n"; //" - " << lpTargetPath << endl;
		}
		//if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) NULL;
	}
	return portList;
}

std::string SelectCOMport()
{
	std::string com_port = "\\\\.\\COM";
	do {
		std::list<int> COMports = GetCOMports();
		if (COMports.empty()) {
			cerr << "No COM ports available! Press q to exit or any other key to retry.\n\n";
			if (_getch() == 'q') exit(EXIT_SUCCESS);
			continue;
		}
		else if (COMports.size() == 1) {
			com_port += to_string(*COMports.begin());
		}
		else {
			std::cout << "Input COM port number and press Enter\n";
			short number = 0;
			cin >> number;
			com_port += to_string(number);
		}
		return com_port;
	} while (true);
}

// Check for socket error codes here: https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

void SendToSocket(TcpClient& socket, std::string msg)
{
	try {
		socket.Send(&msg[0], msg.length());
#ifdef DEBUG_SOCKET
		std::cout << "Socket wrt [" << msg << "]\n";
#endif
	}
	catch (SocketException& ex) {
		std::cout << "Socket wrt: error code " << ex.GetWSErrorCode() << "\n";
	}
}

std::string ReceiveFromSocket(TcpClient& socket, short length)
{
	std::string out;
	out.reserve(length);

	try {
		socket.Recv(&out[0], length);
#ifdef DEBUG_SOCKET
		std::cout << "Socket rec [" << out << "]\n";
#endif
	}
	catch (SocketException& ex) {
		std::cout << "Socket rec: error code " << ex.GetWSErrorCode() << "\n";
		out = std::string(length - 1, '0');
	}

	return out;
}