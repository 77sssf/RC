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

// std::string GetErrInfo(int wsaErrCode) {
// 	std::string res;
// 	LPVOID lpMsgBuf = NULL;
// 	FormatMessage(
// 		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
// 		NULL, 
// 		wsaErrCode, 
// 		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
// 		(LPTSTR)lpMsgBuf, 
// 		0, 
// 		NULL
// 	);
// 	res = (char*)lpMsgBuf;	//  宽字节转char* 应该会有问题
// 	LocalFree(lpMsgBuf);
// 	return res;
// }

CCliSocket::CCliSocket() 
	: m_nIP(INADDR_ANY)
	, m_nPort(0)	//  无效IP和PORT
	, m_sockCli(INVALID_SOCKET)
	, m_pkt()
	, m_bAutoClose(TRUE) {
	//printf("CliSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("无法初始化套接字, 请检查网络环境"), TEXT("初始化错误"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_buf.resize(BUF_SIZ);

	if (m_sockCli == INVALID_SOCKET) {
		_beginthread(threadEntryForSocket, 0, this);	//  保证线程只启动一次
	}
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

BOOL CCliSocket::initSocket() {
	
	if (m_sockCli != INVALID_SOCKET) {
		closeSock();
	}

	m_sockCli = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockCli == INVALID_SOCKET) {
		TRACE(TEXT("socket()失败 : %d \r\n"), WSAGetLastError());
		exit(-1);
		return FALSE;
	}

	//  服务端IP信息
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = htonl(m_nIP);

	addrSrv.sin_port = htons(m_nPort);

	if (addrSrv.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(TEXT("服务器IP地址不存在"));
		return FALSE;
	}

	if (connect(m_sockCli, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		TRACE(TEXT("连接服务器失败 : %d \r\n"), WSAGetLastError());
		exit(-1);
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

	//  WORD	包头 FFFE
	//  DWORD	长度
	//  WORD	命令
	//  string	数据
	//  WORD	校验

	static int idx = 0;
	while (true) {
		int len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);
		if (idx <= 0 && len <= 0) {
			//  0 : 连接断开
			//  1 : SOCKET_ERROR
			return FALSE;
		}

		TRACE(_T("recv len : %u, idx : %u\r\n"), len, idx);

		idx += len;
		len = idx;	// 有bug, 大bug, 不能确定, 两个线程使用一个buf
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
	//  类中含有string类型成员变量
	//  不能(const char*)&tpkt
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

	char* buf = m_buf.data();
	int idx = 0;
	while (TRUE) {
		TRACE("m_listSend.size() : %d \r\n", m_listSend.size());
		if (m_listSend.size() > 0) {
			initSocket();
			TRACE("m_client = %d\r\n", m_sockCli);
			CPkt& head = m_listSend.front();	//  head 客服端发送的包
			sendACK(head);
			
			std::map<HANDLE, BOOL>::iterator it0 = m_mapAutoClose.find(head.m_hEvnet);
			std::map<HANDLE, std::list<CPkt>&>::iterator it1 = m_mapAck.find(head.m_hEvnet);
			
			if (it0 == m_mapAutoClose.end() || it1 == m_mapAck.end()) {
				m_listSend.pop_front();
				SetEvent(head.m_hEvnet);
				break;
			}

			do {
				int len = recv(m_sockCli, buf + idx, BUF_SIZ - idx, 0);
				TRACE("len = %d, idx = %d\r\n", len, idx);
				if (len <= 0 && idx <= 0) {
					TRACE("解析完成\r\n");
					SetEvent(head.m_hEvnet);	//  连接关闭， 数据全部处理完
					closeSock();
					break;
				}

				idx += len;
				size_t size = (size_t)idx;
				CPkt pkt((BYTE*)buf, size, head.m_hEvnet);	//  从服务器接收的包

				if (size > 0) {
					//  TODO : 通知对应cmd
					memmove(buf, buf + size, BUF_SIZ - size);
					idx -= size;
					it1->second.push_back(pkt);
					TRACE("thread map size = %d\r\n", m_mapAck.size());
					if (it0->second == TRUE) {
						SetEvent(head.m_hEvnet);
						break;
					}
				}
				TRACE("socket thread ID : %d\r\n", GetCurrentThreadId());
				TRACE("lstRecved.size() : %d \r\n", it1->second.size());
			} while (it0->second == FALSE);

			m_listSend.pop_front();
			closeSock();
		}
		else {
			Sleep(30);
		}
	}
	return;
}

BOOL CCliSocket::sendPkt(const CPkt& pkt, std::list<CPkt>& lstRecved, BOOL autoClose) {
	m_listSend.push_back(pkt);
	m_mapAutoClose.insert(std::pair<HANDLE, BOOL>(pkt.m_hEvnet, autoClose));
	m_mapAck.insert(std::pair<HANDLE, std::list<CPkt>&>(pkt.m_hEvnet, lstRecved));

	WaitForSingleObject(pkt.m_hEvnet, INFINITE);	//  Wait ... 

	std::map<HANDLE, BOOL>::iterator it0 = m_mapAutoClose.find(pkt.m_hEvnet);
	std::map<HANDLE, std::list<CPkt>&>::iterator it1 = m_mapAck.find(pkt.m_hEvnet);

	TRACE("triger time : %lld\r\n", GetTickCount64());
	TRACE("main thread ID : %d\r\n", GetCurrentThreadId());
	TRACE("main map size = %d\r\n", m_mapAck.size());
	TRACE("lstRecved.size() : %d \r\n", lstRecved.size());
	if (it1 == m_mapAck.end()) {
		return FALSE;
	}
	m_mapAutoClose.erase(it0);
	m_mapAck.erase(it1);
	return TRUE;
}
