
#pragma once
#include "CDrawingBoard.h"

class CDrawingImage : public CDrawingBoard
{
public:
    CDrawingImage();
    virtual ~CDrawingImage();

public:
	// 直接保存为一个指定格式的文件
	bool SaveBoardToFile(const char *pszSavePath);
	// 从一段内存中创建
	void CreateByMem(BYTE *pImgFileMem, int nLen);
	// 从文件中创建
	void CreateByFile(LPCWSTR pszFilePath);
	// 从HBITMAP创建
	void CreateByHBITMAP(HBITMAP hBmp);
	void CreateByHICON(HICON hIcon);
	// 输入的内存格式为 DIB 32 位的数据。
	void CreateByScanMem(int nWidth, int nHeight, BYTE* pScan0);

private:
	bool GetImageCLSID(const WCHAR* format, CLSID* pCLSID);
	void DrawingImageRelease();

private:
	BYTE* m_pScan0;
};
