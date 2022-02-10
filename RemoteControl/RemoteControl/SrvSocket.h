
#include <vector>
#include <list>
#include "Pkt.h"

const int BUF_SIZ = 4096;

typedef void(*SOCK_CALLBACK)(void* arg, BOOL ret, std::list<CPkt>& lstPkt, CPkt&);

class CSrvSocket
{
public:
	static CSrvSocket* getInstance();

	BOOL initSocket(short sPort = 7070);
	BOOL acceptClient();
	BOOL dealRequest();

	BOOL sendACK(const char*, int);
	BOOL sendACK(const CPkt&);
	
	BOOL getFilePath(std::string&);
	BOOL getMouseEvent(MOUSEVENT&);
	BOOL closeClient();
	BOOL closeServer();

	BOOL Run(SOCK_CALLBACK callback, void* arg, short sPort = 7070);

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

	SOCK_CALLBACK m_callback;
	void* m_arg;
	SOCKET m_sockSrv;
	SOCKET m_sockCli;
	CPkt m_pkt;
	std::vector<char> m_buf;
};

//  extern CSrvSocket srv;  //  main����֮ǰ��ʼ��, main����֮���ͷ���Դ.
//  Ǳ������ : ���ٴα�����Ϊ�ֲ��������ٴε��ù��������.
//  ��֤ǿ�� : �淶, �﷨, Ӳ��
//  ����
