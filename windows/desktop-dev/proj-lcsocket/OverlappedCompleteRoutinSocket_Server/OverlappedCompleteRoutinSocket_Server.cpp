// OverlappedCompleteRoutinSocket_Server.cpp : 定义控制台应用程序的入口点。
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

SOCKET sock_con = INVALID_SOCKET;

void CALLBACK workroutine(DWORD dwerror, DWORD bytestransf, LPWSAOVERLAPPED ol, DWORD inflags);

WSABUF wsabuf;
char bufrecv[BUF_SIZE] = {0};


int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	SOCKET sock_lis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	int ret = bind(sock_lis, (sockaddr*)&sin, sizeof(sin));

	if (ret == SOCKET_ERROR){
		ret = WSAGetLastError();
		cout << "error : " << ret << endl;
		return -1;
	}

	ret = listen(sock_lis, 5);
	if (ret == SOCKET_ERROR){
		closesocket(sock_lis);
		return -1;
	}
	SOCKADDR_IN client_addr;
	int addr_len = sizeof(client_addr);

	sock_con = accept(sock_lis, (SOCKADDR*)&client_addr, &addr_len);
	if (sock_con == INVALID_SOCKET){
		ret = WSAGetLastError();
		cout << "error" << ret << endl;
		return -1;
	}

	wsabuf.buf = bufrecv;
	wsabuf.len = BUF_SIZE;

	WSAOVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));

	WSAEVENT events[1];
	events[0] = WSACreateEvent();

	DWORD bytesrecv = 0, flags = 0;

	if (WSARecv(sock_con, &wsabuf, 1, &bytesrecv, &flags, &ol, workroutine) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			cout << "WSARecv error : " << endl;
			return -1;
		}
	}

	while (1){
		int index = WSAWaitForMultipleEvents(1, events, FALSE, WSA_INFINITE, TRUE);
		if (index == WSA_WAIT_IO_COMPLETION){
			cout << "routine finished" << endl;
			//WSAResetEvent(events[0]);
			continue ;
		} else {
			cout << "incredible error occur" << endl;
			return -1;
		}

	}

	return 0;
}

void CALLBACK workroutine(DWORD dwerror, DWORD bytestransf, LPWSAOVERLAPPED ol, DWORD inflags)
{
	if (dwerror != 0 || bytestransf == 0){
		cout << "routine error" << endl;
		return ;
	}
	cout << "received " << bufrecv << endl;

	Sleep(1000);
	inflags = 0;
	memset(ol, 0, sizeof(ol));

	wsabuf.len = BUF_SIZE;
	wsabuf.buf = bufrecv;
	DWORD bytesrecv = 0, flags = 0;
	if (WSARecv(sock_con, &wsabuf, 1, &bytesrecv, &flags, ol, workroutine) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			cout << "WSARecv error " << endl;
			return ;
		}
	}

}