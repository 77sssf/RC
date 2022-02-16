
#include "pch.h"


CClientController* CClientController::m_instance = NULL;
CClientController::CHelper CClientController::m_helper;
std::map<UINT, CClientController::MSGFUNC> CClientController::m_mapFunction;

CClientController* CClientController::getInstance() {
	if (m_instance == NULL) {
		m_instance = new CClientController;
	}
	return m_instance;
}

void CClientController::releaseInstance() {

}

CClientController::CClientController() 
	: m_IsDlgClosed(TRUE)
	, m_monitorDlg(&m_clientDlg)
	, m_statusDlg(&m_clientDlg)
	, m_hThread(INVALID_HANDLE_VALUE)
	, m_hThreadDownload(INVALID_HANDLE_VALUE)
	, m_hThreadMonitor(INVALID_HANDLE_VALUE)
	, m_nThreadID(-1) {
	struct {
		UINT nMsg;
		MSGFUNC pFunction;
	}data[] = {
		{WM_SEND_PACKET, &CClientController::OnSendPacket},
		{WM_SEND_DATA, &CClientController::OnSendData},
		{WM_SHOW_STATUS, &CClientController::OnShowStatus},
		{WM_SHOW_MONITOR, &CClientController::OnShowMonitor},
		{0xCCCC, NULL}
	};

	for (int i = 0; data[i].nMsg != 0xCCCC; ++i) {
		m_mapFunction.insert(std::pair<UINT, MSGFUNC>(data[i].nMsg, data[i].pFunction));
	}
}

unsigned __stdcall CClientController::threadEntry(void* arg) {
	((CClientController*)arg)->threadProc();
	_endthreadex(0);
	return 0;
}

void CClientController::threadProc() {
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		//  msg.wParam : MSGINFO
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_SEND_MESSAGE) {
			MSGINFO* pmsg = (MSGINFO*)msg.wParam;
			HANDLE hEvent = (HANDLE)msg.lParam;

			std::map<UINT, MSGFUNC>::iterator it = m_mapFunction.find(pmsg->msg.message);
			if (it == m_mapFunction.end()) {
				pmsg->result = -1;
				SetEvent(hEvent);
				return;
			}
			pmsg->result = (this->*it->second)(pmsg->msg.message, pmsg->msg.wParam, pmsg->msg.lParam);
			SetEvent(hEvent);
		}
		else {
			std::map<UINT, MSGFUNC>::iterator it = m_mapFunction.find(msg.message);
			if (it == m_mapFunction.end()) {
				continue;
			}
			(this->*it->second)(msg.message, msg.wParam, msg.lParam);
		}
	}
}

LRESULT CClientController::SendMessage(MSG& msg) {
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		return FALSE;
	}
	MSGINFO msginfo(msg);
	PostThreadMessage(m_nThreadID, WM_SEND_MESSAGE, (WPARAM)&msginfo, (LPARAM)hEvent);	//  PostThreadMessage����ȴ�
	WaitForSingleObject(hEvent, INFINITE);
	CloseHandle(hEvent);
	return msginfo.result;
}


LRESULT CClientController::OnSendPacket(UINT nMsg, WPARAM wParam, LPARAM lParam) {
	CCliSocket* pSockCli = CCliSocket::getInstance();
	CPkt* pPkt = (CPkt*)wParam;
	//return pSockCli->sendACK(*pPkt);
	return LRESULT();
}

LRESULT CClientController::OnSendData(UINT nMsg, WPARAM wParam, LPARAM lParam) {
	CCliSocket* pSockCli = CCliSocket::getInstance();
	char* pBuffer = (char*)wParam;
	//return pSockCli->sendACK(pBuffer, (int)lParam);
	return LRESULT();
}

LRESULT CClientController::OnShowStatus(UINT nMsg, WPARAM wParam, LPARAM lParam) {
	return m_statusDlg.ShowWindow(SW_SHOW);
}

LRESULT CClientController::OnShowMonitor(UINT nMsg, WPARAM wParam, LPARAM lParam) {
	return m_monitorDlg.DoModal();
}

CClientController::~CClientController() {
	WaitForSingleObject(m_hThread, 50);
}

BOOL CClientController::Invoke(CWnd*& pMainWnd) {	//  ��ClientAPP�б�����
	pMainWnd = &m_clientDlg;
	return m_clientDlg.DoModal();
}

BOOL CClientController::InitController() {
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &CClientController::threadEntry, this, 0, &m_nThreadID);
	m_statusDlg.Create(IDD_DLG_STATUS, &m_clientDlg);
	m_statusDlg.ShowWindow(SW_HIDE);
	return TRUE;
}

CClientController::CHelper::CHelper() {
	//getInstance();  ��ʱAPP��δ����
}
CClientController::CHelper::~CHelper() {
	releaseInstance();
}

void CClientController::UpdateAddress(const int nIP, const int nPort) {
	CCliSocket::getInstance()->UpdateAddress(nIP, nPort);
}

BOOL CClientController::DealRequest() {
	return CCliSocket::getInstance()->dealRequest();
}

BOOL CClientController::closeSock() {
	return CCliSocket::getInstance()->closeSock();
}

BOOL CClientController::SendPacket(const CPkt& pkt, std::list<CPkt>& lstRecved, BOOL autoClose) {
	CCliSocket::getInstance()->sendPkt(pkt, lstRecved, autoClose);
	return TRUE;
}

BOOL CClientController::SendCommandPacket(const int nCmd, BOOL autoClose, const BYTE* pData, const int nLength, std::list<CPkt>* plstRecved) {
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	std::list<CPkt> lstRecved;
	if (plstRecved == NULL) {
		plstRecved = &lstRecved;
	}
	SendPacket(CPkt(nCmd, pData, nLength, hEvent), *plstRecved, autoClose);
	if (plstRecved->size() > 0) {
		
		CloseHandle(hEvent);
		return plstRecved->front().getCmd();
	}
	CloseHandle(hEvent);
	return -1;
}

int CClientController::GetImage(CImage& img) {
	img = m_clientDlg.getImage();
	return TRUE;
}

int CClientController::DownloadFile(CString remoteFilePath, CString localFilePath) {
	
	m_remoteFilePath = remoteFilePath, m_localFilePath = localFilePath;
	m_hThreadDownload = (HANDLE)_beginthread(&CClientController::threadEntryForDownloadFile, 0, this);
	if (WaitForSingleObject(m_hThreadDownload, 0) != WAIT_TIMEOUT) {
		//  û�г�ʱ˵���߳�û������(����ʧ��, ����ʧ��) 
		return -1;
	}

	return 0;
}

void CClientController::threadEntryForDownloadFile(void* arg) {
	((CClientController*)arg)->threadDownloadFile();
	_endthread();
}

void CClientController::threadDownloadFile() {

	FILE* pf = fopen(m_localFilePath, "wb+");
 	if (pf == NULL) {
 		AfxMessageBox(TEXT("���ش��ļ�ʧ��"));
 		return;
 	}

	TRACE(TEXT("Download file path : %s\r\n"), m_remoteFilePath);
	std::list<CPkt> lstRecved;
	int ret = SendCommandPacket(4, FALSE, (BYTE*)(LPCTSTR)m_remoteFilePath, m_remoteFilePath.GetLength(), &lstRecved);
	if (ret < 0) {
		AfxMessageBox(TEXT("��������ʧ��\r\n"));
		TRACE(TEXT("��������ʧ��, return = %d\r\n"), ret);
		return;
	}

	//  ��һ����Ϊ����
	//  ���һ����Ϊ�մ������
	long long nlength = *((long long*)lstRecved.front().getStrData().c_str());
	lstRecved.pop_front(); //  ��һ����һ����������û���ж�lstRecved�Ƿ�Ϊ��
	if (nlength == 0) {
		//  �ļ���ʧ�ܻ򳤶�Ϊ0�ֽ�
		AfxMessageBox(TEXT("�ļ���ʧ�ܻ򳤶�Ϊ0�ֽ�"));
		fclose(pf);
		return;
	}
	//  ��ʼ����
	m_clientDlg.BeginWaitCursor();

	m_statusDlg.m_info.SetWindowText(TEXT("�ļ�������"));
	m_statusDlg.SetActiveWindow();
	m_statusDlg.CenterWindow(&m_clientDlg);
	m_statusDlg.ShowWindow(SW_SHOW);	//  С�ļ����Ƚ����һ������

	long long cnt = 0;

	// -----------------����̺߳���---------------------------------

	while (cnt < nlength) {
// 		ret = CClientController::getInstance()->DealRequest();
// 		if (ret == FALSE) {
// 			AfxMessageBox(TEXT("�Ͽ����ӻ����ȫ���������"));
// 			TRACE(TEXT("�ļ�����ʧ��, return = %d\r\n"), ret);
// 			fclose(pf);
// 			return;
// 		}
		if (lstRecved.size() <= 0) {
			break;
		}
		
		fwrite(lstRecved.front().getStrData().c_str(), 1, lstRecved.front().getStrData().size(), pf);
		cnt += lstRecved.front().getStrData().size();
		if (lstRecved.front().getStrData().size() == 0) {
			// ...
		}
		lstRecved.pop_front();
	}
	// -------------------------------------------------------------

	CClientController::getInstance()->closeSock();
	fclose(pf);
	m_statusDlg.ShowWindow(SW_HIDE);
	m_clientDlg.EndWaitCursor();
	/*	MessageBox(TEXT("�������!"), TEXT("�ɹ�"), MB_OK);*/
}

int CClientController::OpenMonitor() {
	m_IsDlgClosed = FALSE;
	HANDLE m_hThreadMonitor = (HANDLE)_beginthread(CClientController::ThreadEntryForWatchData, 0, this);
	//GetDlgItem(IDC_BTN_MONITOR)->EnableWindow(FALSE);
	//  ����Dlg��ʾ��ͼ
	m_monitorDlg.DoModal();
	m_IsDlgClosed = TRUE;
	WaitForSingleObject(m_hThreadMonitor, INFINITE);
	return TRUE;
}

void CClientController::ThreadEntryForWatchData(void* args) {
	((CClientController*)args)->ThreadWatchData();
	_endthread();
}

void CClientController::ThreadWatchData() {
	Sleep(50);
	ULONGLONG tick = GetTickCount64();
	while (!m_IsDlgClosed) {
		if (GetTickCount64() - tick < 50) {
			Sleep(50 - (DWORD)(GetTickCount64() - tick));
			tick = GetTickCount64();
		}
		while (m_clientDlg.getIsFull() && !m_IsDlgClosed) {
			Sleep(35);
		}
		std::list<CPkt> lstRecved;
		int ret = SendCommandPacket(6, true, NULL, 0, &lstRecved);
		//
		if (lstRecved.size() == 0) {
			continue;
		}
		if (ret < 0) {
			Sleep(500);
		}
		else {
			//ret = pClient->dealRequest();
			if (ret == 6 && !m_clientDlg.getIsFull()) {
				//  TODO : ����CImage
				if (CTool::Bytes2Image(m_clientDlg.getImage(), lstRecved.front().getStrData())) {
					SetIsFull(TRUE);
					lstRecved.pop_front();
				}
				else {
					TRACE(TEXT("failed to load image\r\n"));
				}
			}
		}
	}
}

BOOL CClientController::getIsFull() const {
	return m_clientDlg.getIsFull();
}

BOOL CClientController::SetIsFull(BOOL b) {
	m_clientDlg.SetIsFull(b);
	return TRUE;
}