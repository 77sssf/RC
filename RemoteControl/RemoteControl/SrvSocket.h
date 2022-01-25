#pragma once

#include "pch.h"
#include "framework.h"

const int BUF_SIZ = 1024;

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();
	BOOL initSocket() {
		m_sockSrv = socket(PF_INET, SOCK_STREAM, 0);
		if (m_sockSrv == INVALID_SOCKET) {
			//  TODO : ��ʼ��ʧ��
			return FALSE;
		}

		//  �����IP��Ϣ
		SOCKADDR_IN addrSrv = {};
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_addr.S_un.S_addr = INADDR_ANY;
		addrSrv.sin_port = htons(7070);

		if (bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
			return FALSE;
		}

		if (listen(m_sockSrv, 1) == SOCKET_ERROR) {
			return FALSE;
		}

		return TRUE;
	}

	BOOL acceptClient() {
		SOCKADDR_IN addrCli = {};
		int len = sizeof(SOCKADDR);
		m_sockCli = accept(m_sockSrv, (SOCKADDR*)&addrCli, &len);
		if (m_sockCli == INVALID_SOCKET) {
			return FALSE;
		}
		return TRUE;
	}

	BOOL dealRequest() {
		if (m_sockCli == INVALID_SOCKET) {
			return FALSE;
		}

		char buf[BUF_SIZ] = {};

		while (true) {
			int len = recv(m_sockCli, buf, BUF_SIZ, 0);

			if (len <= 0) {
				return FALSE;
			}

			//  TODO : ��������

			
		}
	}

	BOOL sendACK(const char* pData, int nSize) {
		return send(m_sockCli, pData, nSize, 0);
	}

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
};

//  extern CSrvSocket srv;  //  main����֮ǰ��ʼ��, main����֮���ͷ���Դ.
//  Ǳ������ : ���ٴα�����Ϊ�ֲ��������ٴε��ù��������.
//  ��֤ǿ�� : �淶, �﷨, Ӳ��
//  ����