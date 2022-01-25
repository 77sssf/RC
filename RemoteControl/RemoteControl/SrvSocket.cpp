#include "pch.h"
#include "SrvSocket.h"

CSrvSocket::CSrvSocket() : m_sockSrv(INVALID_SOCKET), m_sockCli(INVALID_SOCKET) {
	printf("CSrvSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("�޷���ʼ���׽���, �������绷��"), TEXT("��ʼ������"), MB_OK | MB_ICONERROR);
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

CSrvSocket* CSrvSocket::getInstance() {	//  ��̬����
	if (m_instance == NULL) {
		m_instance = new CSrvSocket;
	}
	return m_instance;
}

BOOL CSrvSocket::InitWSA() {
	//  TODO : 1. socket
			//  ��ʼ���׽���
	WSADATA data = {};
	if (WSAStartup(MAKEWORD(1, 1), &data)) {  //  TODO : ����ֵ����
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