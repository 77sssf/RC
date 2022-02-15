#pragma once

#pragma pack(push)
#pragma pack(1)

typedef struct _MOUSEVENT {

	_MOUSEVENT() : nAction(0), nButton(-1), ptXY{ 0, 0 } {

	}

	WORD nAction;	//  �ƶ�����
	WORD nButton;	//  ���, �Ҽ�
	POINT ptXY;

}MOUSEVENT, * PMOUSEVENT;

typedef struct _FILEINFO {
	_FILEINFO() : szFileName{}, IsInvalid(TRUE), IsDirectory(FALSE), HasNext(TRUE) {

	}
	char szFileName[MAX_PATH];
	BOOL IsInvalid; //  �Ƿ���Ч
	BOOL IsDirectory;
	BOOL HasNext;   //  �Ƿ��к���
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

	WORD sHead;	//  ��ͷ�̶�FFFE
	DWORD nLength;	//  ������(�ӿ������ʼ����У�����)
	WORD sCmd;
	std::string strData;
	std::string strRes;
	WORD sSum;	//  ��У��ֵ
};

#pragma pack(pop)