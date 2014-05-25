// OverlappedSocket_Server.cpp : 定义控制台应用程序的入口点。
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

#define BUF_SIZE 4096
#define MAX_CONCURRENCY   WSA_MAXIMUM_WAIT_EVENTS

int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	SOCKET sock_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	int ret = bind(sock_listen, (sockaddr*)&sin, sizeof(sin));
	if (ret == SOCKET_ERROR){
		ret = WSAGetLastError();
		cout << "bind error " << ret << endl;
		return -1;
	}

	ret = listen(sock_listen, 5);
	if (ret == SOCKET_ERROR){
		closesocket(sock_listen);
		return -1;
	}

	SOCKADDR_IN client_addr;
	int addrlen = sizeof(client_addr);

	SOCKET sock_con = accept(sock_listen, (SOCKADDR*)&client_addr, &addrlen);
	if (sock_con == INVALID_SOCKET){
		ret = WSAGetLastError();
		cout << "accept error " << ret << endl;
		return -1;
	}
	WSAEVENT events[MAX_CONCURRENCY] = {0};
	WSABUF databuf[MAX_CONCURRENCY];
	WSAOVERLAPPED ols[MAX_CONCURRENCY];
	memset(ols, 0, sizeof(ols));

	int total = 0;
	events[total] = WSACreateEvent();
	ols[total].hEvent = events[total];

	char buf[MAX_CONCURRENCY][BUF_SIZE] = {0};
	databuf[total].buf = buf[total];
	databuf[total].len = BUF_SIZE;

	total++;

	events[total] = WSACreateEvent();
	ols[total].hEvent = events[total];

	databuf[total].buf = buf[total];
	databuf[total].len = BUF_SIZE;

	total++;

	DWORD bytesrecv = 0, flags = 0, bytestransf = 0, bytessend = 0;
 	if (WSARecv(sock_con, &databuf[0], 1, &bytesrecv, &flags, &ols[0], NULL) == SOCKET_ERROR){
 		if (WSAGetLastError() != WSA_IO_PENDING){
 			ret = WSAGetLastError();
 			cout << "WSARecv error :" << ret << endl;
 			return -2;
 		}
 	}
	while (1){
		int index = WSAWaitForMultipleEvents(total, events, FALSE, 2000, FALSE);
		if (index == WSA_WAIT_TIMEOUT){
			cout << "服务器：等待超时" << endl;
			continue;
		}
		index -= WSA_WAIT_EVENT_0;
		cout << "激活一个socket : " << index << endl;
		WSAResetEvent(events[index]);
		WSAGetOverlappedResult(sock_con, &ols[index], &bytestransf, FALSE, &flags);
		if (bytestransf == 0){
			ret = WSAGetLastError();
			shutdown(sock_con, SD_SEND);
			closesocket(sock_con);
			cout << "error :" << ret << endl;
			WSACloseEvent(events[index - WSA_WAIT_EVENT_0]);
			return -3;
		}

		//cout << "received: " <<  buf[index] << endl;
		memset(&ols[index], 0, sizeof(ols[0]));

		//reset
		memset(&ols[index], 0, sizeof(ols[index]));
		ols[index].hEvent = events[index];
		databuf[index].buf = buf[index];
		databuf[index].len = BUF_SIZE;
		//接收到数据，要重新发送接收请求
			if (WSARecv(sock_con, &databuf[index], 1, &bytesrecv, &flags, &ols[index], NULL) == SOCKET_ERROR){
				if (WSA_IO_PENDING != WSAGetLastError()){
					cout << "-4" << endl;
					return -4;
				}
			}
	}
	return 0;
}

