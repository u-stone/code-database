// OverlappedSocket_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <iostream>
using namespace std;

#include "../lcsocket/sockwrapper.h"

#define BUF_SIZE  4096

int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	char *ip = "127.0.0.1";
	char *port = "9999";
	struct addrinfo* addrs, hints, *ptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(ip, port, &hints, &addrs);
	if (ret == SOCKET_ERROR){
		return -1;
	}
	SOCKET sock_con = INVALID_SOCKET;
	for (ptr = addrs; ptr != 0; ptr = ptr->ai_next){
		sock_con = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock_con == INVALID_SOCKET){
			cout << "1" << endl;
			continue ;
		}
		ret = connect(sock_con, ptr->ai_addr, ptr->ai_addrlen);
		if (SOCKET_ERROR == ret){
			cout << "2" << endl;
			continue;
		}
		break;
	}
	if (sock_con == INVALID_SOCKET){
		return -2;
	}
	freeaddrinfo(addrs);

	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	WSAOVERLAPPED ol;
	int total = 0;

	events[total] = WSACreateEvent();
	ZeroMemory(&ol, sizeof(ol));
	ol.hEvent = events[total];

	char buf[BUF_SIZE] = {0};
	WSABUF databuf;

	databuf.buf = buf;
	databuf.len = BUF_SIZE;

	total++;

	DWORD bytessend = 0;
	DWORD flags = 0;
	DWORD bytestrans = 0;

	char strSend[] = "i'm client";
	memcpy(buf, strSend, strlen(strSend));
	if (WSASend(sock_con, &databuf, 1, &bytessend, flags, &ol, NULL) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			ret = WSAGetLastError();
			cout << "WSASend error:" << WSAGetLastError() << endl;
			return -3;
		}
	}
	
	int count = 0;
	while (1){
		int index = WSAWaitForMultipleEvents(total, events, FALSE, 2000/*WSA_INFINITE*/, FALSE);
		if (index == WSA_WAIT_TIMEOUT){
			cout << "客户端，等待超时" << endl;
			continue;
		}
		Sleep(1000);//操作太快了，等待一下，方便看效果

		WSAResetEvent(events[index - WSA_WAIT_EVENT_0]);

		WSAGetOverlappedResult(sock_con, &ol, &bytestrans, FALSE, &flags);

		if (bytestrans == 0){
			shutdown(sock_con, SD_SEND);
			closesocket(sock_con);
			sock_con = INVALID_SOCKET;
			WSACloseEvent(events[index - WSA_WAIT_EVENT_0]);
			cout << "no bytes send" << endl;
			return -4;
		}

		cout << "receive : " << buf << endl;

		ZeroMemory(&ol, sizeof(ol));
		ol.hEvent = events[0];
		databuf.buf = buf;
		databuf.len = BUF_SIZE;

		char sendbuf[30] = {0};
		sprintf(sendbuf, "this is %d , and send \"i'm client\"", count++);
		memcpy(buf, sendbuf, strlen(sendbuf));
		if (WSASend(sock_con, &databuf, 1, &bytessend, flags, &ol, NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSA_IO_PENDING){
				cout << "WSASend error : " << WSAGetLastError() << endl;
				return -3;
			}
		}
	}

	return 0;
}

