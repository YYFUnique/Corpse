
#include "stdafx.h"
#include <iostream>
#include <atlcomcli.h>
#include "CDrawingImage.h"
/*#include "..\..\Inc\UiFeatureDefs.h"*/

CDrawingImage::CDrawingImage(void)
{
	m_pScan0 = NULL;
	CoInitialize(NULL);
}

CDrawingImage::~CDrawingImage(void)
{
	DrawingImageRelease();
}

void CDrawingImage::DrawingImageRelease()
{
	SAFE_DELETE_LIST(m_pScan0);
}

// ���ļ��ڴ��д���
void CDrawingImage::CreateByFile(LPCWSTR pszFilePath)
{
	if (pszFilePath == NULL || wcslen(pszFilePath) <= 0)
		return;

	DrawingImageRelease();
	WIN32_FILE_ATTRIBUTE_DATA FileAttr;
	if (!::GetFileAttributesExW(pszFilePath, GetFileExInfoStandard, &FileAttr))
		return;

	FILE *pFile = NULL;
	_wfopen_s(&pFile, pszFilePath, L"rb");
	if (pFile == NULL)
		return;

	BYTE *pReadBuf = new BYTE[FileAttr.nFileSizeLow];
	if (pReadBuf == NULL)
	{
		fclose(pFile);
		return;
	}

	int nReadCtns = 0;
	while (nReadCtns < (int)FileAttr.nFileSizeLow)
	{
		BYTE *pRead = pReadBuf + nReadCtns;
		int nNeedRead = FileAttr.nFileSizeLow - nReadCtns;

		int nRead = fread_s(pRead, nNeedRead, 1, nNeedRead, pFile);
		nReadCtns += nRead;
	}

	fclose(pFile);
	if (nReadCtns != (int)FileAttr.nFileSizeLow)
	{
		SAFE_DELETE(pReadBuf);
		return;
	}

	CreateByMem(pReadBuf, nReadCtns);
	SAFE_DELETE(pReadBuf);
}

// ��һ���ڴ��д���
void CDrawingImage::CreateByMem(BYTE *pImgFileMem, int nLen)
{
	if (pImgFileMem == NULL || nLen <= 0)
		return;

	Delete();
	DrawingImageRelease();
	CComPtr<IStream> ImgStream;
	if (::CreateStreamOnHGlobal(NULL, TRUE, &ImgStream) != S_OK)
		return;

	ImgStream->Write(pImgFileMem, nLen, NULL);
	Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(ImgStream);
	if (pBitmap == NULL)
	{
		ImgStream.Release();
		return;
	}

	this->Create(pBitmap->GetWidth(), pBitmap->GetHeight(), false, true);
	if (IS_SAFE_HANDLE(m_hDC))
	{
		Graphics DoGrap(m_hDC);
		RectF bmpRct;
		bmpRct.X = bmpRct.Y = (REAL)0;
		bmpRct.Width = (REAL)pBitmap->GetWidth();
		bmpRct.Height = (REAL)pBitmap->GetHeight();
		DoGrap.DrawImage(pBitmap, bmpRct, 0, 0, bmpRct.Width, bmpRct.Height, UnitPixel);
	}

	SAFE_DELETE(pBitmap);
	ImgStream.Release();
}

bool CDrawingImage::GetImageCLSID(const WCHAR* format, CLSID* pCLSID)  
{   
	// �õ���ʽΪformat��ͼ���ļ��ı���ֵ�����ʸø�ʽͼ���COM�����GUIDֵ������pCLSID��  
	UINT num  = 0;  
	UINT size = 0;  

	ImageCodecInfo* pImageCodecInfo = NULL;  
	GetImageEncodersSize(&num, &size);  

	if (size == 0)  
		return false;   // ������Ϣ������  

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));  
	if (pImageCodecInfo == NULL)  
		return false;

	//���ϵͳ�п��õı��뷽ʽ��������Ϣ  
	GetImageEncoders(num, size, pImageCodecInfo);  

	// �ڿ��ñ�����Ϣ�в���format��ʽ�Ƿ�֧��  
	for (UINT i = 0; i < num; ++i)  
	{
		// MimeType�����뷽ʽ�ľ�������  
		if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)  
		{  
			*pCLSID = pImageCodecInfo[i].Clsid;  
			free(pImageCodecInfo);  
			return true;  
		}  
	}  

	free(pImageCodecInfo);  
	return false;  
}

