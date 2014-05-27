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
	addr ��������IP��ַxx.xx.xx.xx��������������
	port �������˿ں�
	*/
	bool setaddr(char* addr, char* port);
	
	/**
	������������һ������false������Ҫ���µ���conn�����������ӷ�������
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
    ����WinSocket�ĳ�ʼ����ע��
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

