#include "pch.h"

void CTool::Dump(BYTE* pData, size_t nSize) {

}

BOOL CTool::Bytes2Image(CImage& image, const std::string& data) {
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL) {
		TRACE(TEXT("ÄÚ´æ²»×ã"));
		Sleep(500);
		return FALSE;
	}
	IStream* pStream = NULL;
	HRESULT hRet = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (hRet == S_OK) {
		ULONG length = 0;
		pStream->Write(data.c_str(), data.size(), &length);
		LARGE_INTEGER bg = {};
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		if ((HBITMAP)image) {
			image.Destroy();
		}
		image.Load(pStream);						//  memory to image
		pStream->Release();
		GlobalFree(hMem);
		return TRUE;
	}
	return FALSE;
}