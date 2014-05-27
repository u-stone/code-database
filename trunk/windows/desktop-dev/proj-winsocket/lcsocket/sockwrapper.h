#pragma once

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>


//////////////////////////////MACRO///////////////////////////////////////

#define ERR_SOCKET_STARTUP "startup error"
//////////////////////////////////////////////////////////////////////////

class tcpstream{
public:
	tcpstream();
	~tcpstream();

	/**
	param:
	addr 服务器的IP地址xx.xx.xx.xx，或者是主机名
	port 服务器端口号
	*/
	bool setaddr(char* addr, char* port);
	
	/**
	下面三个函数一旦返回false，就需要重新调用conn函数，来链接服务器。
	*/
	bool read(bool bStop = false);
	bool write(bool bStop = false);
	bool heartbeat();

	void clearbuf(bool bBufRecv = true);

// 	bool isBlocked(){
// 		return mbBlocked;
// 	}
private:
	bool conn();
	bool cutoff();
	
private:
	char mBufRec[1024];
	char mBufSend[1024];
	struct addrinfo mAddr;
	struct addrinfo* mpAddrs;
	SOCKET mConSocket;
	//bool mbBlocked;
};

/**
note
    负责WinSocket的初始化和注销
*/
class netjob
{
public:
	netjob(void){
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (ret != 0)
			throw ERR_SOCKET_STARTUP;
	}
	~netjob(void){
		WSACleanup();
	}
};

