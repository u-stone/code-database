// IOCP_server.cpp : 定义控制台应用程序的入口点。
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

typedef struct _PER_HANDLE_DATA{
	SOCKET sock;
	SOCKADDR_STORAGE client_addr;
}PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

typedef struct{
	WSABUF     buf;
	WSAOVERLAPPED ol;
	int        buflen;
	int        optype;
	//int        dummy[1024];
}PER_IO_DATA, *LPPER_IO_DATA;

int _tmain(int argc, _TCHAR* argv[])
{
	netjob nj;
	////////////////////////////支持IPv6的代码//////////////////////////////////////////////
	//SOCKET sock_lis = INVALID_SOCKET;
	//char* szPort = "9999";
	//struct addrinfo hints, *addrs, *ptr;
	//int count = 0, ret;
	//memset(&hints, 0, sizeof(hints));
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;
	//ret = getaddrinfo(NULL, szPort, &hints, &addrs);
	//if (ret != 0){
	//	return -1;
	//}

	//ptr = addrs;

	//while (ptr){
	//	sock_lis = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	//	if (sock_lis == INVALID_SOCKET){
	//		continue;
	//	}
	//	ret = bind(sock_lis, ptr->ai_addr, ptr->ai_addrlen);
	//	if (ret == SOCKET_ERROR){
	//		continue;
	//	}
	//	ret = listen(sock_lis, 7);
	//	if (ret == SOCKET_ERROR){
	//		continue;
	//	}
	//	//break;
	//	count ++;
	//	ptr = ptr->ai_next;
	//}
	//if (sock_lis == INVALID_SOCKET){
	//	return -1;
	//}

	/////////////////////////////仅支持IPv4的代码/////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////

	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < si.dwNumberOfProcessors; ++i){
		HANDLE th = CreateThread(NULL, 0, WorkThread, hcp, 0, NULL);
	}

	char bufrecv[DATA_BUFSIZE] = "i'm server";
	char bufsend[DATA_BUFSIZE] = "i'm server";

	DWORD bytesrecv = 0, flags = 0;
	while (1){
		PER_HANDLE_DATA* phd = 0;
		SOCKADDR_IN saRemote;
		SOCKET sock_con = INVALID_SOCKET;
		int remote_len = sizeof(saRemote);
		sock_con = accept(sock_lis, (SOCKADDR*)&saRemote, &remote_len);
		if (sock_con == INVALID_SOCKET){
			ret = WSAGetLastError();
			cout << "error : " << ret << endl;
			break;
		}
		phd = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		phd->sock = sock_con;
		memcpy(&phd->client_addr, &saRemote, remote_len);
		LPPER_IO_DATA piod = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
		piod->buf.buf = bufrecv;
		piod->buf.len = DATA_BUFSIZE;
		piod->buflen = 1;
		piod->optype = RECV_POSTED;
		memset(&piod->ol, 0, sizeof(piod->ol));

		CreateIoCompletionPort((HANDLE)sock_con, hcp, (DWORD)phd, 0);
		if (WSARecv(sock_con, &(piod->buf), 1, &bytesrecv, &flags, &(piod->ol), NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSA_IO_PENDING){
				ret = WSAGetLastError();
				cout << ret << endl;
				return 2; 
			}
		}

		LPPER_IO_DATA piodsend = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
		piodsend->buf.buf = bufsend;
		piodsend->buf.len = DATA_BUFSIZE;
		piodsend->buflen = 1;
		piodsend->optype = SEND_POSTED;
		memset(&piodsend->ol, 0, sizeof(piodsend->ol));

		CreateIoCompletionPort((HANDLE)sock_con, hcp, (DWORD)phd, 0);
		if (WSASend(sock_con, &(piodsend->buf), 1, &bytesrecv, flags, &(piodsend->ol), NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSA_IO_PENDING){
				ret = WSAGetLastError();
				cout << ret << endl;
				return 2; 
			}
		}
	}
	return 0;
}

DWORD WINAPI WorkThread(LPVOID lpParam){
	
	HANDLE hcp = (HANDLE)lpParam;
	DWORD bytestransf;
	//OVERLAPPED ol;
	LPPER_HANDLE_DATA phd;
	LPPER_IO_DATA piod;
	WSAOVERLAPPED *pol;
	DWORD bytessend, bytesrecv;
	DWORD flags;
	char bufrecv[DATA_BUFSIZE],	bufsend[DATA_BUFSIZE] = "i'm server";
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

		if (piod->optype == RECV_POSTED){
			cout << "received : " << piod->buf.buf << endl;
			flags = 0;
			memset(&(piod->ol), 0, sizeof(OVERLAPPED));
			piod->buf.buf = bufrecv;
			piod->buf.len = DATA_BUFSIZE;
			piod->optype = RECV_POSTED;
			if (WSARecv(phd->sock, &piod->buf, 1, &bytesrecv, &flags, &(piod->ol), NULL) == SOCKET_ERROR){
				if (WSAGetLastError() != WSA_IO_PENDING){
					cout << "error" << WSAGetLastError() << endl;
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
				if (WSAGetLastError() != WSA_IO_PENDING){
					cout << "error" << WSAGetLastError() << endl;
					return 2;
				}
			}
			cout << "send : " << piod->buf.buf << endl;
		}
	}
	return 0;
}