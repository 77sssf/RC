#pragma once

#include <map>

class CCommandHanle
{
public:
	CCommandHanle();
	~CCommandHanle();
	BOOL ExecuteCommand(int nCmd);

protected:
	typedef BOOL (CCommandHanle::*CMDFUNC)();
	std::map<int, CMDFUNC> m_mapFunction;	//  nCmd -> function
	CLockDialog dlg;
	unsigned int TID;

protected:
	BOOL MakeDriverInfo();		//  1
	BOOL MakeDirectoryInfo();	//  2
	BOOL RunFile();				//  3
	BOOL DownloadFile();		//  4
	BOOL MouseEvent();			//  5
	BOOL SendScreen();			//  6
	BOOL LockMachine();			//  7
	BOOL UnLockMachine();		//  8
	BOOL DeleteLocalFile();		//  9
	BOOL ConnectTest();			//  7777
	static unsigned int __stdcall  ThreadLoackMachine(void* arg);
	void ThreadProc();
};

