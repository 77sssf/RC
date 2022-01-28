#pragma once

#include "pch.h"
#include "framework.h"

#pragma pack(push)
#pragma pack(1)

const int BUF_SIZ = 4096;

class CPacket {
public:

	CPacket();
	CPacket(WORD, const BYTE*, size_t);
	CPacket(const CPacket& rhs);
	CPacket& operator=(const CPacket& rhs);
	CPacket(const BYTE* pData, size_t& nSize);
	~CPacket();

	
	
	WORD getCmd()const;
	DWORD getLength()const;
	const char* getData() const;
	std::string getStrData() const;

private:

	void calcData();

private:

	WORD sHead;	//  包头固定FFFE
	DWORD nLength;	//  包长度(从控制命令开始到和校验结束)
	WORD sCmd;
	std::string strData;
	std::string strRes;
	WORD sSum;	//  和校验值

};

#pragma pack(pop)

typedef struct _MOUSEVENT{

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{0, 0} {
	
	}

	WORD nAction;	//  移动或点击
	WORD nButton;	//  左键, 右键
	POINT ptXY;

}MOUSEVENT, *PMOUSEVENT;

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();
	BOOL initSocket();

	BOOL acceptClient();

	BOOL dealRequest();

	BOOL sendACK(const char* pData, int nSize);
	BOOL sendACK(const CPacket&);
	
	BOOL getFilePath(std::string& filePath);
	BOOL getMouseEvent(MOUSEVENT& mouse);

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
