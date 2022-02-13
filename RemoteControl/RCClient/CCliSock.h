#pragma once

#include "pch.h"
#include "pkt.h"
#include <string>
#include <vector>
#include "ClientController.h"

const int BUF_SIZ = 4096000;

typedef struct _MOUSEVENT {

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{ 0, 0 } {

	}

	_MOUSEVENT(WORD a, WORD b, POINT c) : nAction(a), nButton(b), ptXY(c) {
	
	}

	WORD nAction;	//  �ƶ�����
	WORD nButton;	//  ���, �Ҽ�
	POINT ptXY;

}MOUSEVENT, * PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(FALSE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  �Ƿ���Ч
	BOOL IsDirectory;
	BOOL HasNext;   //  �Ƿ��к���
}FILEINFO, * PFILEINFO;

class CCliSocket
{
public:
	static CCliSocket* getInstance();
	BOOL initSocket();

	BOOL dealRequest();

	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);

	BOOL getFilePath(std::string&);
	BOOL getMouseEvent(MOUSEVENT&);
	CPkt getPkt() const;
	void UpdateAddress(const int nIP, const int nPort);
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
	 
	int m_nIP;
	int m_nPort;
	static CHelper m_helper;
	SOCKET m_sockCli;
	CPkt m_pkt;
	std::vector<char> m_buf;
};