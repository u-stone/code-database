// AsyncEventSelect_svr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../lcsocket/sockwrapper.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR* argv1 = "127.0.0.1";
	netjob nj;
	struct addrinfo hints, *ptr, *addrs;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int ret = getaddrinfo(NULL, argv1, &hints, &addrs);
	if (ret == SOCKET_ERROR){
		return -2;
	}
	SOCKET sock_lis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(56666);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	ret = bind(sock_lis, (sockaddr*)&sin, sizeof(sin));
	if (ret == SOCKET_ERROR){
		ret = WSAGetLastError();
		freeaddrinfo(addrs);
		closesocket(sock_lis);
		return -4;
	}
	freeaddrinfo(addrs);

	ret = listen(sock_lis, 5);
	if (ret == SOCKET_ERROR){
		closesocket(sock_lis);
		return -5;
	}
	
	WSAEVENT events[10] = {0};
	SOCKET socks[10] = {0};

	int total = 0;
	events[total] = WSACreateEvent();
	socks[total] = sock_lis;
	WSAEventSelect(socks[total], events[total], FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE);
	total++;


	char recvbuf[20] = {0};
	char sendbuf[20] = {0};

	while (1){
		std::cout << "ready for wait,and for infinite..." << std::endl;
		int index = WSAWaitForMultipleEvents(total, events, FALSE, WSA_INFINITE, FALSE) - WSA_WAIT_EVENT_0;
		if (index == WSA_WAIT_TIMEOUT)
			continue ;

		index = index - WSA_WAIT_EVENT_0;

		for (int i = index; i < total; ++i){
			index = WSAWaitForMultipleEvents(1, &events[i], TRUE, 0, FALSE);
			if (index == WSA_WAIT_FAILED){
				std::cout << "wait ,but failed" << std::endl;
				continue;
			}

			WSANETWORKEVENTS event;
			WSAEnumNetworkEvents(socks[i], events[i], &event);

			if (event.lNetworkEvents & FD_ACCEPT){
				if (event.iErrorCode[FD_ACCEPT_BIT] == 0){
					if (total > WSA_MAXIMUM_WAIT_EVENTS){
						std::cout << "out of limit" << std::endl;
						continue;
					}

					SOCKET sock = accept(socks[i], 0, 0);
					std::cout << "aceept a socket" << std::endl;
					WSAEVENT event = WSACreateEvent();
					events[total] = event;
					socks[total] = sock;
					total++;
				}
			} else if (event.lNetworkEvents & FD_CLOSE){
				if (event.iErrorCode[FD_CLOSE_BIT] == 0){
					shutdown(socks[i], SD_SEND);
					std::cout << "close a socket" << std::endl;
					closesocket(socks[i]);
					for (int j = i; j < total - 1; ++j){
						socks[j] = socks[j+1];
						events[j] = events[j+1];
					}
				}
			} else if (event.lNetworkEvents & FD_READ){
				if (event.iErrorCode[FD_READ_BIT] == 0){
					std::cout << "read a socket" << std::endl;
					memset(recvbuf, 0, sizeof(recvbuf));
					ret = recv(socks[i], recvbuf, sizeof(recvbuf), 0);
					if (ret > 0){
						//TODO: 接收成功
						std::cout << "received:" << std::endl;
						std::cout << recvbuf << std::endl;
					} else if (ret == SOCKET_ERROR) {
						if (WSAGetLastError() == WSAEWOULDBLOCK){
							std::cout << "recv buffer is empty" << std::endl;
							break;
						} else {
							std::cout << "some real error here..." << std::endl;
							break;
						}
					} else {
						std::cout << "Connectino is closed" << std::endl;
					}
				}
			} else if (event.lNetworkEvents & FD_WRITE){
				if (event.iErrorCode[FD_WRITE_BIT] == 0){
					memset(sendbuf, 0, sizeof(sendbuf));
					memcpy(sendbuf, "i'm server", 10);
					std::cout << "write some bytes" << std::endl;
					//while (1){
						ret = send(socks[i], sendbuf, sizeof(sendbuf), 0);
						if (ret == SOCKET_ERROR){
							if (WSAGetLastError() == WSAEWOULDBLOCK){
								//send buf is full, ready for resend. but now don't handle it.
								std::cout << "kernel send buffer is full, wait for the next send-time." << std::endl;
								break;
							} else {
								//some error don't kown
								std::cout << "error " << std::endl;
							}
						} else {
							std::cout << "send successfully" << std::endl;
						}
					//}
				}
				break;
			}
		}
	}
	
	return 0;
}

