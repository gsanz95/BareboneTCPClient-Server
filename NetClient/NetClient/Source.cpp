#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void main()
{
	std::string ipAddr = "127.0.0.1";	// IP Address of the server
	int port = 54000;					// Port of connection used

	//Init WinSock
	WSAData wsdata;
	WORD ver = MAKEWORD(2,2);

	int wsRes = WSAStartup(ver, &wsdata);

	if(wsRes != 0)
	{
		std::cerr << "Can't create winsock, Err# " << wsRes << std::endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		std::cerr << "Socket unable to be created, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// Fill sockaddr struct
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddr.c_str(), &hint.sin_addr);

	// Connect to server
	int conn = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if(conn == SOCKET_ERROR)
	{
		std::cerr << "Failed to connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// While : send and receive data
	char buff[4096];
	std::string userInput;

	do
	{
		// Prompt for text input
		std::cout << "> ";
		std::getline(std::cin, userInput);

		if(!userInput.empty()) // User has entered something
		{
			// Send text to server
			int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if(sendRes != SOCKET_ERROR)
			{
				memset(buff, 0, 4096);

				// Wait for response
				int bytesReceived = recv(sock, buff, 4096, 0);
				if(bytesReceived > 0)
				{
					// Echo response into console
					std::cout << "SERVER> " << std::string(buff, 0, bytesReceived) << std::endl;
				}
			}

		}
	}
	while(!userInput.empty());

	// Cleanup all
	closesocket(sock);
	WSACleanup();
}