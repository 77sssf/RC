#include "pch.h"


CCommandHanle::CCommandHanle() : TID(0) {
	struct {
		int nCmd;
		CMDFUNC pFunction;
	}data[] = {
		{1, &CCommandHanle::MakeDriverInfo},
		{2, &CCommandHanle::MakeDirectoryInfo},
		{3, &CCommandHanle::RunFile},
		{4, &CCommandHanle::DownloadFile},
		{5, &CCommandHanle::MouseEvent},
		{6, &CCommandHanle::SendScreen},
		{7, &CCommandHanle::LockMachine},
		{8, &CCommandHanle::UnLockMachine},
		{9, &CCommandHanle::DeleteLocalFile},
		{7777, &CCommandHanle::ConnectTest},
		{-1, NULL}
	};

	for (int i = 0; data[i].nCmd != -1; ++i) {
		m_mapFunction.insert(std::pair<int, CMDFUNC>(data[i].nCmd, data[i].pFunction));
	}
}

CCommandHanle::~CCommandHanle() {

}

void CCommandHanle::RunCommand(void* arg, BOOL status, std::list<CPkt>& lstPkt, CPkt& inPkt) {	//  static
	CCommandHanle* pCmd = (CCommandHanle*)arg;
	if (pCmd->ExecuteCommand(status, lstPkt, inPkt) == FALSE) {
		//
	}
}

BOOL CCommandHanle::ExecuteCommand(int nCmd, std::list<CPkt>& lstPkt, CPkt& inPkt) {
	std::map<int, CMDFUNC>::iterator it = m_mapFunction.find(nCmd);
	if (it == m_mapFunction.end()) {
		return FALSE;
	}
	return (this->*it->second)(lstPkt, inPkt);
}

BOOL CCommandHanle::MakeDriverInfo(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	std::string res;
	//  _chdrive(); //  A�� : 1 B�� : 2 ... Z�� : 26
	for (int i = 1; i <= 26; ++i) {
		if (!_chdrive(i)) {
			//             if (res.size()) {
			//                 res += ',';
			//             }
			res += ('A' + i - 1);
			res += ",";
		}
	}
	lstPkt.push_back(CPkt((WORD)1, (BYTE*)res.c_str(), res.size()));
	return TRUE;
}

BOOL CCommandHanle::MakeDirectoryInfo(std::list<CPkt>& lstPkt, CPkt& inPkt) {

	std::string filePath = inPkt.getStrData();
	//std::list<FILEINFO> lst;

// 	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
// 		//  cmd != 2
// 		return FALSE;
// 	}

	//  �л�_chdir
	if (_chdir(filePath.c_str())) {
		//  �л�ʧ��
		//  û��Ȩ�޻�·������
		return FALSE;
	}

	//  �����ļ�
	_finddata_t fdata = {};
	intptr_t hFile = 0;
	if ((hFile = _findfirst("*", &fdata)) == -1) {
		//  û���ҵ��κ��ļ�, ���Ϳյ�FILEINFO, INVALID = TRUE
		return FALSE;
	}

	do {

		FILEINFO fifo = {};
		fifo.IsInvalid = FALSE;
		fifo.IsDirectory = (fdata.attrib & _A_SUBDIR) ? TRUE : FALSE;
		memcpy(fifo.szFileName, fdata.name, strlen(fdata.name));
		//lst.push_back(fifo);

		//TRACE(TEXT("srv send file: %s\r\n"), fifo.szFileName);
		//Sleep(50);
		lstPkt.push_back(CPkt(2, (BYTE*)&fifo, sizeof(fifo)));

	} while (!_findnext(hFile, &fdata));

	FILEINFO fifo;  //  constructor
	fifo.HasNext = FALSE;
	lstPkt.push_back(CPkt(2, (BYTE*)&fifo, sizeof(fifo)));

	_findclose(hFile);

	//  ������͵��ͻ���
	//  һ���Է���, ����ļ�̫��, �ͻ��˻�ȴ��ܾ�
	//  ��һ����һ��, ����Ƿ������һ���ļ�

	return TRUE;
}

BOOL CCommandHanle::RunFile(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	std::string filePath = inPkt.getStrData();
// 	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
// 		return FALSE;
// 	}
	ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	lstPkt.push_back(CPkt(3, NULL, 0)); //  ���ݿ�, sendACK�Ƿ��жϿ�ָ������
	return TRUE;
}

BOOL CCommandHanle::DownloadFile(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	std::string filePath = inPkt.getStrData();
	long long dlen = 0;
// 	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
// 		return FALSE;
// 	}
	FILE* pf = fopen(filePath.c_str(), "rb");
	if (pf == NULL) {
		lstPkt.push_back(CPkt(4, (BYTE*)&dlen, 8));
		return FALSE;
	}

	fseek(pf, 0, SEEK_END);
	dlen = _ftelli64(pf);
	fseek(pf, 0, SEEK_SET);

	lstPkt.push_back(CPkt(4, (BYTE*)&dlen, 8));    //  �ļ���С

	char buf[1024] = {};
	size_t rlen = 0;
	do {
		rlen = fread(buf, 1, 1024, pf);
		lstPkt.push_back(CPkt(4, (BYTE*)buf, rlen));
	} while (rlen >= 1024);
	lstPkt.push_back(CPkt(4, NULL, 0));    //  �����β
	fclose(pf);
	return TRUE;
}

BOOL CCommandHanle::DeleteLocalFile(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	std::string filePath = inPkt.getStrData();
	// CSrvSocket::getInstance()->getFilePath(filePath);
	int ret = DeleteFile(filePath.c_str());
	lstPkt.push_back(CPkt(9, NULL, 0));
	return TRUE;
}

BOOL CCommandHanle::MouseEvent(std::list<CPkt>& lstPkt, CPkt& inPkt) {

	MOUSEVENT mouse;
	memcpy(&mouse, inPkt.getStrData().c_str(), sizeof(MOUSEVENT));
	if (TRUE) {

		DWORD nFlags = 0;

		switch (mouse.nButton) {
		case 0: //  ���
			nFlags = 0x0001;
			break;
		case 1: //  �Ҽ�
			nFlags = 0x0002;
			break;
		case 2: //  �м�
			nFlags = 0x0004;
			break;
		case 3: //  �ް���
			nFlags = 0x0008;
			break;
		default:
			break;
		}

		switch (mouse.nAction) {
		case 0: //  ����
			nFlags |= 0x0010;
			break;
		case 1: //  ˫��
			nFlags |= 0x0020;
			break;
		case 2: //  ����
			nFlags |= 0x0040;
			break;
		case 3: //  ����
			nFlags |= 0x0080;
			break;
		case 4:

			break;
		default:
			break;
		}

		if ((nFlags & 0x0008) == 0) {
			SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);
		}

		//TRACE(TEXT("nFlags : %08x x: %d, y: %d\r\n"), nFlags, mouse.ptXY.x, mouse.ptXY.y);

		switch (nFlags) {
		case 0x21:  //  ���˫��
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x11:  //  �������
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x22:  //  �Ҽ�˫��
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x12:  //  �Ҽ�����
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x24:  //  �м�˫��
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x14:  //  �м�����
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x41:  //  �������
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x42:  //  �Ҽ�����
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x44:  //  �м�����
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x81:  //  �������
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x82:  //  �Ҽ�����
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x84:  //  �м�����
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x08:  //  ����ƶ�
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mouse.ptXY.x * 65546 / 1920, mouse.ptXY.y * 65536 / 1080, 0, GetMessageExtraInfo());
			break;
		default:
			break;
		}
		lstPkt.push_back(CPkt(4, NULL, 0));
	}
	else {
		//  
	}

	return TRUE;
}


