#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void main()
{
	// Init WinSock
	WSADATA wsData;

	WORD ver=MAKEWORD(2, 2);

	int wsock = WSAStartup(ver, &wsData);

	if(wsock != 0)
	{
		std::cerr << "Can't init winsock!" << std::endl;
		return;
	}

	// Create Socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket!" << std::endl;
	}

	// Bind IP and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inetpton ...
	bind(listener, (sockaddr*)&hint, sizeof(hint));

	// Assign socket to winsock
	listen(listener, SOMAXCONN);

	// Wait for connection
	sockaddr_in client;
	int clientSz = sizeof(client);

	SOCKET clientSocket = accept(listener, (sockaddr*)&client, &clientSz);
	if(clientSocket == INVALID_SOCKET)
	{
		// If socket is faulty
	}

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXHOST]; // Service (i.e. port) the client connect is located

	memset(host, 0, NI_MAXHOST); // ZeroMemory(host, NI_MAXHOST);
	memset(service, 0, NI_MAXHOST);// ZeroMemory(service, NI_MAXHOST);

	if(getnameinfo((sockaddr*) &client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// Close Listening Socket
	closesocket(listener);

	// While : accept and echo msg to client
	char buff[4096];

	while(true)
	{
		memset(buff, 0, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buff, 4096, 0);

		if(bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error on recv(). Quitting" << std::endl;
			break;
		}

		if(bytesReceived == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		// Echo message to client
		send(clientSocket, buff, bytesReceived + 1, 0);

		// Print onto console
		std::cout << std::string(buff, 0, bytesReceived) << std::endl;
	}

	// Free Socket
	closesocket(clientSocket);

	// Clean WinSock
	WSACleanup();
}