// ֱ�ӱ���Ϊһ��ָ����ʽ���ļ�
bool CDrawingImage::SaveBoardToFile(const char *pszSavePath)
{
	HDC hDC = NULL;
	// ��ǰ�ֱ�����ÿ������ռ�ֽ���
	int iBits = 0;
	// λͼ��ÿ������ռ�ֽ���
	WORD wBitCount = 0;
	// �����ɫ���С��λͼ�������ֽڴ�С��λͼ�ļ���С��д���ļ��ֽ���
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	// λͼ�ļ�ͷ�ṹ
	BITMAPFILEHEADER bmfHdr;
	// λͼ��Ϣͷ�ṹ
	BITMAPINFOHEADER bi;
	//ָ��λͼ��Ϣͷ�ṹ
	LPBITMAPINFOHEADER lpbi;
	//�����ļ��������ڴ�������ɫ����
	HANDLE fh = NULL, hDib = NULL, hPal = NULL, hOldPal = NULL;

	//����λͼ�ļ�ÿ��������ռ�ֽ���
	hDC = ::CreateDCA("DISPLAY", NULL, NULL, NULL);
	iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	::DeleteDC(hDC);
	if(iBits <= 1)
		wBitCount = 1;
	else if(iBits <= 4)
		wBitCount = 4;
	else if(iBits <=8 )
		wBitCount = 8;
	else if(iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;

	bi.biSize= sizeof(BITMAPINFOHEADER);
	bi.biWidth = m_DcSize.cx;
	bi.biHeight = m_DcSize.cy;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((m_DcSize.cx * wBitCount + 31) / 32) * 4 * m_DcSize.cy;

	//Ϊλͼ���ݷ����ڴ�
	hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);
	*lpbi = bi;

	// �����ɫ��
	hPal = ::GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		::RealizePalette(hDC);
	}

	// ��ȡ�õ�ɫ�����µ�����ֵ
	::GetDIBits(hDC,m_hBmp, 0, (UINT)m_DcSize.cy,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�
	fh= ::CreateFileA(pszSavePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return false;

	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// д��λͼ�ļ�ͷ
	::WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// д��λͼ�ļ���������
	::WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//���
	::GlobalUnlock(hDib);
	::GlobalFree(hDib);
	::CloseHandle(fh);
	return true;
}

// ������ڴ��ʽΪ DIB 32 λ�����ݡ�
void CDrawingImage::CreateByScanMem(int nWidth, int nHeight, BYTE* pScan0)
{
	if (nWidth <= 0 || nHeight <= 0 || pScan0 == NULL)
		return;

	DrawingImageRelease();
	int nLen = nHeight * nWidth * 4;
	m_pScan0 = new BYTE[nLen];
	if (m_pScan0 == NULL)
		return;

	memcpy(m_pScan0, pScan0, nLen);

	Bitmap* pBitmap = new Gdiplus::Bitmap(nWidth, nHeight, nWidth * 4, PixelFormat32bppARGB, m_pScan0);
	if (pBitmap == NULL)
		return;

	this->Create(pBitmap->GetWidth(), pBitmap->GetHeight(), false, true);
	if (IS_SAFE_HANDLE(m_hDC))
	{
		Graphics DoGrap(m_hDC);
		RectF bmpRct;
		bmpRct.X = bmpRct.Y = (REAL)0;
		bmpRct.Width = (REAL)pBitmap->GetWidth();
		bmpRct.Height = (REAL)pBitmap->GetHeight();
		DoGrap.DrawImage(pBitmap, bmpRct, 0, 0, bmpRct.Width, bmpRct.Height, UnitPixel);
	}

	SAFE_DELETE(pBitmap);
}

// ��HBITMAP����
void CDrawingImage::CreateByHBITMAP(HBITMAP hBmp)
{
	if (hBmp == NULL)
		return;

	DrawingImageRelease();
	Bitmap* pBitmap = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
	if (pBitmap == NULL)
		return;

	this->Create(pBitmap->GetWidth(), pBitmap->GetHeight(), false, true);
	if (IS_SAFE_HANDLE(m_hDC))
	{
		Graphics DoGrap(m_hDC);
		RectF bmpRct;
		bmpRct.X = bmpRct.Y = (REAL)0;
		bmpRct.Width = (REAL)pBitmap->GetWidth();
		bmpRct.Height = (REAL)pBitmap->GetHeight();
		DoGrap.DrawImage(pBitmap, bmpRct, 0, 0, bmpRct.Width, bmpRct.Height, UnitPixel);
	}

	SAFE_DELETE(pBitmap);
}

void CDrawingImage::CreateByHICON(HICON hIcon)
{
	if (hIcon == NULL)
		return;

	DrawingImageRelease();
	Bitmap* pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
	if (pBitmap == NULL)
		return;

	this->Create(pBitmap->GetWidth(), pBitmap->GetHeight(), false, true);
	if (IS_SAFE_HANDLE(m_hDC))
	{
		Graphics DoGrap(m_hDC);
		RectF bmpRct;
		bmpRct.X = bmpRct.Y = (REAL)0;
		bmpRct.Width = (REAL)pBitmap->GetWidth();
		bmpRct.Height = (REAL)pBitmap->GetHeight();
		DoGrap.DrawImage(pBitmap, bmpRct, 0, 0, bmpRct.Width, bmpRct.Height, UnitPixel);
	}

	SAFE_DELETE(pBitmap);
}
