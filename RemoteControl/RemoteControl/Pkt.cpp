#include "pch.h"

CPkt::CPkt() : sHead(0), nLength(0), sCmd(0), sSum(0) {

}

CPkt::CPkt(const BYTE* pData, size_t& nSize) : sHead(0), nLength(0), sCmd(0), sSum(0) {
	size_t i = 0;
	for (i = 0; i < nSize; ++i) {
		if (*((WORD*)(pData + i)) == 0xFFFE) {
			//  找到包头
			i += 2;
			sHead = 0xFFFE;
			break;
		}
	}
	if (i + 4 + 2 + 2 > nSize) {
		nSize = 0;		//  用掉了0字节
		return;
	}

	nLength = *(DWORD*)(pData + i);
	i += 4;
	if (nLength + i > nSize) {	//  包数据不全
		nSize = 0;
		return;
	}

	sCmd = *(WORD*)(pData + i);
	i += 2;
	if (nLength > 4) {
		strData.resize(nLength - 4);
		memcpy((void*)strData.c_str(), pData + i, nLength - 4);
		i += nLength - 4;
	}

	sSum = *(WORD*)(pData + i);
	i += 2;

	WORD tSum = 0;
	for (size_t j = 0; j < strData.size(); ++j) {
		tSum += (BYTE)strData[j];
	}

	if (sSum == tSum) {
		nSize = i;
	}
	else {
		nSize = 0;
	}

	calcData();
}

CPkt::CPkt(const CPkt& rhs) {
	sHead = rhs.sHead;
	nLength = rhs.nLength;
	sCmd = rhs.sCmd;
	strData = rhs.strData;
	sSum = rhs.sSum;

	calcData();
}

CPkt::CPkt(WORD cmd, const BYTE* pData, size_t nSize) : sHead(0), nLength(0), sCmd(0), sSum(0) {
	sHead = 0xFFFE;
	nLength = nSize + 2 + 2;
	sCmd = cmd;
	if (nSize > 0) {
		strData.resize(nSize);
		memcpy((void*)strData.c_str(), pData, nSize);
	}
	else {
		strData.clear();
	}

	//strData = (const char*)pData;
	for (size_t j = 0; j < strData.size(); ++j) {
		sSum += (BYTE)strData[j];
	}

	calcData();
}

CPkt& CPkt::operator=(const CPkt& rhs) {
	sHead = rhs.sHead;
	nLength = rhs.nLength;
	sCmd = rhs.sCmd;
	strData = rhs.strData;
	sSum = rhs.sSum;
	calcData();
	return *this;
}

CPkt::~CPkt() {

}

WORD CPkt::getCmd() const {
	return sCmd;
}

DWORD CPkt::getLength() const {
	return nLength;
}

const char* CPkt::getData() const {
	return strRes.c_str();
}
std::string CPkt::getStrData() const {
	return strData;
}

void CPkt::calcData() {
	strRes.resize(nLength + 2 + 4);
	char* pData = (char*)strRes.c_str();
	*(WORD*)pData = sHead;
	pData += 2;
	*(DWORD*)pData = nLength;
	pData += 4;
	*(WORD*)pData = sCmd;
	pData += 2;
	memcpy(pData, strData.c_str(), strData.size());
	pData += strData.size();
	*(WORD*)pData = sSum;

	//return str.c_str();		//  BUG : 返回局部变量的地址
}

size_t CPkt::size() const {
// 	WORD sHead;	//  包头固定FFFE
// 	DWORD nLength;	//  包长度(从控制命令开始到和校验结束)
// 	WORD sCmd;
// 	std::string strData;
// 	std::string strRes;
// 	WORD sSum;	//  和校验值
	return (sizeof(WORD) + sizeof(DWORD) + sizeof(WORD) + strData.size() + sizeof(WORD));
}