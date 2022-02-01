#pragma once

#include "pch.h"
#include "pkt.h"
#include <string>
#include <vector>

const int BUF_SIZ = 409600;

typedef struct _MOUSEVENT {

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{ 0, 0 } {

	}

	_MOUSEVENT(WORD a, WORD b, POINT c) : nAction(a), nButton(b), ptXY(c) {
	
	}

	WORD nAction;	//  移动或点击
	WORD nButton;	//  左键, 右键
	POINT ptXY;

}MOUSEVENT, * PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(FALSE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  是否有效
	BOOL IsDirectory;
	BOOL HasNext;   //  是否还有后续
}FILEINFO, * PFILEINFO;

class CCliSocket
{
public:
	static CCliSocket* getInstance();
	BOOL initSocket(const int, const int);

	BOOL dealRequest();

	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);

	BOOL getFilePath(std::string&);
	BOOL getMouseEvent(MOUSEVENT&);
	CPkt getPkt() const;

	BOOL closeSock();
private:

	CCliSocket();
	~CCliSocket();
	CCliSocket(const CCliSocket&);
	//CSrvSocket& operator=(const CSrvSocket& rhs);

	BOOL InitWSA();

	static CCliSocket* m_instance;

	static void releaseInstance();

	class CHelper {
	public:
		CHelper();
		~CHelper();
	};

	static CHelper m_helper;
	SOCKET m_sockCli;
	CPkt m_pkt;
	std::vector<char> m_buf;
};