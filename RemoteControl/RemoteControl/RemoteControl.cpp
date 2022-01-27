// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteControl.h"
#include "SrvSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

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
