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
		MessageBox(NULL, TEXT("设置开启自动启动失败, 请检查权限, 程序启动失败"), TEXT("错误"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		exit(0);
	}
}

void SetReg() {
	CString sSubKey = TEXT("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run");
	char sCurPath[MAX_PATH] = {};
	GetCurrentDirectory(MAX_PATH, sCurPath);
	//  可能有动态库依赖, 所以使用mklink
	std::string strCmd = std::string("mklink ") + "%SystemRoot%\\SysWOW64\\RemoteControl.exe " + sCurPath + "\\RemoteControl.exe";
	int ret = system(strCmd.c_str());

	//  读写注册表
	HKEY hKey = NULL;
	ret = RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		MessageBox(NULL, TEXT("设置开启自动启动失败, 请检查权限, 程序启动失败"), TEXT("错误"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		//  TODO : 删除mklink
		exit(0);
	}
	std::string data = "%SystemRoot%\\SysWOW64\\RemoteControl.exe";
	ret = RegSetValueEx(hKey, "RemoteControl", 0, REG_SZ, (const BYTE*)data.c_str(), data.length());
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		MessageBox(NULL, TEXT("设置开启自动启动失败, 请检查权限, 程序启动失败"), TEXT("错误"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		//  TODO : 删除mklink
		exit(0);
	}
}

void AutoInvoke() {
	//  C:\\Windows\\System32\\
	//  HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	if (PathFileExists("C:\\Windows\\SysWOW64\\RemoteControl.exe") || PathFileExists("C:\\Users\\moeby\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\RemoteControl.exe")) {
		return;
	}
	CString programInfo = TEXT("该程序只允许用于合法用途\n");
	programInfo += TEXT("继续运行该程序将使系统处于被监控状态\n");
	programInfo += TEXT("如果不希望被监控, 请按\"取消\", 退出程序\n");
	programInfo += TEXT("点击\"是\", 该程序启动并且设置开机自动启动\n");
	programInfo += TEXT("点击\"否\", 该程序启动不会设置开启自动启动\n");
	int ret = MessageBox(NULL, programInfo, TEXT("警告"), MB_YESNOCANCEL | MB_ICONWARNING | MB_TOPMOST);
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
	//  1. 获取管理员权限
	//  2. 使用管理员权限创建进程
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
		MessageBox(NULL, TEXT("当前为管理员"), TEXT("账户身份"), MB_OK);
	}
	else {
		//  TODO : 提升权限
		MessageBox(NULL, TEXT("当前为普通用户"), TEXT("账户身份"), MB_OK);
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