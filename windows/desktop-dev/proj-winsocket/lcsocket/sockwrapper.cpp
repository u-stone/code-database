#include "StdAfx.h"
#include "sockwrapper.h"


tcpstream::tcpstream()
{
	memset(mBufRec, 0, sizeof(mBufRec));
	memset(&mAddr, 0, sizeof(mAddr));
	mAddr.ai_family = AF_UNSPEC;
	mAddr.ai_socktype = SOCK_STREAM;
	mAddr.ai_protocol = IPPROTO_TCP;
}

tcpstream::~tcpstream()
{
	freeaddrinfo(mpAddrs);
	closesocket(mConSocket);
}

bool tcpstream::setaddr(char* addr, char* port)
{
	int ret = getaddrinfo(addr, port, &mAddr, &mpAddrs);
	if (ret != 0)
		return false;
	return true;
}

bool tcpstream::read(bool bStop)
{
	int ret = 0;
	if (bStop){
		ret = shutdown(mConSocket, SD_RECEIVE);
		if (ret == SOCKET_ERROR)
			return false;
	}
	ret = recv(mConSocket, mBufRec, sizeof(mBufRec), 0);
	if (ret == SOCKET_ERROR){
		closesocket(mConSocket);
		return false;
	}
	mBufRec[ret] = '\0';
	return true;
}

bool tcpstream::heartbeat()
{
	int ret = 0;
	memset(mBufSend, 0, sizeof(mBufSend));
	char echo[] = "keep alive";
	memcpy(mBufSend, echo, sizeof(echo));
	ret = send(mConSocket, mBufSend, sizeof(mBufSend), 0);
	if (ret == SOCKET_ERROR){
		closesocket(mConSocket);
		return false;
	}
	return true;
}

bool tcpstream::write(bool bStop)
{
	int ret = 0;
	if (bStop){
		ret = shutdown(mConSocket, SD_SEND);
		if (ret == SOCKET_ERROR)
			return false;
	}
	ret = send(mConSocket, mBufSend, sizeof(mBufSend), 0);
	if (ret == SOCKET_ERROR){
		closesocket(mConSocket);
		return false;
	}
	return true;
}

void tcpstream::clearbuf(bool bBufRecv)
{
	if (bBufRecv)
		memset(mBufRec, 0, sizeof(mBufRec));
	else
		memset(mBufSend, 0, sizeof(mBufSend));
}

bool tcpstream::conn()
{
	int ret = 0;
	struct addrinfo *ptr;
	for (ptr = mpAddrs; ptr != NULL; ptr = ptr->ai_next){

		mConSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (mConSocket == INVALID_SOCKET)
			return false;

		unsigned long iMode = 1;//set socket no-block
		ret = ioctlsocket(mConSocket, FIONBIO, &iMode);
		if (ret == SOCKET_ERROR){
			closesocket(mConSocket);
			return false;
		}

// 		if (iMode == 0)   mbBlocked = true;
// 		else              mbBlocked = false;

		ret = connect(mConSocket, ptr->ai_addr, ptr->ai_addrlen);
		if (ret == SOCKET_ERROR){
			closesocket(mConSocket);
			mConSocket = INVALID_SOCKET;
			continue;
		}
		
		break;//假定只有一个IP地址，并且直接选中第一个找到的IP。
	}
	if (mConSocket == INVALID_SOCKET)
		return false;

	return true;
}

bool tcpstream::cutoff()
{
	int ret = shutdown(mConSocket, SD_BOTH);
	if (ret == SOCKET_ERROR)
		return false;
	return true;
}
