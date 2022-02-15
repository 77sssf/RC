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
	BOOL initSocket();

	BOOL dealRequest();

	

	CPkt getPkt() const;
	void UpdateAddress(const int nIP, const int nPort);
	BOOL closeSock();
	BOOL sendPkt(const CPkt&, std::list<CPkt>&, BOOL = TRUE);
private:
	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);
	static void CCliSocket::threadEntryForSocket(void* arg);

	void CCliSocket::threadSocket();

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

	std::list<CPkt> m_listSend;
	std::map<HANDLE, std::list<CPkt>&> m_mapAck;
	std::map<HANDLE, BOOL> m_mapAutoClose;
	bool m_bAutoClose;
	int m_nIP;
	int m_nPort;
	static CHelper m_helper;
	SOCKET m_sockCli;
	CPkt m_pkt;
	std::vector<char> m_buf;
};