BOOL CCommandHanle::SendScreen(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	//CSrvSocket::getInstance()->sendACK(CPkt(6, NULL, 0));
	CImage screen;
	HDC hScreen = ::GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);   //  һ�������ö��ٸ�bit, ��͸���ȵ�ARGB8888 32bit, RGB888 24��ɫ��
	int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeight = GetDeviceCaps(hScreen, VERTRES);

	screen.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(screen.GetDC(), 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

	ReleaseDC(NULL, hScreen);

	//screen.Save(TEXT("test2022.png"), Gdiplus::ImageFormatPNG);
	//screen.Save(TEXT("test2020.jpg"), Gdiplus::ImageFormatJPEG);

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL) {
		return FALSE;
	}
	IStream* pStream = NULL;

	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret == S_OK) {
		screen.Save(pStream, Gdiplus::ImageFormatPNG);
		LARGE_INTEGER li = {};
		pStream->Seek(li, SEEK_SET, NULL);
		PBYTE pData = (PBYTE)GlobalLock(hMem);
		if (pData == NULL) {
			//
		}
		SIZE_T nSize = GlobalSize(hMem);
		//TRACE("send screen size : %d\r\n", nSize);
		lstPkt.push_back(CPkt(6, pData, nSize));	//  ���pData������ն˻������, ��Ϊ�ͻ���recvһ�ν���4K
		GlobalUnlock(hMem);
	}

	screen.ReleaseDC();
	pStream->Release();
	GlobalFree(hMem);

	return TRUE;
}

BOOL CCommandHanle::LockMachine(std::list<CPkt>& lstPkt, CPkt& inPkt) {

	if (!dlg.m_hWnd || dlg.m_hWnd == INVALID_HANDLE_VALUE) {
		//_beginthread(threadLoackMachine, 0, NULL);
		_beginthreadex(NULL, 0, CCommandHanle::ThreadLoackMachine, this, 0, &TID);
	}

	lstPkt.push_back(CPkt(7, NULL, 0));

	return TRUE;
}

BOOL CCommandHanle::UnLockMachine(std::list<CPkt>& lstPkt, CPkt& inPkt) {

	//SendMessage(dlg.m_hWnd, WM_KEYDOWN, 0x1B, 0x01E0001);   //  ����ͬһ�߳�
	PostThreadMessage(TID, WM_KEYDOWN, 0x1B, 0x01E0001);

	lstPkt.push_back(CPkt(8, NULL, 0));
	while (dlg.m_hWnd) {
		Sleep(50);
	}
	return TRUE;
}

BOOL CCommandHanle::ConnectTest(std::list<CPkt>& lstPkt, CPkt& inPkt) {
	lstPkt.push_back(CPkt(7777, NULL, 0));
	TRACE(TEXT("connected\r\n"));
	return TRUE;
}


unsigned int __stdcall  CCommandHanle::ThreadLoackMachine(void* arg) {

	CCommandHanle* pCmd = (CCommandHanle*)arg;
	pCmd->ThreadProc();

	_endthreadex(0);
	return 0;
}

void CCommandHanle::ThreadProc() {
	dlg.Create(IDD_DIALOG_INFO);
	dlg.ShowWindow(SW_SHOW);
	::ShowWindow(::FindWindow(TEXT("Shell_TrayWnd"), NULL), SW_HIDE);
	dlg.SetWindowPos(&dlg.wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	CRect rect = { 0, 0, (int)(GetSystemMetrics(SM_CXFULLSCREEN)), (int)(GetSystemMetrics(SM_CYFULLSCREEN) * 1.1) };

	dlg.MoveWindow(rect);

	CWnd* pText = dlg.GetDlgItem(IDC_STATIC);
	if (pText) {
		CRect rtText;
		pText->GetWindowRect(&rtText);
		int nWidth = rtText.Width() / 2;
		int nHeight = rtText.Height() / 2;
		int x = (rect.right - nWidth) / 2;
		int y = (rect.bottom - nHeight) / 2;
		pText->MoveWindow(x, y, rtText.Width(), rtText.Height());
	}

	ShowCursor(false);
	ClipCursor(CRect(0, 0, 0, 0));

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_KEYDOWN && msg.wParam == 0x1B) {
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//dlg.DestroyWindow();

	ShowCursor(TRUE);
	::ShowWindow(::FindWindow(TEXT("Shell_TrayWnd"), NULL), SW_SHOW);
	ClipCursor(NULL);
	dlg.DestroyWindow();
}