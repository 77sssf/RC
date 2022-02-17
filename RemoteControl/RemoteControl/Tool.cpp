#include "pch.h"

void CTool::Dump(BYTE* pData, size_t nSize) {

}

void CTool::ShowError() {
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

BOOL CTool::CheckAdmin() {
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

void CTool::RunAsAdmin() {
	//  1. ��ȡ����ԱȨ��
	//  2. ʹ�ù���ԱȨ�޴�������
	STARTUPINFOW si = {};
	PROCESS_INFORMATION pi = {};
	LPWSTR sPath = NULL;
	sPath = GetCommandLineW();

	//ret = CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE,NULL, sPath, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
	BOOL ret = CreateProcessWithLogonW(ADMIN, NULL, NULL, LOGON_WITH_PROFILE, NULL, sPath, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
	if (ret == FALSE) {
		ShowError();
		exit(0);
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void CTool::SetStarupDir() {
	//  C:\Users\moeby\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
	CString sPath = TEXT(STARTUP_PATH);
	CString sCmd = GetCommandLine();
	sCmd.Replace("\"", "");
	BOOL ret = CopyFile(sCmd, sPath, FALSE);
	if (ret == FALSE) {
		MessageBox(NULL, TEXT("���ÿ����Զ�����ʧ��, ����Ȩ��, ��������ʧ��"), TEXT("����"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		exit(0);
	}
}

void CTool::SetReg() {
	CString sSubKey = TEXT(SUB_KEY);
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

void CTool::AutoInvoke() {
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
	else if (ret == IDCANCEL) {
		exit(0);
	}
	else {
		//  TODO : Nothing;
	}
}

BOOL CTool::MFCinit() {
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(nullptr);
	if (hModule == nullptr) {
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}
	// initialize MFC and print and error on failure
	if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
	{
		// TODO: code your application's behavior here.
		wprintf(L"Fatal Error: MFC initialization failed\n");
		nRetCode = 1;
	}
	return nRetCode;
}