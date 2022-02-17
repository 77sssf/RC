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
		MessageBox(NULL, TEXT("��ǰΪ����Ա"), TEXT("�˻����"), MB_OK);
		if (CTool::MFCinit()) {
			return 1;
		}
		CTool::AutoInvoke();
		CCommandHanle cmd;
		if (!CSrvSocket::getInstance()->Run(&CCommandHanle::RunCommand, &cmd)) {
			MessageBox(NULL, TEXT("�����ʼ���쳣, �������绷��"), TEXT("�������"), MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}
	else {
		//  TODO : ����Ȩ��
		MessageBox(NULL, TEXT("��ǰΪ��ͨ�û�"), TEXT("�˻����"), MB_OK);
		CTool::RunAsAdmin();
	}
	return 0;
}