#include "pch.h"
#include "CCliSock.h"


CCliSocket* CCliSocket::m_instance = NULL;
CCliSocket::CHelper CCliSocket::m_helper;

std::string GetErrorInfo(int wsaErrCode) {
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

CCliSocket::CCliSocket() : m_sockCli(INVALID_SOCKET) {
	printf("CSrvSock()\n");

	if (!InitWSA()) {
		MessageBox(NULL, TEXT("无法初始化套接字, 请检查网络环境"), TEXT("初始化错误"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}

CCliSocket::CCliSocket(const CCliSocket&) {
	
}

CCliSocket::~CCliSocket() {
	printf("~CSrvSock()\n");
	closesocket(m_sockCli);
	WSACleanup();
}

CCliSocket* CCliSocket::getInstance() {

}

void CCliSocket::releaseInstance() {
	
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

BOOL CCliSocket::initSocket(const std::string& IP_ADDR) {

	m_sockCli = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sockCli == INVALID_SOCKET) {
		//  TODO : 初始化失败
		return FALSE;
	}

	//  服务端IP信息
	SOCKADDR_IN addrSrv = {};
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(IP_ADDR.c_str());
	addrSrv.sin_port = htons(7070);

	if (addrSrv.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(TEXT("服务器IP地址不存在"));
		return FALSE;
	}

	if (connect(m_sockCli, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		AfxMessageBox(TEXT("连接服务器失败"));
		TRACE(TEXT("连接失败 : %d %s\r\n", WSAGetLastError(), GetErrorInfo(WSAGetLastError()).c_str()));
		return FALSE;
	}

	return TRUE;
}

BOOL CCliSocket::dealRequest() {
	
}