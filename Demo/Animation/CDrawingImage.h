
#pragma once
#include "CDrawingBoard.h"

class CDrawingImage : public CDrawingBoard
{
public:
    CDrawingImage();
    virtual ~CDrawingImage();

public:
	// ֱ�ӱ���Ϊһ��ָ����ʽ���ļ�
	bool SaveBoardToFile(const char *pszSavePath);
	// ��һ���ڴ��д���
	void CreateByMem(BYTE *pImgFileMem, int nLen);
	// ���ļ��д���
	void CreateByFile(LPCWSTR pszFilePath);
	// ��HBITMAP����
	void CreateByHBITMAP(HBITMAP hBmp);
	void CreateByHICON(HICON hIcon);
	// ������ڴ��ʽΪ DIB 32 λ�����ݡ�
	void CreateByScanMem(int nWidth, int nHeight, BYTE* pScan0);

private:
	bool GetImageCLSID(const WCHAR* format, CLSID* pCLSID);
	void DrawingImageRelease();

private:
	BYTE* m_pScan0;
};
