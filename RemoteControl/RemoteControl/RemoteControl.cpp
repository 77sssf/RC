// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

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
            CSrvSocket* pSockSrv = CSrvSocket::getInstance();
            CCommandHanle cmd;

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
                cmd.ExecuteCommand(ret);
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