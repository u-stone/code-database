// IOCP_client.cpp : 定义控制台应用程序的入口点。
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

#define  DATA_BUFSIZE 4096
#define RECV_POSTED 1
#define SEND_POSTED 2

DWORD WINAPI WorkThread(LPVOID lpParam);

struct PER_HANDLE_DATA 
{
	SOCKET sock;
};

struct PER_IO_DATA{
	WSAOVERLAPPED ol;
	WSABUF        buf;
	int           optype;
};


int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	addrinfo hints, *addrs, *ptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	char* ip = "127.0.0.1";
	char* port = "9999";
	int ret = getaddrinfo(ip, port, &hints, &addrs);
	if (ret != 0){
		return -1;
	}
	SOCKET sock_con = INVALID_SOCKET;
	for (ptr = addrs; ptr != NULL; ptr = ptr->ai_next){
		sock_con = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock_con == INVALID_SOCKET){
			continue;
		}
		ret = connect(sock_con, ptr->ai_addr, ptr->ai_addrlen);
		if (ret == SOCKET_ERROR){
			continue ;
		}
		break;
	}
	if (sock_con == INVALID_SOCKET){
		return -2;
	}
	freeaddrinfo(addrs);

	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (si.dwNumberOfProcessors < 2)
		return -2;

	char bufsend[DATA_BUFSIZE] = "i'm client";
	char bufrecv[DATA_BUFSIZE] = {0};
	HANDLE *threads = new HANDLE[si.dwNumberOfProcessors];
	for (int i = 0; i < si.dwNumberOfProcessors/2; ++i){
		threads[i] = CreateThread(NULL, 0, WorkThread, hcp, 0, NULL);
		PER_HANDLE_DATA* phd = (PER_HANDLE_DATA*)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		phd->sock = sock_con;

		CreateIoCompletionPort((HANDLE)sock_con, hcp, (DWORD)phd, 0);

		PER_IO_DATA* piod = (PER_IO_DATA*)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
		piod->buf.buf = bufsend;
		piod->buf.len = DATA_BUFSIZE;
		memset(&piod->ol, 0, sizeof(piod->ol));
		piod->optype = SEND_POSTED;
		DWORD bytessend = 0, flags = 0;
		//抛出一个发送数据的请求
		if (WSASend(phd->sock, &(piod->buf), 1, &bytessend, flags, &(piod->ol), NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSA_IO_PENDING){
				return 2;
			}
		}
	}
	for (int i = si.dwNumberOfProcessors / 2; i < si.dwNumberOfProcessors; ++i){
		threads[i] = CreateThread(NULL, 0, WorkThread, hcp, 0, NULL);
		PER_HANDLE_DATA* phd = (PER_HANDLE_DATA*)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		phd->sock = sock_con;

		CreateIoCompletionPort((HANDLE)sock_con, hcp, (DWORD)phd, 0);

		PER_IO_DATA* piod = (PER_IO_DATA*)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
		piod->buf.buf = bufsend;
		piod->buf.len = DATA_BUFSIZE;
		memset(&piod->ol, 0, sizeof(piod->ol));
		piod->optype = RECV_POSTED;
		DWORD bytesrecv = 0, flags = 0;
		//抛出一个接受数据的请求 
		if (WSARecv(phd->sock, &(piod->buf), 1, &bytesrecv, &flags, &(piod->ol), NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSA_IO_PENDING){
				return 2;
			}
		}
	}
	//wait until all thread quit!
	WaitForMultipleObjects(si.dwNumberOfProcessors, threads, TRUE, INFINITE);
	return 0;
}

DWORD WINAPI WorkThread(LPVOID lpParam)
{
	HANDLE hcp = (HANDLE)lpParam;
	DWORD bytestransf;
	//OVERLAPPED ol;
	PER_HANDLE_DATA *phd;
	PER_IO_DATA *piod;
	WSAOVERLAPPED *pol;
	DWORD bytessend, bytesrecv;
	DWORD flags;
	char bufrecv[DATA_BUFSIZE],	bufsend[DATA_BUFSIZE] = "i'm client and iocp with overlapped! O(∩_∩)O";
	while (1) {
		if (!GetQueuedCompletionStatus(hcp, &bytestransf, (LPDWORD)&phd, &pol, INFINITE))
			continue ;
		piod = CONTAINING_RECORD(pol, PER_IO_DATA, ol);
		if (bytestransf == 0 && (piod->optype == RECV_POSTED || piod->optype == SEND_POSTED)){
			closesocket(phd->sock);
			GlobalFree(phd);
			GlobalFree(piod);
			continue ;
		}
		Sleep(1000);
		if (piod->optype == RECV_POSTED){
			cout << "received : " << piod->buf.buf << endl;
			bytesrecv = 0;
			flags = 0;
			memset(&(piod->ol), 0, sizeof(OVERLAPPED));
			piod->buf.buf = bufrecv;
			piod->buf.len = DATA_BUFSIZE;
			piod->optype = RECV_POSTED;
			if (WSARecv(phd->sock, &piod->buf, 1, &bytesrecv, &flags, &(piod->ol), NULL) == SOCKET_ERROR){
				if (WSAGetLastError() != WSA_IO_PENDING){
					cout << "error" << endl;
					return 1;
				}
			}
		} else if (piod->optype == SEND_POSTED) {
			bytessend = 0;
			flags = 0;
			memset(&(piod->ol), 0, sizeof(OVERLAPPED));
			piod->buf.buf = bufsend;
			piod->buf.len = DATA_BUFSIZE;
			piod->optype = SEND_POSTED;
			if (WSASend(phd->sock, &piod->buf, 1, &bytessend, flags, &(piod->ol), NULL) == SOCKET_ERROR){
				if (WSAGetLastError() == WSA_IO_PENDING){
					cout << "error : " << endl;
					return 2;
				}
			}
			cout << "send : " << piod->buf.buf << endl;
		}
	}
	return 0;
}