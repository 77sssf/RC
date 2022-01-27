#pragma once

#include "pch.h"
#include "framework.h"

const int BUF_SIZ = 4096;

class CPacket {
public:

	CPacket();
	CPacket(const CPacket& rhs);
	CPacket& operator=(const CPacket& rhs);
	CPacket(const BYTE* pData, size_t& nSize);
	~CPacket();
	WORD getCmd();

private:

	WORD sHead;	//  包头固定FFFE
	DWORD nLength;	//  包长度(从控制命令开始到和校验结束)
	WORD sCmd;
	std::string strData;
	WORD sSum;	//  和校验值
};

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();
	BOOL initSocket();

	BOOL acceptClient();

	BOOL dealRequest();

	BOOL sendACK(const char* pData, int nSize);
	
private:
	
	CSrvSocket();
	~CSrvSocket();
	CSrvSocket(const CSrvSocket& rhs);
	//CSrvSocket& operator=(const CSrvSocket& rhs);

	BOOL InitWSA();

	static CSrvSocket* m_instance;
	
	static void releaseInstance();

	class CHelper {
	public:
		CHelper();
		~CHelper();
	};

	static CHelper m_helper;
	SOCKET m_sockSrv;
	SOCKET m_sockCli;
	CPacket m_pkt;
};

//  extern CSrvSocket srv;  //  main函数之前初始化, main函数之后释放资源.
//  潜在问题 : 若再次被声明为局部变量将再次调用构造和析构.
//  保证强度 : 规范, 语法, 硬件
//  单例
