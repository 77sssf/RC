#pragma once

#include "Windows.h"
#include "atlimage.h"
#include <string>

class CTool
{
public:
	static void Dump(BYTE* pData, size_t nSize);
	static BOOL Bytes2Image(CImage& image, const std::string& data);
};

