// server.cpp : 定义控制台应用程序的入口点。
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
	// 事件句柄和套接字句柄
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET  sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;
	USHORT nPort = 4567; // 监听端口号
	// 创建监听套接字
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
	// 创建事件对象，并关联到新的套接字
	WSAEVENT event = ::WSACreateEvent();
	::WSAEventSelect(sListen, event, FD_ACCEPT|FD_CLOSE);
	// 添加到列表中
	eventArray[nEventTotal] = event;
	sockArray[nEventTotal] = sListen; 
	nEventTotal++;
	char szText[512];
	// 处理网络事件
	while(TRUE)
	{
		// 在所有对象上等待
		int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		// 确定事件的状态
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		// WSAWaitForMultipleEvents总是返回所有事件对象的最小值，为了确保所有的事件对象得到执行的机会，对 大于nIndex的事件对象进行轮询，使其得到执行的机会。
		for(int i=nIndex; i<nEventTotal; i++) 
		{
			nIndex = ::WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 0, FALSE);
			if(nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				// 获取到来的通知消息，WSAEnumNetworkEvents函数会自动重置受信事件
				WSANETWORKEVENTS event;
				::WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event);
				if(event.lNetworkEvents & FD_ACCEPT)    // 处理FD_ACCEPT事件
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
						// 添加到列表中
						eventArray[nEventTotal] = event;
						sockArray[nEventTotal] = sNew; 
						nEventTotal++;
					}
				}
				else if(event.lNetworkEvents & FD_READ)   // 处理FD_READ事件
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
				else if(event.lNetworkEvents & FD_CLOSE)  // 处理FD_CLOSE事件
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
				else if(event.lNetworkEvents & FD_WRITE)  // FD_WRITE事件破难理解，下面将重点说明。
				{
				}
			}
		}
	}
	return 0;
}