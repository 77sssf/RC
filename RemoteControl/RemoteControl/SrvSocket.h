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

	const char* getData();
	WORD getCmd()const;
	DWORD getLength()const;
	

private:

	WORD sHead;	//  ��ͷ�̶�FFFE
	DWORD nLength;	//  ������(�ӿ������ʼ����У�����)
	WORD sCmd;
	std::string strData;
	WORD sSum;	//  ��У��ֵ

};

#pragma pack(pop)

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();
	BOOL initSocket();

	BOOL acceptClient();

	BOOL dealRequest();

	BOOL sendACK(const char* pData, int nSize);
	BOOL sendACK(const CPacket&);
	
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

//  extern CSrvSocket srv;  //  main����֮ǰ��ʼ��, main����֮���ͷ���Դ.
//  Ǳ������ : ���ٴα�����Ϊ�ֲ��������ٴε��ù��������.
//  ��֤ǿ�� : �淶, �﷨, Ӳ��
//  ����
