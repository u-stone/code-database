// AsyncEventSelect.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "../lcsocket/sockwrapper.h"

#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3){
		std::cout << "参数格式：  xxx.exe  ip   port" << std::endl;
		return -1;
	}
	TCHAR* argv1 = "127.0.0.1";
	TCHAR* argv2 = "9999";
	netjob nj;
	struct addrinfo hints, *addrs, *ptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(argv1, argv2, &hints, &addrs);
	if (ret == SOCKET_ERROR)
		return -2;

	SOCKET sock_con = INVALID_SOCKET;
	for (ptr = addrs; ptr != NULL; ptr = ptr->ai_next){
		sock_con = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock_con == INVALID_SOCKET){
			return -3;
		}
		
		ret = connect(sock_con, ptr->ai_addr, ptr->ai_addrlen);
		if (ret == SOCKET_ERROR){
			ret = WSAGetLastError();
			closesocket(sock_con);
			sock_con = INVALID_SOCKET;
			continue;
		}
		
		break;
	}
	if (sock_con == SOCKET_ERROR)
		return -4;

	freeaddrinfo(addrs);
	addrs = 0;
	
#define EVENT_COUNT 1

	WSAEVENT events[EVENT_COUNT];
	events[0] = WSACreateEvent();
	ret = WSAEventSelect(sock_con, events[0], FD_READ | FD_WRITE | FD_CLOSE);
	if (ret == SOCKET_ERROR){
		closesocket(sock_con);
		return -5;
	}
	char optval = 0;
	int optlen = sizeof(optval);
	ret = getsockopt(sock_con, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
	if (ret == SOCKET_ERROR){
		ret = WSAGetLastError();
	}

	char recvbuf[20];
	char sendbuf[20];

	while (true){

		DWORD index = WSAWaitForMultipleEvents(1, events, FALSE, 2000, FALSE) - WSA_WAIT_EVENT_0;
		if (index == WSA_WAIT_FAILED){
			closesocket(sock_con);
			return -6;
		}

		if (index == WSA_WAIT_TIMEOUT){
			//TODO: send keep alive package
			send(sock_con, "keep alive", 10, 0);
			continue;
		}

		WSANETWORKEVENTS myNWE;
		ret = WSAEnumNetworkEvents(sock_con, events[index], &myNWE);
		if (ret == SOCKET_ERROR){
			closesocket(sock_con);
			return -7;
		}

		if (myNWE.lNetworkEvents & FD_READ){
			if (myNWE.iErrorCode[FD_READ_BIT] != 0){
				continue;
			}

			ret = recv(sock_con, recvbuf, sizeof(recvbuf), 0);

			//收到数据之后  做什么呢  todo
			std::cout << "\nreceived : " << std::endl;
			std::cout << recvbuf << std::endl;
		}

		if (myNWE.lNetworkEvents & FD_WRITE){
			if (myNWE.iErrorCode[FD_WRITE_BIT] != 0){
				continue;
			}

			//应该如何准备要发送的数据呢 todo
			memset(sendbuf, 0, sizeof(sendbuf));
			memcpy(sendbuf, "i'm client", 10);
			//while (1){
				ret = send(sock_con, sendbuf, sizeof(sendbuf), 0);
				if (ret == SOCKET_ERROR){
					if (WSAGetLastError() == WSAEWOULDBLOCK){
						break;
					} else {
						std::cout << "error " << std::endl;
					}
				}
			//}
		}

		if (myNWE.lNetworkEvents & FD_CLOSE){
			if (myNWE.iErrorCode[FD_CLOSE_BIT] != 0){
				continue;
			}
			shutdown(sock_con, SD_SEND);
			closesocket(sock_con);
		}

	}
	
	return 0;
}

