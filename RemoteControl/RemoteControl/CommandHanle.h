#include <map>
#include <list>
#include "Pkt.h"

class CCommandHanle
{
public:
	CCommandHanle();
	~CCommandHanle();
	BOOL ExecuteCommand(int nCmd, std::list<CPkt>& lstPkt, CPkt& inPkt);
	static void RunCommand(void* arg, BOOL status, std::list<CPkt>& lstPkt, CPkt& inPkt);
protected:
	typedef BOOL (CCommandHanle::*CMDFUNC)(std::list<CPkt>& lstPkt, CPkt& inPkt);
	std::map<int, CMDFUNC> m_mapFunction;	//  nCmd -> function
	CLockDialog dlg;
	unsigned int TID;

protected:
	BOOL MakeDriverInfo(std::list<CPkt>& lstPkt, CPkt& inPkt);		//  1
	BOOL MakeDirectoryInfo(std::list<CPkt>& lstPkt, CPkt& inPkt);	//  2
	BOOL RunFile(std::list<CPkt>& lstPkt, CPkt& inPkt);				//  3
	BOOL DownloadFile(std::list<CPkt>& lstPkt, CPkt& inPkt);		//  4
	BOOL MouseEvent(std::list<CPkt>& lstPkt, CPkt& inPkt);			//  5
	BOOL SendScreen(std::list<CPkt>& lstPkt, CPkt& inPkt);			//  6
	BOOL LockMachine(std::list<CPkt>& lstPkt, CPkt& inPkt);			//  7
	BOOL UnLockMachine(std::list<CPkt>& lstPkt, CPkt& inPkt);		//  8
	BOOL DeleteLocalFile(std::list<CPkt>& lstPkt, CPkt& inPkt);		//  9
	BOOL ConnectTest(std::list<CPkt>& lstPkt, CPkt& inPkt);			//  7777
	static unsigned int __stdcall  ThreadLoackMachine(void* arg);
	void ThreadProc();
};

