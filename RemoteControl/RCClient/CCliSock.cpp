#include "pch.h"
#include "CCliSock.h"


CCliSocket* CCliSocket::m_instance = NULL;
CCliSocket::CHelper CCliSocket::m_helper;

CCliSocket* CCliSocket::getInstance() {
	if (m_instance == NULL) {
		m_instance = new CCliSocket;
	}
	return m_instance;
}

void CCliSocket::releaseInstance() {
	delete m_instance;
}

CCliSocket::CHelper::CHelper() {
	CCliSocket::getInstance();
}

CCliSocket::CHelper::~CHelper() {
	CCliSocket::releaseInstance();
}

std::string GetErrInfo(int wsaErrCode) {
	std::string res;
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		NULL, 
		wsaErrCode, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)lpMsgBuf, 
		0, 
		NULL
	);
	res = (char*)lpMsgBuf;	//  ���ֽ�תchar* Ӧ�û�������
	LocalFree(lpMsgBuf);
	return res;
}

CCliSocket::CCliSocket() 
	: m_nIP(INADDR_ANY)
	, m_nPort(0)	//  ��ЧIP��PORT
	, m_sockCli(INVALID_SOCKET)
	, m_pkt() {
	//printf("CliSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("�޷���ʼ���׽���, �������绷��"), TEXT("��ʼ������"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_buf.resize(BUF_SIZ);
}

CCliSocket::CCliSocket(const CCliSocket&) : m_sockCli(INVALID_SOCKET), m_pkt() {
	
}

CCliSocket::~CCliSocket() {
	printf("~CSrvSock()\n");
	closesocket(m_sockCli);
	WSACleanup();
}


BOOL CCliSocket::InitWSA() {
	//  TODO : 1. socket
			//  ��ʼ���׽���
	WSADATA data = {};
	if (WSAStartup(MAKEWORD(1, 1), &data)) {  //  TODO : ����ֵ����
		return FALSE;
	}
	return TRUE;
}

BOOL CCliSocket::initSocket() {
	
	if (m_sockCli != INVALID_SOCKET) {
		closeSock();
	}

	m_sockCli = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockCli == INVALID_SOCKET) {
		//  TODO : ��ʼ��ʧ��
		return FALSE;
	}

	int opt_val = 1;
	setsockopt(m_sockCli, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

	//  �����IP��Ϣ
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = htonl(m_nIP);

	addrSrv.sin_port = htons(m_nPort);

	if (addrSrv.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(TEXT("������IP��ַ������"));
		return FALSE;
	}

	if (connect(m_sockCli, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		AfxMessageBox(TEXT("���ӷ�����ʧ��"));
		TRACE(TEXT("����ʧ�� : %d %s\r\n"), WSAGetLastError(), GetErrInfo(WSAGetLastError()).c_str());
		return FALSE;
	}

	m_buf.clear();
	m_buf.resize(BUF_SIZ);

	return TRUE;
}

int CCliSocket::dealRequest() {

	//char buf[BUF_SIZ] = {};		//  local variable
	char* buf = m_buf.data();
	//memset(buf, 0, BUF_SIZ);

	//  WORD	��ͷ FFFE
	//  DWORD	����
	//  WORD	����
	//  string	����
	//  WORD	У��

	static int idx = 0;
	while (true) {
		int len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);
		if (idx <= 0 && len <= 0) {
			//  0 : ���ӶϿ�
			//  1 : SOCKET_ERROR
			return FALSE;
		}

		TRACE(_T("recv len : %u, idx : %u\r\n"), len, idx);

		idx += len;
		len = idx;	// ��bug, ��bug, ����ȷ��, �����߳�ʹ��һ��buf
		size_t tot = len;
		m_pkt = CPkt((BYTE*)buf, tot);

		//TRACE(TEXT("cli recv: %d, %s\r\n"), m_pkt.getCmd(), ((PFILEINFO)m_pkt.getStrData().c_str())->szFileName);
		if (len > 0) {
			memmove(buf, buf + tot, BUF_SIZ - tot);
			idx -= tot;
			return m_pkt.getCmd();
		}
	}
	return FALSE;
}

BOOL CCliSocket::sendACK(const char* pData, int nSize) {
	return send(m_sockCli, pData, nSize, 0);
}

BOOL CCliSocket::sendACK(const CPkt& tpkt) {
	//  ���к���string���ͳ�Ա����
	//  ����(const char*)&tpkt
	//const char* p = tpkt.getData();
	return send(m_sockCli, tpkt.getData(), tpkt.getLength() + 2 + 4, 0) > 0 ? TRUE : FALSE;
}


CPkt CCliSocket::getPkt() const {
	return m_pkt;
}

BOOL CCliSocket::closeSock() {
	closesocket(m_sockCli);
	m_sockCli = INVALID_SOCKET;
	return TRUE;
}

void CCliSocket::UpdateAddress(const int nIP, const int nPort) {
	m_nIP = nIP;
	m_nPort = nPort;
}

void CCliSocket::threadEntryForSocket(void* arg) {
	((CCliSocket*)arg)->threadSocket();
	_endthread();
}

void CCliSocket::threadSocket() {

	std::string strBuffer;
	strBuffer.resize(BUF_SIZ);
	char* pBuf = (char*)strBuffer.c_str();
	int idx = 0;
	while (m_sockCli != INVALID_SOCKET) {
		TRACE("m_listSend.size() : %d \r\n", m_listSend.size());
		if (m_listSend.size() > 0) {
			CPkt& head = m_listSend.front();	//  head �ͷ��˷��͵İ�
			sendACK(head);

			auto pr = m_mapAck.insert(std::pair<HANDLE, std::list<CPkt>>(head.m_hEvnet, std::list<CPkt>()));

			std::list<CPkt> lstRecv;
			int len = recv(m_sockCli, pBuf + idx, BUF_SIZ - idx, 0);
			if (len <= 0 && idx <= 0) {
				closeSock();
				continue;
			}

			idx += len;
			size_t size = (size_t)idx;
			CPkt pkt((BYTE*)pBuf, size);	//  �ӷ��������յİ�
			
			if (size > 0) {
				//  TODO : ֪ͨ��Ӧcmd
				pkt.m_hEvnet = head.m_hEvnet;
				pr.first->second.push_back(pkt);
				SetEvent(head.m_hEvnet);

			}
			m_listSend.pop_front();
		}
		closeSock();
	}
}

BOOL CCliSocket::sendPkt(const CPkt& pkt, std::list<CPkt>& lstRecved) {
	m_listSend.push_back(pkt);
	if (m_sockCli == INVALID_SOCKET) {
		initSocket();
		_beginthread(threadEntryForSocket, 0, this);
	}
	WaitForSingleObject(pkt.m_hEvnet, 50);
	std::map<HANDLE, std::list<CPkt>>::iterator itm = m_mapAck.find(pkt.m_hEvnet);
	if (itm == m_mapAck.end()) {
		return FALSE;
	}
	//  ���� lstSend
// 	for (std::list<CPkt>::iterator itl = itm->second.begin(); itl != itm->second.end(); ++itl) {
// 		//  itl : CPkt*
// 
// 	}
	lstRecved = itm->second;
	m_mapAck.erase(itm);
	return TRUE;
}
