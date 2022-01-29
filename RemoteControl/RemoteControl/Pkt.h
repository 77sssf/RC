#pragma once

#pragma pack(push)
#pragma pack(1)

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