// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

void AutoInvoke() {
    //  C:\\Windows\\System32\\
    //  HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
    CString sSubKey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    CString programInfo = TEXT("�ó���ֻ�������ںϷ���;\n");
    programInfo += TEXT("�������иó���ʹϵͳ���ڱ����״̬\n");
	programInfo += TEXT("�����ϣ�������, �밴\"ȡ��\", �˳�����\n");
    programInfo += TEXT("���\"��\", �ó��������������ÿ����Զ�����\n");
	programInfo += TEXT("���\"��\", �ó��������������ÿ����Զ�����\n");
    int ret = MessageBox(NULL, programInfo, TEXT("����"), MB_YESNOCANCEL | MB_ICONWARNING | MB_TOPMOST);
    if (ret == IDYES) {
        char sCurPath[MAX_PATH] = {};
        GetCurrentDirectory(MAX_PATH, sCurPath);
        std::string strCmd = std::string("mklink ") + "%SystemRoot%\\System32\\RemoteControl.exe " + sCurPath + "\\RemoteControl.exe";
        ret = system(strCmd.c_str());

        //  ��дע���
        HKEY hKey = NULL;
        ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
        if (ret != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            MessageBox(NULL, TEXT("���ÿ����Զ�����ʧ��, ����Ȩ��, ��������ʧ��"), TEXT("����"), MB_OK | MB_ICONERROR | MB_TOPMOST);
            //  TODO : ɾ��mklink
            exit(0);
        }
        std::string data = "%SystemRoot%\\SysWOW64\\RemoteControl.exe";
        ret = RegSetValueEx(hKey, "RemoteControl", 0, REG_SZ, (const BYTE*)data.c_str(), data.length());
        if (ret != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			MessageBox(NULL, TEXT("���ÿ����Զ�����ʧ��, ����Ȩ��, ��������ʧ��"), TEXT("����"), MB_OK | MB_ICONERROR | MB_TOPMOST);
			//  TODO : ɾ��mklink
			exit(0);
        }
    }
    else if(ret == IDCANCEL) {
        exit(0);
    }
    else {
        
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
            AutoInvoke();

            CCommandHanle cmd;
            if (!CSrvSocket::getInstance()->Run(&CCommandHanle::RunCommand, &cmd)) {
                MessageBox(NULL, TEXT("�����ʼ���쳣, �������绷��"), TEXT("�������"), MB_OK | MB_ICONERROR);
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