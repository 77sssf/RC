// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

void SetStarupDir() {
	//  C:\Users\moeby\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
	CString sPath = TEXT("C:\\Users\\moeby\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\RemoteControl.exe");
	CString sCmd = GetCommandLine();
	sCmd.Replace("\"", "");
	BOOL ret = CopyFile(sCmd, sPath, FALSE);
	if (ret == FALSE) {
		MessageBox(NULL, TEXT("���ÿ����Զ�����ʧ��, ����Ȩ��, ��������ʧ��"), TEXT("����"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		exit(0);
	}
}

void SetReg() {
	CString sSubKey = TEXT("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run");
	char sCurPath[MAX_PATH] = {};
	GetCurrentDirectory(MAX_PATH, sCurPath);
	//  �����ж�̬������, ����ʹ��mklink
	std::string strCmd = std::string("mklink ") + "%SystemRoot%\\SysWOW64\\RemoteControl.exe " + sCurPath + "\\RemoteControl.exe";
	int ret = system(strCmd.c_str());

	//  ��дע���
	HKEY hKey = NULL;
	ret = RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
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

void AutoInvoke() {
	//  C:\\Windows\\System32\\
	//  HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	if (PathFileExists("C:\\Windows\\SysWOW64\\RemoteControl.exe") || PathFileExists("C:\\Users\\moeby\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\RemoteControl.exe")) {
		return;
	}
	CString programInfo = TEXT("�ó���ֻ�������ںϷ���;\n");
	programInfo += TEXT("�������иó���ʹϵͳ���ڱ����״̬\n");
	programInfo += TEXT("�����ϣ�������, �밴\"ȡ��\", �˳�����\n");
	programInfo += TEXT("���\"��\", �ó��������������ÿ����Զ�����\n");
	programInfo += TEXT("���\"��\", �ó��������������ÿ����Զ�����\n");
	int ret = MessageBox(NULL, programInfo, TEXT("����"), MB_YESNOCANCEL | MB_ICONWARNING | MB_TOPMOST);
	if (ret == IDYES) {
		SetStarupDir();
	}
	else if(ret == IDCANCEL) {
		exit(0);
	}
	else {
		//  TODO : Nothing;
	}
}

void ShowError() {
	LPSTR lpMsgBuf = NULL;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, LANG_CHINESE_SIMPLIFIED),
		(LPSTR)&lpMsgBuf, 
		0, NULL);
	OutputDebugString(lpMsgBuf);
	LocalFree(lpMsgBuf);
}

BOOL CheckAdmin() {
	HANDLE hToken = NULL;
	BOOL ret = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	if (ret == FALSE) {
		ShowError();
		return FALSE;
	}
	TOKEN_ELEVATION eve;
	DWORD len = 0;
	ret = GetTokenInformation(hToken, TokenElevation, &eve, sizeof(eve), &len);
	if (ret == FALSE) {
		
	}
	CloseHandle(hToken);
	if (len == sizeof(eve)) {
		return eve.TokenIsElevated;
	}
	return FALSE;
}

void RunAsAdmin() {
	//  1. ��ȡ����ԱȨ��
	//  2. ʹ�ù���ԱȨ�޴�������
	STARTUPINFOW si = {};
	PROCESS_INFORMATION pi = {};
	LPWSTR sPath = NULL;
	sPath = GetCommandLineW();
	
	//ret = CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE,NULL, sPath, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
	BOOL ret = CreateProcessWithLogonW(L"Administrator", NULL, NULL, LOGON_WITH_PROFILE, NULL, sPath, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
	if (ret == FALSE) {
		ShowError();
		exit(0);
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int main()
{
	if (CheckAdmin()) {
		MessageBox(NULL, TEXT("��ǰΪ����Ա"), TEXT("�˻����"), MB_OK);
	}
	else {
		//  TODO : ����Ȩ��
		MessageBox(NULL, TEXT("��ǰΪ��ͨ�û�"), TEXT("�˻����"), MB_OK);
		RunAsAdmin();
		return 0;
	}

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
			//AutoInvoke();
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