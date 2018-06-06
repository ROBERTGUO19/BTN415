#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <string>
#include "Packet.h"

int main(int argc, char* argv[])
{
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return -1;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return -1;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address

	std::string InputStr = "";
	unsigned int CurrentLine = 0;
	Packet newPkt;

	std::ifstream f("InputFile.txt");
	if (f.is_open())
	{
		while (!f.eof())
		{
			std::getline(f, InputStr);

			newPkt.SetLineNumber(CurrentLine++);
			newPkt.SetData((char *)InputStr.c_str(), InputStr.length());
			int Size = 0;
			char *Tx = newPkt.SerializeData(Size);

			//TODO:   Add the UDP send here

			Sleep(500);
		}
	}

	closesocket(ClientSocket);
	WSACleanup();

	return 1;
}
