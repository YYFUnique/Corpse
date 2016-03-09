
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

// 从文件内存中创建
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

// 从一段内存中创建
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
	// 得到格式为format的图像文件的编码值，访问该格式图像的COM组件的GUID值保存在pCLSID中  
	UINT num  = 0;  
	UINT size = 0;  

	ImageCodecInfo* pImageCodecInfo = NULL;  
	GetImageEncodersSize(&num, &size);  

	if (size == 0)  
		return false;   // 编码信息不可用  

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));  
	if (pImageCodecInfo == NULL)  
		return false;

	//获得系统中可用的编码方式的所有信息  
	GetImageEncoders(num, size, pImageCodecInfo);  

	// 在可用编码信息中查找format格式是否被支持  
	for (UINT i = 0; i < num; ++i)  
	{
		// MimeType：编码方式的具体描述  
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

// 直接保存为一个指定格式的文件
bool CDrawingImage::SaveBoardToFile(const char *pszSavePath)
{
	HDC hDC = NULL;
	// 当前分辨率下每象素所占字节数
	int iBits = 0;
	// 位图中每象素所占字节数
	WORD wBitCount = 0;
	// 定义调色板大小，位图中像素字节大小，位图文件大小，写入文件字节数
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	// 位图文件头结构
	BITMAPFILEHEADER bmfHdr;
	// 位图信息头结构
	BITMAPINFOHEADER bi;
	//指向位图信息头结构
	LPBITMAPINFOHEADER lpbi;
	//定义文件，分配内存句柄，调色板句柄
	HANDLE fh = NULL, hDib = NULL, hPal = NULL, hOldPal = NULL;

	//计算位图文件每个像素所占字节数
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

	//为位图内容分配内存
	hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);
	*lpbi = bi;

	// 处理调色板
	hPal = ::GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		::RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值
	::GetDIBits(hDC,m_hBmp, 0, (UINT)m_DcSize.cy,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件
	fh= ::CreateFileA(pszSavePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return false;

	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// 写入位图文件头
	::WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	::WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除
	::GlobalUnlock(hDib);
	::GlobalFree(hDib);
	::CloseHandle(fh);
	return true;
}

// 输入的内存格式为 DIB 32 位的数据。
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

// 从HBITMAP创建
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
