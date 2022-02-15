#include "pch.h"

CSrvSocket* CSrvSocket::m_instance = NULL;
CSrvSocket::CHelper CSrvSocket::m_helper;	//  definition

CSrvSocket* CSrvSocket::getInstance() {	//  ��̬����
	if (m_instance == NULL) {
		m_instance = new CSrvSocket;
	}
	return m_instance;
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

CSrvSocket::CSrvSocket() : m_sockSrv(INVALID_SOCKET), m_sockCli(INVALID_SOCKET), m_pkt() {
	printf("CSrvSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("�޷���ʼ���׽���, �������绷��"), TEXT("��ʼ������"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_buf.resize(BUF_SIZ);
}

CSrvSocket::CSrvSocket(const CSrvSocket& rhs) : m_sockSrv(INVALID_SOCKET), m_sockCli(INVALID_SOCKET), m_pkt(){

}

// CSrvSocket& CSrvSocket::operator=(const CSrvSocket& rhs) {
// 	
// }

CSrvSocket::~CSrvSocket() {
	printf("~CSrvSock()\n");
	closesocket(m_sockSrv);
	WSACleanup();
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

BOOL CSrvSocket::initSocket(short sPort) {
	m_sockSrv = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockSrv == INVALID_SOCKET) {
		//  TODO : ��ʼ��ʧ��
		return FALSE;
	}

	int opt_val = 1;
	setsockopt(m_sockCli, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

	//  �����IP��Ϣ
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = INADDR_ANY;
	addrSrv.sin_port = htons(sPort);

	if (bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		return FALSE;
	}

	if (listen(m_sockSrv, 1) == SOCKET_ERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL CSrvSocket::Run(SOCK_CALLBACK callback, void* arg, short sPort) {
	
	BOOL ret = initSocket(sPort);
	if (ret == FALSE) {
		return ret;
	}
	std::list<CPkt> lstPkt;
	m_callback = callback;
	m_arg = arg;

	while (TRUE) {
		int cnt = 0;
		if (!acceptClient()) {
			if (cnt >= 3) {
				MessageBox(NULL, TEXT("�޷������û�"), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
				exit(0);
			}
			MessageBox(NULL, TEXT("�޷������û�, �Զ�����..."), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
			Sleep(1000);
			cnt++;
			continue;
		}
		ret = dealRequest();
		if (ret > 0) {
			m_callback(m_arg, ret, lstPkt, m_pkt);	//  ����һ��������
			//  send
			while (lstPkt.size() > 0) {
				TRACE("server send size : %d\r\n", lstPkt.front().size());
				sendACK(lstPkt.front());
				lstPkt.pop_front();
			}
		}
		closeClient();
	}

	return TRUE;
}

BOOL CSrvSocket::acceptClient() {
	SOCKADDR_IN addrCli = {};
	int len = sizeof(SOCKADDR);
	m_sockCli = accept(m_sockSrv, (SOCKADDR*)&addrCli, &len);
	if (m_sockCli == INVALID_SOCKET) {
		return FALSE;
	}
	return TRUE;
}

int CSrvSocket::dealRequest() {
	if (m_sockCli == INVALID_SOCKET) {
		return FALSE;
	}

	//char buf[BUF_SIZ] = {};		//  local variable

	char* buf = m_buf.data();

	//memset(buf, 0, BUF_SIZ);
	if (buf == NULL) {
		return FALSE;
	}
	//  ��ͷ FFFE
	//  ����
	//  ����
	//  ����
	//  У��
	static size_t idx = 0;
	while (true) {
		size_t len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);
		if (len <= 0 && idx <= 0) {
			//  0 : ���ӶϿ�
			//  1 : SOCKET_ERROR
			return FALSE;
		}

		idx += len;
		len = idx;

		m_pkt = CPkt((BYTE*)buf, len);

		//TRACE(TEXT("srv recv: %d, %s\r\n"), m_pkt.getCmd(), ((PFILEINFO)m_pkt.getStrData().c_str())->szFileName);

		if (len > 0) {
			memmove(buf, buf + len, BUF_SIZ - len);
			idx -= len;
			return m_pkt.getCmd();
		}
	}
	return FALSE;
}

BOOL CSrvSocket::sendACK(const char* pData, int nSize) {
	return send(m_sockCli, pData, nSize, 0);
}

BOOL CSrvSocket::sendACK(const CPkt& tpkt) {
	//  ���к���string���ͳ�Ա����
	//  ����(const char*)&tpkt
	//const char* p = tpkt.getData();
	return send(m_sockCli, tpkt.getData(), tpkt.getLength() + 2 + 4, 0);
}

BOOL CSrvSocket::getFilePath(std::string& filePath) {
// 	if (m_pkt.getCmd() > 4 && m_pkt.getCmd() < 2) {
// 		return FALSE;
// 	}
	filePath = m_pkt.getStrData();
	return TRUE;
}

BOOL CSrvSocket::getMouseEvent(MOUSEVENT& mouse) {
	if (m_pkt.getCmd() != 5) {
		return FALSE;
	}
	//  �ƶ�, ���(����, ˫��)
	memcpy(&mouse, m_pkt.getStrData().c_str(), sizeof(MOUSEVENT));
	return TRUE;
}

BOOL CSrvSocket::closeClient(/*SOCKET& s*/) {
// 	closesocket(s);
// 	s = INVALID_SOCKET;
	closesocket(m_sockCli);
	m_sockCli = INVALID_SOCKET;
	m_buf.clear();
	m_buf.resize(BUF_SIZ);
	return TRUE;
}

BOOL CSrvSocket::closeServer() {
	closesocket(m_sockSrv);
	m_sockCli = INVALID_SOCKET;
	return TRUE;
}