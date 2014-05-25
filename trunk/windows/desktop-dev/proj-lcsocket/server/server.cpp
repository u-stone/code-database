// server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// �¼�������׽��־��
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET  sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;
	USHORT nPort = 4567; // �����˿ں�
	// ���������׽���
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if(::bind(sListen, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" Failed bind() /n");
		return -1;
	}
	::listen(sListen, 200);
	// �����¼����󣬲��������µ��׽���
	WSAEVENT event = ::WSACreateEvent();
	::WSAEventSelect(sListen, event, FD_ACCEPT|FD_CLOSE);
	// ��ӵ��б���
	eventArray[nEventTotal] = event;
	sockArray[nEventTotal] = sListen; 
	nEventTotal++;
	char szText[512];
	// ���������¼�
	while(TRUE)
	{
		// �����ж����ϵȴ�
		int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		// ȷ���¼���״̬
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		// WSAWaitForMultipleEvents���Ƿ��������¼��������Сֵ��Ϊ��ȷ�����е��¼�����õ�ִ�еĻ��ᣬ�� ����nIndex���¼����������ѯ��ʹ��õ�ִ�еĻ��ᡣ
		for(int i=nIndex; i<nEventTotal; i++) 
		{
			nIndex = ::WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 0, FALSE);
			if(nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				// ��ȡ������֪ͨ��Ϣ��WSAEnumNetworkEvents�������Զ����������¼�
				WSANETWORKEVENTS event;
				::WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event);
				if(event.lNetworkEvents & FD_ACCEPT)    // ����FD_ACCEPT�¼�
				{
					if(event.iErrorCode[FD_ACCEPT_BIT] == 0)
					{
						if(nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							printf(" Too many connections! /n");
							continue;
						}
						SOCKET sNew = ::accept(sockArray[i], NULL, NULL);
						WSAEVENT event = ::WSACreateEvent();
						::WSAEventSelect(sNew, event, FD_READ|FD_CLOSE|FD_WRITE);
						// ��ӵ��б���
						eventArray[nEventTotal] = event;
						sockArray[nEventTotal] = sNew; 
						nEventTotal++;
					}
				}
				else if(event.lNetworkEvents & FD_READ)   // ����FD_READ�¼�
				{ 
					if(event.iErrorCode[FD_READ_BIT] == 0)
					{
						memset(szText, 0x01, sizeof(szText));
						int nRecv = ::recv(sockArray[i], szText, strlen(szText), 0);
						if(nRecv > 0)    
						{
							szText[nRecv] = '/0';
							printf("%s /n", szText);
						}
					}
				}
				else if(event.lNetworkEvents & FD_CLOSE)  // ����FD_CLOSE�¼�
				{
					if(event.iErrorCode[FD_CLOSE_BIT] == 0)
					{
						::closesocket(sockArray[i]);
						for(int j=i; j<nEventTotal-1; j++)
						{
							sockArray[j] = sockArray[j+1];
							sockArray[j] = sockArray[j+1]; 
						}
						nEventTotal--;
					}
				}
				else if(event.lNetworkEvents & FD_WRITE)  // FD_WRITE�¼�������⣬���潫�ص�˵����
				{
				}
			}
		}
	}
	return 0;
}