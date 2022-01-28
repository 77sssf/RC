// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteControl.h"
#include "SrvSocket.h"
#include <direct.h>
#include <io.h>
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

typedef struct _FILEINFO{
    _FILEINFO() : szFileName{}, IsInvalid(FALSE), IsDirectory(FALSE), HasNext(TRUE) {
    
    }
    char szFileName[MAX_PATH];
    BOOL IsInvalid; //  是否有效
    BOOL IsDirectory;
    BOOL HasNext;   //  是否还有后续
}FILEINFO, *PFILEINFO;



BOOL MakeDriverInfo(){
    std::string res;
    //  _chdrive(); //  A盘 : 1 B盘 : 2 ... Z盘 : 26
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
        //  没有找到任何文件
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
    CSrvSocket::getInstance()->sendACK(CPacket(3, NULL, 0)); //  传递空, sendACK是否判断空指针的情况
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
    CSrvSocket::getInstance()->sendACK(CPacket(4, (BYTE*)&dlen, 8));    //  文件大小

    char buf[1024] = {};
    size_t rlen = 0;
    do {
        rlen = fread(buf, 1, 1024, pf);
        CSrvSocket::getInstance()->sendACK(CPacket(4, (BYTE*)buf, rlen));
    } while (rlen >= 1024);
    CSrvSocket::getInstance()->sendACK(CPacket(4, NULL, 0));    //  代表结尾
    fclose(pf);
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
            //  先写难度大的
            //  1. 进度可控性
            //  2. 方便对接
            //  3. 可行性评估

            
            

//             CSrvSocket* pSockSrv = CSrvSocket::getInstance();
//             if (pSockSrv == NULL) {
//                 return FALSE;
//             }
// 
//             if (pSockSrv->initSocket() == FALSE) {
//                 MessageBox(NULL, TEXT("网络初始化异常, 请检查网络环境"), TEXT("网络错误"), MB_OK | MB_ICONERROR);
//             }
//             
//             while (CSrvSocket::getInstance()) {
//                 int cnt = 0;
//                 if (pSockSrv->acceptClient() == FALSE) {
//                     if (cnt >= 3) {
// 						MessageBox(NULL, TEXT("无法接入用户"), TEXT("接入用户失败"), MB_OK | MB_ICONERROR);
//                         exit(0);
//                     }
//                     MessageBox(NULL, TEXT("无法接入用户, 自动重试..."), TEXT("接入用户失败"), MB_OK | MB_ICONERROR);
//                     Sleep(1000);
//                     cnt++;
// 				}
// 
//                 int ret = pSockSrv->dealRequest();
// 
//             }
            int nCmd = 1;
            switch (nCmd) {
            case 1: //  获取盘符
                MakeDriverInfo();
                break;
            case 2: //  查看指定目录下的文件
                MakeDirectoryInfo();
                break;
            case 3: //  打开文件
                RunFile();
                break;
            case 4: //  下载文件
                DownloadFile();
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
