#pragma once

#define WM_SEND_PACKET	(WM_USER + 1)
#define WM_SEND_DATA	(WM_USER + 2)
#define WM_SHOW_STATUS	(WM_USER + 3)
#define WM_SHOW_MONITOR	(WM_USER + 4)
#define WM_SEND_MESSAGE	(WM_USER + 0x1000)

#include "Tool.h"
#include "CCliSock.h"
#include "MonitorDlg.h"
#include "RCClientDlg.h"
#include "StatusDlg.h"
#include <map>

class CClientController
{
public:
	typedef LRESULT(CClientController::* MSGFUNC)(UINT, WPARAM, LPARAM);
	static CClientController* getInstance();
	BOOL Invoke(CWnd*& pMainWnd);
	BOOL InitController();
	static void releaseInstance();
	
	LRESULT SendMessage(MSG& msg);	//  通过等待函数, SendMessage确保消息被执行完成了才会返回

	LRESULT OnSendPacket(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendData(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowStatus(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowMonitor(UINT nMsg, WPARAM wParam, LPARAM lParam);
	
	void UpdateAddress(const int nIP, const int nPort);

	BOOL DealRequest();

	BOOL closeSock();

	BOOL SendPacket(const CPkt& pkt);

	BOOL SendCommandPacket(const int nCmd, BOOL autoClose = TRUE, const BYTE* pData = NULL, const int nLength = 0);

	int GetImage(CImage& img);
	int DownloadFile(CString remoteFilePath, CString localFilePath);
	int OpenMonitor();
private:
	CClientController();
	~CClientController();

	static unsigned __stdcall threadEntry(void* arg);

	void threadProc();

	static void threadEntryForDownloadFile(void* arg);
	void threadDownloadFile();

	static void ThreadEntryForWatchData(void* args);
	void ThreadWatchData();

private:
	typedef struct _MSGINFO{
		_MSGINFO(const MSG& m) {
			result = 0;
			memcpy(&msg, &m, sizeof(MSG));
		}

		_MSGINFO() {
			result = 0;
			memset(&msg, 0, sizeof(MSG));
		}

		_MSGINFO(const _MSGINFO& rhs) {
			result = rhs.result;
			memcpy(&msg, &rhs.msg, sizeof(MSG));
		}

		_MSGINFO& operator=(const _MSGINFO& rhs) {
			if (this == &rhs) {
				return *this;
			}
			result = rhs.result;
			memcpy(&msg, &rhs.msg, sizeof(MSG));
		}

		MSG msg;
		LRESULT result;

	}MSGINFO;

	class CHelper {
	public:
		CHelper();
		~CHelper();
	};

	static CClientController* m_instance;
	static CHelper m_helper;
	static std::map<UINT, MSGFUNC> m_mapFunction;

	CMonitorDlg m_monitorDlg;
	CRCClientDlg m_clientDlg;
	CStatusDlg m_statusDlg;
	HANDLE m_hThread;
	HANDLE m_hThreadDownload;
	HANDLE m_hThreadMonitor;
	CString m_remoteFilePath;
	CString m_localFilePath;

	BOOL m_IsDlgClosed;

	unsigned int m_nThreadID;
};