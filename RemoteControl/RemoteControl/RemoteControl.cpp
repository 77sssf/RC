// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteControl.h"
#include "SrvSocket.h"
#include <direct.h>
#include <io.h>
//#include <list>
#include <atlimage.h>
#include "LockDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
CLockDialog dlg;
unsigned int TID;





BOOL MakeDriverInfo(){
    std::string res;
    //  _chdrive(); //  A盘 : 1 B盘 : 2 ... Z盘 : 26
    for (int i = 1; i <= 26; ++i) {
        if (!_chdrive(i)) {
//             if (res.size()) {
//                 res += ',';
//             }
            res += ('A' + i - 1);
            res += ",";
        }
    }
    CSrvSocket::getInstance()->sendACK(CPkt((WORD)1, (BYTE*)res.c_str(), res.size()));
    return TRUE;
}


BOOL MakeDirectoryInfo() {
    
    std::string filePath;
    //std::list<FILEINFO> lst;

    if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
        //  cmd != 2
        return FALSE;
    }

    //  切换_chdir
    if (_chdir(filePath.c_str())) {
        //  切换失败
        //  没有权限或路径错误
        return FALSE;
    }

    //  遍历文件
    _finddata_t fdata = {};
    intptr_t hFile = 0;
    if ((hFile = _findfirst("*", &fdata)) == -1) {
        //  没有找到任何文件, 发送空的FILEINFO, INVALID = TRUE
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
        CSrvSocket::getInstance()->sendACK(CPkt(2, (BYTE*)&fifo, sizeof(fifo)));

    } while (!_findnext(hFile, &fdata));
    
    FILEINFO fifo;  //  constructor
    fifo.HasNext = FALSE;
    CSrvSocket::getInstance()->sendACK(CPkt(2, (BYTE*)&fifo, sizeof(fifo)));

    _findclose(hFile);

    //  封包发送到客户端
    //  一次性发送, 如果文件太多, 客户端会等待很久
    //  有一个发一个, 标记是否是最后一个文件

    return TRUE;
}

BOOL RunFile() {
    std::string filePath;
	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
		return FALSE;
	}
    ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    CSrvSocket::getInstance()->sendACK(CPkt(3, NULL, 0)); //  传递空, sendACK是否判断空指针的情况
    return TRUE;
}

BOOL DownloadFile() {
	std::string filePath;
    long long dlen = 0;
	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
		return FALSE;
	}
    FILE* pf = fopen(filePath.c_str(), "rb");
    if (pf == NULL) {
        CSrvSocket::getInstance()->sendACK(CPkt(4, (BYTE*)&dlen, 8));
        return FALSE;
    }

    fseek(pf, 0, SEEK_END);
    dlen = _ftelli64(pf);
    fseek(pf, 0, SEEK_SET);

    CSrvSocket::getInstance()->sendACK(CPkt(4, (BYTE*)&dlen, 8));    //  文件大小
	
    char buf[1024] = {};
    size_t rlen = 0;
    do {
        rlen = fread(buf, 1, 1024, pf);
        CSrvSocket::getInstance()->sendACK(CPkt(4, (BYTE*)buf, rlen));
    } while (rlen >= 1024);
    CSrvSocket::getInstance()->sendACK(CPkt(4, NULL, 0));    //  代表结尾
    fclose(pf);
    return TRUE;
}

BOOL DeleteLocalFile() {
    std::string filePath;
    CSrvSocket::getInstance()->getFilePath(filePath);
    int ret = DeleteFile(filePath.c_str());
    CSrvSocket::getInstance()->sendACK(CPkt(9, NULL, 0));
    return TRUE;
}

