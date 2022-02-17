// RemoteControl.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
CWinApp theApp;

int main()
{
	if (CTool::CheckAdmin()) {
		MessageBox(NULL, TEXT("当前为管理员"), TEXT("账户身份"), MB_OK);
		if (CTool::MFCinit()) {
			return 1;
		}
		CTool::AutoInvoke();
		CCommandHanle cmd;
		if (!CSrvSocket::getInstance()->Run(&CCommandHanle::RunCommand, &cmd)) {
			MessageBox(NULL, TEXT("网络初始化异常, 请检查网络环境"), TEXT("网络错误"), MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}
	else {
		//  TODO : 提升权限
		MessageBox(NULL, TEXT("当前为普通用户"), TEXT("账户身份"), MB_OK);
		CTool::RunAsAdmin();
	}
	return 0;
}