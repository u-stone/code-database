// OverlappedCompleteRoutineSocket_Client.cpp : 定义控制台应用程序的入口点。
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

void CALLBACK workroutine(DWORD dwerror, DWORD bytestransf, LPWSAOVERLAPPED ol, DWORD inflags);
SOCKET sock_con = INVALID_SOCKET;
int falgs = 0;

WSABUF wsabuf;
char bufsend[BUF_SIZE] = "i'm client";

DWORD bytessend = 0;
DWORD flags = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	addrinfo *addrs, hints, *ptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char* ip = "127.0.0.1";
	char* port = "9999";
	int ret = getaddrinfo(ip, port, &hints, &addrs);
	if (ret == SOCKET_ERROR){
		cout << "getaddrinfo error" << endl;
		return -1;
	}

	for (ptr = addrs; ptr != 0; ptr = ptr->ai_next){
		sock_con = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock_con == INVALID_SOCKET){
			continue;
		}
		ret = connect(sock_con, ptr->ai_addr, ptr->ai_addrlen);
		if (SOCKET_ERROR == ret){
			cout << "connect error" << endl;
			continue;
		}
		break;
	}
	if (sock_con == INVALID_SOCKET){
		cout << "invalide socket created " << endl;
		return -1;
	}

	freeaddrinfo(addrs);

	WSAOVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));
	wsabuf.buf = bufsend;
	wsabuf.len = BUF_SIZE;



	//throw an send request
	if (WSASend(sock_con, &wsabuf, 1, &bytessend, flags, &ol, workroutine) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			cout << "throw send request error" << endl;
			return -1;
		}
	}

	WSAEVENT events[1];
	events[0] = WSACreateEvent();

	while (1){
		int index = WSAWaitForMultipleEvents(1, events, FALSE, WSA_INFINITE, TRUE);
		if (index == WSA_WAIT_IO_COMPLETION){
			cout << "an routin finished" << endl;
			//WSAResetEvent(events[0]);
			continue ;
		} else {
			cout << "an error occurred" << endl;
			return -1;
		}
	}

	return 0;
}


void CALLBACK workroutine(DWORD dwerror, DWORD bytestransf, LPWSAOVERLAPPED sendol, DWORD flags)
{
	DWORD bytessend, bytesrecv;
	if (dwerror != 0 || bytestransf == 0){
		closesocket(sock_con);
		return ;
	}
	cout << "send " << bufsend << endl;
	Sleep(1000);

	falgs = 0;
	memset(sendol, 0, sizeof(WSAOVERLAPPED));
	wsabuf.buf = bufsend;
	wsabuf.len = BUF_SIZE;

	if (WSASend(sock_con, &wsabuf, 1, &bytessend, flags, sendol, workroutine) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			cout << "WSASend error" << endl;
			return ;
		}
		cout << "throw an send request" << endl;
	}
}