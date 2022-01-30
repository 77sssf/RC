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
	res = (char*)lpMsgBuf;	//  宽字节转char* 应该会有问题
	LocalFree(lpMsgBuf);
	return res;
}

CCliSocket::CCliSocket() : m_sockCli(INVALID_SOCKET), m_pkt() {
	//printf("CliSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("无法初始化套接字, 请检查网络环境"), TEXT("初始化错误"), MB_OK | MB_ICONERROR);
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
			//  初始化套接字
	WSADATA data = {};
	if (WSAStartup(MAKEWORD(1, 1), &data)) {  //  TODO : 返回值处理
		return FALSE;
	}
	return TRUE;
}

BOOL CCliSocket::initSocket(const int nIP, const int nPort) {
	
	if (m_sockCli != INVALID_SOCKET) {
		closeSock();
	}

	m_sockCli = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockCli == INVALID_SOCKET) {
		//  TODO : 初始化失败
		return FALSE;
	}

	//  服务端IP信息
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = htonl(nIP);

	addrSrv.sin_port = htons(nPort);

	if (addrSrv.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(TEXT("服务器IP地址不存在"));
		return FALSE;
	}

	if (connect(m_sockCli, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		AfxMessageBox(TEXT("连接服务器失败"));
		TRACE(TEXT("连接失败 : %d %s\r\n"), WSAGetLastError(), GetErrInfo(WSAGetLastError()).c_str());
		return FALSE;
	}

	return TRUE;
}

int CCliSocket::dealRequest() {

	//char buf[BUF_SIZ] = {};		//  local variable

	char* buf = m_buf.data();
	memset(buf, 0, BUF_SIZ);

	//  包头 FFFE
	//  长度
	//  命令
	//  数据
	//  校验
	int idx = 0;
	while (true) {
		int len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);

		size_t tlen = len;

		if (len <= 0) {
			return FALSE;
		}

		//  TODO : 处理请求
		idx = len;

		m_pkt = CPkt((BYTE*)buf, tlen);	//  重载CPacket类的赋值运算符
		if (tlen > 0) {
			memmove(buf, buf + len, BUF_SIZ - tlen);
			idx -= tlen;
			return m_pkt.getCmd();
		}
	}
}

BOOL CCliSocket::sendACK(const char* pData, int nSize) {
	return send(m_sockCli, pData, nSize, 0);
}

BOOL CCliSocket::sendACK(const CPkt& tpkt) {
	//  类中含有string类型成员变量
	//  不能(const char*)&tpkt
	//const char* p = tpkt.getData();
	return send(m_sockCli, tpkt.getData(), tpkt.getLength() + 2 + 4, 0);
}


CPkt CCliSocket::getPkt() const {
	return m_pkt;
}

BOOL CCliSocket::closeSock() {
	closesocket(m_sockCli);
	m_sockCli = INVALID_SOCKET;
	return TRUE;
}
