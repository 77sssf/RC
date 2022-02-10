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
			CCommandHanle cmd;
            if (!CSrvSocket::getInstance()->Run(&CCommandHanle::RunCommand, &cmd)) {
                MessageBox(NULL, TEXT("网络初始化异常, 请检查网络环境"), TEXT("网络错误"), MB_OK | MB_ICONERROR);
                exit(-1);
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