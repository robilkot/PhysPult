#include <ws2tcpip.h>
#include <exception>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)
#pragma once

class SocketException : std::exception {
	int _wsErrorCode;

public:
	SocketException() : std::exception() {
		_wsErrorCode = WSAGetLastError();
	}

	SocketException(int wsErrorCode) : std::exception() {
		_wsErrorCode = wsErrorCode;
	}

	int GetWSErrorCode() {
		return _wsErrorCode;
	}
};

class TcpSocket {
protected:
	SOCKET _socket;
	sockaddr* _socketAddress;

	static WSADATA* s_wsData;

public:
	TcpSocket(const char* ipString, u_short port) {
		Initialize();

		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in addressInfo{};
		addressInfo.sin_family = AF_INET;
		addressInfo.sin_port = htons(port);
		addressInfo.sin_addr.s_addr = inet_addr(ipString);

		_socketAddress = (sockaddr*)&addressInfo;
	}

	TcpSocket(SOCKET socket) {
		_socket = socket;
		_socketAddress = nullptr;
	}

	~TcpSocket() {
		closesocket(_socket);
		Dispose();
	}

	static void Initialize() {
		WSADATA data;
		int result = WSAStartup(MAKEWORD(2, 2), &data);

		if (result != 0) {
			throw SocketException(result);
		}
	}

	static WSADATA* GetWSData() {
		return s_wsData;
	}

	static void Dispose() {
		int result = WSACleanup();

		if (result != 0) {
			throw SocketException(result);
		}
	}
};

class TcpClient : TcpSocket {
public:
	TcpClient(const char* ipString, u_short port) : TcpSocket(ipString, port) { }

	TcpClient(SOCKET socket) : TcpSocket(socket) { }

	void Connect() {
		if (_socketAddress != nullptr) {
			int result = connect(_socket, _socketAddress, sizeof(sockaddr));

			if (result == SOCKET_ERROR) {
				throw SocketException();
			}
		}
	}

	void Send(const char* buffer, int bufLen, int flags = NULL) {
		int result = send(_socket, buffer, bufLen, flags);

		if (result == SOCKET_ERROR) {
			throw SocketException();
		}
	}

	void Recv(char* buffer, int bufLen, int flags = NULL) {
		int result = recv(_socket, buffer, bufLen, flags);

		if (result == SOCKET_ERROR) {
			throw SocketException();
		}
	}
};

class TcpServer : TcpSocket {
private:
	void(*_clientHandler)(TcpClient*);
	int _backlog;

public:
	TcpServer(const char* ipString, u_short port, void(*clientHandler)(TcpClient*) , int backlog = 5) : TcpSocket(ipString, port) {
		bind(_socket, _socketAddress, sizeof(sockaddr));
		_clientHandler = clientHandler;
		_backlog = backlog;
	}

	int Start() {
		int result = listen(_socket, _backlog);

		if (result == SOCKET_ERROR) {
			throw SocketException();
		}

		while (true) {
			SOCKET clientSocket = accept(_socket, NULL, NULL);

			if (clientSocket != INVALID_SOCKET) {
				_clientHandler(new TcpClient(clientSocket));
			}

			Sleep(100);
		}
	}
};
