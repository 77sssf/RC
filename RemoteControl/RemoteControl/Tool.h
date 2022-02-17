#pragma once
class CTool
{
public:
	static void Dump(BYTE* pData, size_t nSize);
	static void ShowError();
	static BOOL CheckAdmin();
	static void RunAsAdmin();
	static void SetStarupDir();
	static void SetReg();
	static void AutoInvoke();
	static BOOL MFCinit();
};

