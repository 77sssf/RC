#pragma once

#include <string>

#pragma pack(push)
#pragma pack(1)

class CPkt
{
public:

	CPkt();
	CPkt(WORD, const BYTE*, size_t, HANDLE hEvent = INVALID_HANDLE_VALUE);
	CPkt(const CPkt& rhs);
	CPkt& operator=(const CPkt& rhs);
	CPkt(const BYTE* pData, size_t& nSize, HANDLE hEvent = INVALID_HANDLE_VALUE);
	~CPkt();



	WORD getCmd()const;
	DWORD getLength()const;
	const char* getData() const;
	std::string getStrData() const;
	size_t size() const;
public:
	HANDLE m_hEvnet;

private:

	void calcData();

private:


	WORD sHead;	//  包头固定FFFE
	DWORD nLength;	//  包长度(从控制命令开始到和校验结束)
	WORD sCmd;
	std::string strData;
	std::string strRes;
	WORD sSum;	//  和校验值
};

#pragma pack(pop)