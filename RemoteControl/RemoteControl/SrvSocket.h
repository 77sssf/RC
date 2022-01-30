#pragma once

#include "pch.h"
#include "framework.h"
#include "Pkt.h"

const int BUF_SIZ = 4096;

typedef struct _MOUSEVENT{

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{0, 0} {
	
	}

	WORD nAction;	//  �ƶ�����
	WORD nButton;	//  ���, �Ҽ�
	POINT ptXY;

}MOUSEVENT, *PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(TRUE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  �Ƿ���Ч
	BOOL IsDirectory;
	BOOL HasNext;   //  �Ƿ��к���
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

//  extern CSrvSocket srv;  //  main����֮ǰ��ʼ��, main����֮���ͷ���Դ.
//  Ǳ������ : ���ٴα�����Ϊ�ֲ��������ٴε��ù��������.
//  ��֤ǿ�� : �淶, �﷨, Ӳ��
//  ����
