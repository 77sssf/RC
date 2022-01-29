#pragma once

#include "pch.h"
#include "pkt.h"
#include <string>
#include <vector>

const int BUF_SIZ = 4096;

typedef struct _MOUSEVENT {

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{ 0, 0 } {

	}

	WORD nAction;	//  ÒÆ¶¯»òµã»÷
	WORD nButton;	//  ×ó¼ü, ÓÒ¼ü
	POINT ptXY;

}MOUSEVENT, * PMOUSEVENT;

class CCliSocket
{
public:
	static CCliSocket* getInstance();
	BOOL initSocket(const std::string&);

	BOOL dealRequest();

	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);

	BOOL getFilePath(std::string&);
	BOOL getMouseEvent(MOUSEVENT&);
	WORD getCmd() const;

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