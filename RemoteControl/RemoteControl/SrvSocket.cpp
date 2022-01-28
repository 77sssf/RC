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

BOOL CSrvSocket::initSocket() {
	m_sockSrv = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockSrv == INVALID_SOCKET) {
		//  TODO : ��ʼ��ʧ��
		return FALSE;
	}

	//  �����IP��Ϣ
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = INADDR_ANY;
	addrSrv.sin_port = htons(7070);

	if (bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		return FALSE;
	}

	if (listen(m_sockSrv, 1) == SOCKET_ERROR) {
		return FALSE;
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

	char* buf = new char[BUF_SIZ];
	memset(buf, 0, BUF_SIZ);

	//  ��ͷ FFFE
	//  ����
	//  ����
	//  ����
	//  У��
	int idx = 0;
	while (true) {
		int len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);

		size_t tlen = len;

		if (len <= 0) {
			return FALSE;
		}

		//  TODO : ��������
		idx = len;

		m_pkt = CPacket((BYTE*)buf, tlen);	//  ����CPacket��ĸ�ֵ�����
		if (tlen > 0) {
			memmove(buf, buf + len, BUF_SIZ - tlen);
			idx -= tlen;
			return m_pkt.getCmd();
		}
	}
}


BOOL CSrvSocket::sendACK(const char* pData, int nSize) {
	return send(m_sockCli, pData, nSize, 0);
}

BOOL CSrvSocket::sendACK(const CPacket& tpkt) {
	//  ���к���string���ͳ�Ա����
	//  ����(const char*)&tpkt
	//const char* p = tpkt.getData();
	return send(m_sockCli, tpkt.getData(), tpkt.getLength() + 2 + 4, 0);
}


CSrvSocket* CSrvSocket::m_instance = NULL;

CSrvSocket::CHelper CSrvSocket::m_helper;	//  definition




CPacket::CPacket() : sHead(0), nLength(0), sCmd(0), sSum(0) {

}

CPacket::CPacket(const BYTE* pData, size_t& nSize) : sHead(0), nLength(0), sCmd(0), sSum(0) {
	size_t i = 0;
	for (i = 0; i < nSize; ++i) {
		if (*((WORD*)(pData + i)) == 0xFFFE) {
			//  �ҵ���ͷ
			i += 2;
			sHead = 0xFFFE;
			break;
		}
	}
	if (i + 4 + 2 + 2 > nSize) {
		nSize = 0;		//  �õ���0�ֽ�
		return;
	}

	nLength = *(DWORD*)(pData + i);
	i += 4;
	if (nLength + i > nSize) {	//  �����ݲ�ȫ
		nSize = 0;
		return;
	}

	sCmd = *(WORD*)(pData + i);
	i += 2;
	if (nLength > 4) {
		strData.resize(nLength - 4);
		memcpy((void*)strData.c_str(), pData + i, nLength - 4);
		i += nLength - 4;
	}

	sSum = *(WORD*)(pData + i);
	i += 2;

	WORD tSum = 0;
	for (size_t j = 0; j < strData.size(); ++j) {
		tSum += (BYTE)strData[j];
	}

	if (sSum == tSum) {
		nSize = i;
	}
	else {
		nSize = 0;
	}

	calcData();
}

CPacket::CPacket(const CPacket& rhs) {
	sHead = rhs.sHead;
	nLength = rhs.nLength;
	sCmd = rhs.sCmd;
	strData = rhs.strData;
	sSum = rhs.sSum;

	calcData();
}

CPacket& CPacket::operator=(const CPacket& rhs) {
	sHead = rhs.sHead;
	nLength = rhs.nLength;
	sCmd = rhs.sCmd;
	strData = rhs.strData;
	sSum = rhs.sSum;
	calcData();
	return *this;
}

CPacket::CPacket(WORD cmd, const BYTE* pData, size_t nSize) : sHead(0), nLength(0), sCmd(0), sSum(0)  {
	sHead = 0xFFFE;
	nLength = nSize + 2 + 2;
	sCmd = cmd;
	if (nSize > 0) {
		strData.resize(nSize);
		memcpy((void*)strData.c_str(), pData, nSize);
	}
	else {
		strData.clear();
	}
 	
	//strData = (const char*)pData;
	for (size_t j = 0; j < strData.size(); ++j) {
		sSum += (BYTE)strData[j];
	}
	calcData();
}

CPacket::~CPacket() {

}

WORD CPacket::getCmd() const {
	return sCmd;
}

DWORD CPacket::getLength() const {
	return nLength;
}

const char* CPacket::getData() const {
	return strRes.c_str();
}

void CPacket::calcData() {
	strRes.resize(nLength + 2 + 4);
	char* pData = (char*)strRes.c_str();
	*(WORD*)pData = sHead;
	pData += 2;
	*(DWORD*)pData = nLength;
	pData += 4;
	*(WORD*)pData = sCmd;
	pData += 2;
	memcpy(pData, strData.c_str(), strData.size());
	pData += strData.size();
	*(WORD*)pData = sSum;

	//return str.c_str();		//  BUG : ���ؾֲ������ĵ�ַ
}

BOOL CSrvSocket::getFilePath(std::string& filePath) {
	if (m_pkt.getCmd() <= 4 && m_pkt.getCmd() >= 2) {
		return FALSE;
	}
	filePath = m_pkt.getStrData();
	return TRUE;
}

std::string CPacket::getStrData() const {
	return strData;
}