BOOL MouseEvent() {
    
    MOUSEVENT mouse;

    if (CSrvSocket::getInstance()->getMouseEvent(mouse)) {

        DWORD nFlags = 0;

        switch (mouse.nButton) {
        case 0: //  左键
            nFlags = 0x0001;
            break;
        case 1: //  右键
            nFlags = 0x0002;
            break;
        case 2: //  中键
            nFlags = 0x0004;
            break;
        case 3: //  无按键
            nFlags = 0x0008;
            break;
        default:
            break;
        }
        
		switch (mouse.nAction) {
		case 0: //  单击
			nFlags |= 0x0010;
			break;
		case 1: //  双击
			nFlags |= 0x0020;
			break;
		case 2: //  按下
			nFlags |= 0x0040;
			break;
        case 3: //  弹起
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

        switch (nFlags) {
		case 0x21:  //  左键双击
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x11:  //  左键单击
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x22:  //  右键双击
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x12:  //  右键单击
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x24:  //  中键双击
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x14:  //  中键单击
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x41:  //  左键按下
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x42:  //  右键按下
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x44:  //  中键按下
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x81:  //  左键弹起
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x82:  //  右键弹起
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
		case 0x84:  //  中键弹起
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x08:  //  鼠标移动
            mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
            break;
        default:
            break;
        }
        CSrvSocket::getInstance()->sendACK(CPkt(4, NULL, 0));
    
    }
    else {
        //  
    }
    
    return TRUE;
}


BOOL SendScreen() {
    CImage screen;
    HDC hScreen = ::GetDC(NULL);
    int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);   //  一个像素用多少个bit, 带透明度的ARGB8888 32bit, RGB888 24真色彩
    int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeight = GetDeviceCaps(hScreen, VERTRES);

    screen.Create(nWidth, nHeight, nBitPerPixel);
    BitBlt(screen.GetDC(), 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

    ReleaseDC(NULL, hScreen);

    //screen.Save(TEXT("test2022.png"), Gdiplus::ImageFormatPNG);
	//screen.Save(TEXT("test2020.jpg"), Gdiplus::ImageFormatJPEG);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
    if (hMem == NULL) {
        
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
        CSrvSocket::getInstance()->sendACK(CPkt(6, pData, nSize));
		GlobalUnlock(hMem);
    }

    screen.ReleaseDC();
    pStream->Release();
    GlobalFree(hMem);

    return TRUE;
}

unsigned int __stdcall  threadLoackMachine(void* arg) {

	//  弹出消息 : 已锁机, 请联系管理员解锁
	//  消息框最顶层

	dlg.Create(IDD_DIALOG_INFO);
	dlg.ShowWindow(SW_SHOW);
	::ShowWindow(::FindWindow(TEXT("Shell_TrayWnd"), NULL), SW_HIDE);
	dlg.SetWindowPos(&dlg.wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	CRect rect = { 0, 0, (int)(GetSystemMetrics(SM_CXFULLSCREEN) * 1.2), (int)(GetSystemMetrics(SM_CYFULLSCREEN) * 1.2) };

	dlg.MoveWindow(rect);




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
    
    dlg.DestroyWindow();

    _endthreadex(0);
    return 0;
}

BOOL LockMachine() {

    if (!dlg.m_hWnd || dlg.m_hWnd == INVALID_HANDLE_VALUE) {
        //_beginthread(threadLoackMachine, 0, NULL);
        _beginthreadex(NULL, 0, threadLoackMachine, NULL, 0, &TID);
    }

	CSrvSocket::getInstance()->sendACK(CPkt(7, NULL, 0));

    return TRUE;
}

BOOL UnLockMachine() {

    //SendMessage(dlg.m_hWnd, WM_KEYDOWN, 0x1B, 0x01E0001);   //  不在同一线程
    PostThreadMessage(TID, WM_KEYDOWN, 0x1B, 0x01E0001);

    return TRUE;
}

void connectTest() {
    CSrvSocket::getInstance()->sendACK(CPkt(7777, NULL, 0));
}

void execCmd(int nCmd) {
	
	switch (nCmd) {
	case 1: //  获取盘符
		MakeDriverInfo();
		break;
	case 2: //  查看指定目录下的文件
        if (MakeDirectoryInfo() == FALSE) {
			FILEINFO fifo;  //  constructor
			fifo.HasNext = FALSE;
			CSrvSocket::getInstance()->sendACK(CPkt(2, (BYTE*)&fifo, sizeof(fifo)));
        }
		break;
	case 3: //  打开文件
		RunFile();
		break;
	case 4: //  下载文件
		DownloadFile();
		break;
	case 5: //  鼠标操作
		MouseEvent();
		break;
	case 6: //  发送屏幕截图
		SendScreen();
		break;
	case 7: //  锁机 : 禁止操作
		LockMachine();
		break;
	case 8: //  解锁
		UnLockMachine();
		break;
    case 9:
        DeleteLocalFile();
        break;
    case 7777:
        connectTest();
        break;
	default:
		break;
	}
}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
            //  先写难度大的
            //  1. 进度可控性
            //  2. 方便对接
            //  3. 可行性评估

            
            

            CSrvSocket* pSockSrv = CSrvSocket::getInstance();
            if (pSockSrv == NULL) {
                return FALSE;
            }

            if (pSockSrv->initSocket() == FALSE) {
                MessageBox(NULL, TEXT("网络初始化异常, 请检查网络环境"), TEXT("网络错误"), MB_OK | MB_ICONERROR);
            }
            
            while (CSrvSocket::getInstance()) {
                int cnt = 0;
                if (pSockSrv->acceptClient() == FALSE) {
                    if (cnt >= 3) {
						MessageBox(NULL, TEXT("无法接入用户"), TEXT("接入用户失败"), MB_OK | MB_ICONERROR);
                        exit(0);
                    }
                    MessageBox(NULL, TEXT("无法接入用户, 自动重试..."), TEXT("接入用户失败"), MB_OK | MB_ICONERROR);
                    Sleep(1000);
                    cnt++;
				}

                int ret = pSockSrv->dealRequest();

                execCmd(ret);

                pSockSrv->closeClient();
            }
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
