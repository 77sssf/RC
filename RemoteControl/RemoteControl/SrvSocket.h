#pragma once

#include "pch.h"
#include "framework.h"
#include "Pkt.h"

const int BUF_SIZ = 4096;

typedef struct _MOUSEVENT{

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{0, 0} {
	
	}

	WORD nAction;	//  移动或点击
	WORD nButton;	//  左键, 右键
	POINT ptXY;

}MOUSEVENT, *PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(TRUE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  是否有效
	BOOL IsDirectory;
	BOOL HasNext;   //  是否还有后续
}FILEINFO, * PFILEINFO;

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();

	BOOL initSocket();
	BOOL acceptClient();
	BOOL dealRequest();

	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);
	
	BOOL getFilePath(std::string&);
	BOOL getMouseEvent(MOUSEVENT&);
	BOOL closeClient();

private:
	
	CSrvSocket();
	~CSrvSocket();
	CSrvSocket(const CSrvSocket&);
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
	CPkt m_pkt;
};

//  extern CSrvSocket srv;  //  main函数之前初始化, main函数之后释放资源.
//  潜在问题 : 若再次被声明为局部变量将再次调用构造和析构.
//  保证强度 : 规范, 语法, 硬件
//  单例
