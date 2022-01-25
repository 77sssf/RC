#include "pch.h"
#include "SrvSocket.h"

CSrvSocket::CSrvSocket() : m_sockSrv(INVALID_SOCKET), m_sockCli(INVALID_SOCKET) {
	printf("CSrvSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("无法初始化套接字, 请检查网络环境"), TEXT("初始化错误"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}

CSrvSocket::CSrvSocket(const CSrvSocket& rhs) {

}

// CSrvSocket& CSrvSocket::operator=(const CSrvSocket& rhs) {
// 	
// }

CSrvSocket::~CSrvSocket() {
	printf("~CSrvSock()\n");
	closesocket(m_sockSrv);
	WSACleanup();
}

CSrvSocket* CSrvSocket::getInstance() {	//  静态函数
	if (m_instance == NULL) {
		m_instance = new CSrvSocket;
	}
	return m_instance;
}

BOOL CSrvSocket::InitWSA() {
	//  TODO : 1. socket
			//  初始化套接字
	WSADATA data = {};
	if (WSAStartup(MAKEWORD(1, 1), &data)) {  //  TODO : 返回值处理
		return FALSE;
	}
	return TRUE;
}


void CSrvSocket::releaseInstance() {
	if (m_instance) {
		CSrvSocket* tmp = m_instance;
		m_instance = NULL;
		delete tmp;
	}
}

CSrvSocket::CHelper::CHelper() {
	CSrvSocket::getInstance();
}

CSrvSocket::CHelper::~CHelper() {
	CSrvSocket::releaseInstance();
}

CSrvSocket* CSrvSocket::m_instance = NULL;

CSrvSocket::CHelper CSrvSocket::m_helper;	//  definition