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
            //  ��д�Ѷȴ��
            //  1. ���ȿɿ���
            //  2. ����Խ�
            //  3. ����������

            
            

            CSrvSocket* pSockSrv = CSrvSocket::getInstance();
            if (pSockSrv == NULL) {
                return FALSE;
            }

            if (pSockSrv->initSocket() == FALSE) {
                MessageBox(NULL, TEXT("�����ʼ���쳣, �������绷��"), TEXT("�������"), MB_OK | MB_ICONERROR);
            }
            
            while (CSrvSocket::getInstance()) {
                int cnt = 0;
                if (pSockSrv->acceptClient() == FALSE) {
                    if (cnt >= 3) {
						MessageBox(NULL, TEXT("�޷������û�"), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
                        exit(0);
                    }
                    MessageBox(NULL, TEXT("�޷������û�, �Զ�����..."), TEXT("�����û�ʧ��"), MB_OK | MB_ICONERROR);
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
