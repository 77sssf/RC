// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteControl.h"
#include "SrvSocket.h"
#include <direct.h>
#include <io.h>
#include <list>
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

typedef struct _FILEINFO{
    _FILEINFO() : szFileName{}, IsInvalid(FALSE), IsDirectory(FALSE), HasNext(TRUE) {
    
    }
    char szFileName[MAX_PATH];
    BOOL IsInvalid; //  �Ƿ���Ч
    BOOL IsDirectory;
    BOOL HasNext;   //  �Ƿ��к���
}FILEINFO, *PFILEINFO;



BOOL MakeDriverInfo(){
    std::string res;
    //  _chdrive(); //  A�� : 1 B�� : 2 ... Z�� : 26
    for (int i = 1; i <= 26; ++i) {
        if (!_chdrive(i)) {
            if (res.size()) {
                res += ',';
            }
            res += ('A' + i - 1);
        }
    }
    CSrvSocket::getInstance()->sendACK(CPacket((WORD)1, (BYTE*)res.c_str(), res.size()));
    return TRUE;
}


BOOL MakeDirectoryInfo() {
    
    std::string filePath;
    //std::list<FILEINFO> lst;

    if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
        //  cmd != 2
        return FALSE;
    }

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
        //  û���ҵ��κ��ļ�
        return FALSE;
    }

    do {

        FILEINFO fifo = {};
        fifo.IsInvalid = FALSE;
        fifo.IsDirectory = (fdata.attrib & _A_SUBDIR) ? TRUE : FALSE;
        memcpy(fifo.szFileName, fdata.name, strlen(fdata.name));
        //lst.push_back(fifo);
        CSrvSocket::getInstance()->sendACK(CPacket(2, (BYTE*)&fifo, sizeof(fifo)));

    } while (!_findnext(hFile, &fdata));
    
    FILEINFO fifo;
    fifo.HasNext = FALSE;
    CSrvSocket::getInstance()->sendACK(CPacket(2, (BYTE*)&fifo, sizeof(fifo)));

    _findclose(hFile);

    //  ������͵��ͻ���
    //  һ���Է���, ����ļ�̫��, �ͻ��˻�ȴ��ܾ�
    //  ��һ����һ��, ����Ƿ������һ���ļ�

    return TRUE;
}

BOOL RunFile() {
    std::string filePath;
	if (CSrvSocket::getInstance()->getFilePath(filePath) == FALSE) {
		return FALSE;
	}
    ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    CSrvSocket::getInstance()->sendACK(CPacket(3, NULL, 0)); //  ���ݿ�, sendACK�Ƿ��жϿ�ָ������
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
        CSrvSocket::getInstance()->sendACK(CPacket(4, (BYTE*)&dlen, 8));
        return FALSE;
    }

    fseek(pf, 0, SEEK_END);
    dlen = _ftelli64(pf);
    fseek(pf, 0, SEEK_SET);
    CSrvSocket::getInstance()->sendACK(CPacket(4, (BYTE*)&dlen, 8));    //  �ļ���С

    char buf[1024] = {};
    size_t rlen = 0;
    do {
        rlen = fread(buf, 1, 1024, pf);
        CSrvSocket::getInstance()->sendACK(CPacket(4, (BYTE*)buf, rlen));
    } while (rlen >= 1024);
    CSrvSocket::getInstance()->sendACK(CPacket(4, NULL, 0));    //  �����β
    fclose(pf);
    return TRUE;
}


BOOL MouseEvent() {
    
    MOUSEVENT mouse;

    if (CSrvSocket::getInstance()->getMouseEvent(mouse)) {

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
            mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
            break;
        default:
            break;
        }
        CSrvSocket::getInstance()->sendACK(CPacket(4, NULL, 0));
    
    }
    else {
        //  
    }
    
    return TRUE;
}


BOOL SendScreen() {
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
        
    }
    IStream* pStream = NULL;

    HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
    if (ret == S_OK) {
        screen.Save(pStream, Gdiplus::ImageFormatPNG);
        LARGE_INTEGER li = {};
        pStream->Seek(li, SEEK_SET, NULL);
        PBYTE pData = (PBYTE)GlobalLock(hMem);
        SIZE_T nSize = GlobalSize(hMem);
        CSrvSocket::getInstance()->sendACK(CPacket(6, pData, nSize));
		GlobalUnlock(hMem);
    }

    screen.ReleaseDC();
    pStream->Release();
    GlobalFree(hMem);

    return TRUE;
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
            //  ��д�Ѷȴ��
            //  1. ���ȿɿ���
            //  2. ����Խ�
            //  3. ����������

            
            

//             CSrvSocket* pSockSrv = CSrvSocket::getInstance();
//             if (pSockSrv == NULL) {
//                 return FALSE;
//             }
// 
//             if (pSockSrv->initSocket() == FALSE) {
//                 MessageBox(NULL, TEXT("�����ʼ���쳣, �������绷��"), TEXT("�������"), MB_OK | MB_ICONERROR);
//             }
//             
//             while (CSrvSocket::getInstance()) {
//                 int cnt = 0;
//                 if (pSockSrv->acceptClient() == FALSE) {
//                     if (cnt >= 3) {
// 						MessageBox(NULL, TEXT("�޷������û�"), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
//                         exit(0);
//                     }
//                     MessageBox(NULL, TEXT("�޷������û�, �Զ�����..."), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
//                     Sleep(1000);
//                     cnt++;
// 				}
// 
//                 int ret = pSockSrv->dealRequest();
// 
//             }
            int nCmd = 6;
            switch (nCmd) {
            case 1: //  ��ȡ�̷�
                MakeDriverInfo();
                break;
            case 2: //  �鿴ָ��Ŀ¼�µ��ļ�
                MakeDirectoryInfo();
                break;
            case 3: //  ���ļ�
                RunFile();
                break;
            case 4: //  �����ļ�
                DownloadFile();
                break;
            case 5: //  ������
                MouseEvent();
                break;
            case 6: //  ������Ļ��ͼ
                SendScreen();
                break;
            default:
                break;
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
