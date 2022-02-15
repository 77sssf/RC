#pragma once

#pragma pack(push)
#pragma pack(1)

typedef struct _MOUSEVENT {

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{ 0, 0 } {

	}

	WORD nAction;	//  移动或点击
	WORD nButton;	//  左键, 右键
	POINT ptXY;

}MOUSEVENT, * PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(TRUE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  是否有效
	BOOL IsDirectory;
	BOOL HasNext;   //  是否还有后续
}FILEINFO, * PFILEINFO;

class CPkt
{
public:

	CPkt();
	CPkt(WORD, const BYTE*, size_t);
	CPkt(const CPkt& rhs);
	CPkt(const BYTE* pData, size_t& nSize);
	CPkt& operator=(const CPkt& rhs);
	~CPkt();



	WORD getCmd()const;
	DWORD getLength()const;
	const char* getData() const;
	std::string getStrData() const;
	size_t size() const ;
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