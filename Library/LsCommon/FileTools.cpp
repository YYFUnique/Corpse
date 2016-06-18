#include "Stdafx.h"
#include "FileTools.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "version.lib")

void GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath)
{
	lpszFilePath[0]=0;

	HRESULT hResult=SHGetFolderPath(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,lpszFilePath);
	if (hResult != S_OK)
	{
		if (SHGetFolderPath(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_DEFAULT,lpszFilePath)!=S_OK)
		{
			GetWindowsDirectory(lpszFilePath,MAX_PATH);
			PathStripToRoot(lpszFilePath);
			PathAppend(lpszFilePath,_T("Documents and Settings\\All Users\\Application Data"));
		}
	}
	PathAppend(lpszFilePath,_T("Corpse"));

	if (PathIsDirectory(lpszFilePath) == FALSE)
		SHCreateDirectoryEx(0,lpszFilePath,0);

	PathAppend(lpszFilePath,lpszFileDirectory);
	if (PathIsDirectory(lpszFilePath) == FALSE)
		SHCreateDirectoryEx(0,lpszFilePath,0);

	PathAppend(lpszFilePath,lpszFileTitle);
}

CString GetFilePath(LPCTSTR lpszFileName,LPCTSTR lpszExtendName,LPCTSTR lpszInfo /* = NULL*/)
{
	ASSERT_STRING_NOT_NONE(lpszFileName);
	ASSERT_STRING_NOT_NONE(lpszExtendName);

	TCHAR szLogPath[MAX_PATH];
	GetCommonAppDataFilePath(lpszFileName,_T(""),szLogPath);
	if (PathIsDirectory(szLogPath)==FALSE)
	{
		if (CreateDirectory(szLogPath,0)==FALSE)
			::PathRemoveFileSpec(szLogPath);
	}

	CString strExtension(lpszExtendName);
	strExtension.TrimLeft(_T("."));

	CString strInfo(lpszInfo);
	if (strInfo.IsEmpty() == FALSE)
		strInfo.Insert(0,_T("-"));

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);
	CString szFileName;
	szFileName.Format(_T("%s(%02d-%02d-%02d)%s.%s"),GetMainModuleFileName(),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,strInfo,strExtension);
	if (szFileName.GetLength()+_tcslen(szLogPath)+2>MAX_PATH)
		::GetSystemDirectory(szLogPath,MAX_PATH);

	::PathAppend(szLogPath,szFileName);
	return szLogPath;
}

DWORD GetFileVersion(LPCTSTR lpszFilePath)
{
	TCHAR szFilePath[MAX_PATH];

	static DWORD dwSelfVersion=0;
	BOOL bReadSelfVersion=FALSE;

	if (lpszFilePath)
		_tcscpy_s(szFilePath,sizeof(szFilePath),lpszFilePath);
	else if (dwSelfVersion)
		return dwSelfVersion;
	else
	{
		bReadSelfVersion=TRUE;
		::GetModuleFileName(0,szFilePath,MAX_PATH);
	}

	DWORD dwVersion=0x1000000;//1.0.0.0

	BOOL bRet = FALSE;
	DWORD dwHandle = 0;
	VS_FIXEDFILEINFO * pVersion = NULL;
	DWORD dwVersionSize = GetFileVersionInfoSize(szFilePath, &dwHandle);

	if (dwVersionSize > 0)
	{
		LPVOID lpData = LocalAlloc(LMEM_ZEROINIT, dwVersionSize);

		if (GetFileVersionInfo(szFilePath, dwHandle, dwVersionSize, lpData))
		{
			UINT uLen = 0;
			if (VerQueryValue(lpData, _T("\\"), (void **)&pVersion, &uLen))
			{
				dwVersion=((pVersion->dwFileVersionMS&0xff0000)<<8) | ((pVersion->dwFileVersionMS&0xff)<<16) |
					((pVersion->dwFileVersionLS&0xff0000)>>8) | (pVersion->dwFileVersionLS&0xff) ;
			}			
		}
		LocalFree(lpData);
	}

	if (bReadSelfVersion)
		dwSelfVersion=dwVersion;
	return dwVersion;
}

BOOL GetFileVersionEx(LPCTSTR lpszFilePath,LPCTSTR lpszFileVersion,CString& strFileVersionInfo)
{
	DWORD dwVersionSize = GetFileVersionInfoSize(lpszFilePath,NULL);
	if (dwVersionSize ==0)
		return FALSE;

	LPVOID lpData = LocalAlloc(LMEM_ZEROINIT, dwVersionSize);
	
	GetFileVersionInfo(lpszFilePath,0,dwVersionSize,lpData);

	BYTE* pVerValue = NULL;
	UINT nSize = 0;
	BOOL bSuccess = FALSE;
	do 
	{
		PLANGANDCODEPAGE pLangandCodePage=0;
		if (VerQueryValue(lpData,_T("\\VarFileInfo\\Translation"),(LPVOID*)&pLangandCodePage,&nSize) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件%s版本信息时获取语言和代码页失败"),PathFindFileName(lpszFilePath));
			break;
		}

		UINT nLen = nSize/sizeof(LANGANDCODEPAGE);
		
		for (UINT i=0;i<nLen;++i)
		{
			CString strSubBlock;
			strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
						pLangandCodePage[i].wLanguage,pLangandCodePage[i].wCodePage,lpszFileVersion);
			if (VerQueryValue(lpData,strSubBlock,(LPVOID*)&pVerValue,&nSize) != FALSE)
			{
				strFileVersionInfo = CString((LPTSTR)pVerValue);
				bSuccess = TRUE;
				break;
			}
		}
		//获取具体文件信息
				
	} while (FALSE);
	
	if (lpData)
		LocalFree(lpData);
	return bSuccess;
}
// 
// CString GetFileVersionInfoText(FILE_VERSION FileVersion)
// {
// 	CString strFileVersionInfo;
// 	switch(FileVersion)
// 	{
// 		case FILE_VERSION_FileDescription:
// 				strFileVersionInfo = _T("FileDescription");
// 			break;
// 		case FILE_VERSION_CompanyName:
// 				strFileVersionInfo = _T("CompanyName");
// 			break;
// 		case FILE_VERSION_ProductName:
// 				strFileVersionInfo = _T("ProductName");
// 			break;
// 		default:
// 				strFileVersionInfo = _T("FileVersion");
// 	}
// 	return strFileVersionInfo;
// }

CString GetVersionText(DWORD dwVersion)
{
	CString strVersion;
	strVersion.Format(_T("V%d.%d.%d.%d"),dwVersion>>24,(dwVersion>>16)&0xff,(dwVersion>>8)&0xff,dwVersion&0xff);
	return strVersion;
}

DWORD LsGetMainAppVersion()
{
	static DWORD s_dwMainAppVersion = 0;
	if (s_dwMainAppVersion == 0)
	{
		TCHAR szMainAppPath[MAX_PATH];
		GetModuleFileName(0 , szMainAppPath , _countof(szMainAppPath));
		PathRemoveFileSpec(szMainAppPath);
		PathAppend(szMainAppPath , _T("Languard.exe"));
		s_dwMainAppVersion = GetFileVersion(szMainAppPath);
	}

	return s_dwMainAppVersion;
}

CString LsGetMainAppVersionText()
{
	DWORD dwMainAppVersion = LsGetMainAppVersion();
	return GetVersionText(dwMainAppVersion);
}

ULONGLONG GetFileSizeByName(LPCTSTR lpszFilePath)
{
	ULONGLONG ulFileSizeInByte = 0;
	do 
	{
		if (PathFileExists(lpszFilePath) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件%s大小时，文件不存在"),lpszFilePath);
			break;
		}

		CFileStatus FileStatusInfo;
		if(CFile::GetStatus(lpszFilePath , FileStatusInfo) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件%s大小时失败"),lpszFilePath);
			break;
		}

		ulFileSizeInByte = FileStatusInfo.m_size;

	} while (FALSE);

	return ulFileSizeInByte;
}

ULONGLONG GetFileLastModifyTime(LPCTSTR lpszFilePath)
{
	ULONGLONG ulFileLastModifyTime = 0;
	do 
	{
		if (PathFileExists(lpszFilePath) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件%s大小时，文件不存在"),lpszFilePath);
			break;
		}

		CFileStatus FileStatusInfo;
		if(CFile::GetStatus(lpszFilePath , FileStatusInfo) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件%s大小时失败"),lpszFilePath);
			break;
		}

		ulFileLastModifyTime = FileStatusInfo.m_mtime.GetTime();

	} while (FALSE);

	return ulFileLastModifyTime;
}

LPSHELLFOLDER GetSpecialFolder(HWND hwnd, UINT nFolderCsidl)
{
	LPSHELLFOLDER lpShellDesktop;
	HRESULT hRes;
	LPSHELLFOLDER pShellFolder = NULL;
	LPITEMIDLIST pItemIdList;

	do 
	{
		hRes = SHGetDesktopFolder(&lpShellDesktop);
		if (FAILED(hRes))
			break;

		hRes = SHGetSpecialFolderLocation(hwnd, nFolderCsidl, &pItemIdList);
		if (SUCCEEDED(hRes))
		{
			hRes = lpShellDesktop->BindToObject(pItemIdList, NULL, IID_IShellFolder, (LPVOID *)&pShellFolder);
			CoTaskMemFree(pItemIdList);
		}
	} while (FALSE);

	if (lpShellDesktop)
		lpShellDesktop->Release();
	
	return pShellFolder;
}

LPSHELLFOLDER GetSpecialFolder(HWND hWnd,LPCTSTR lpszSepcialFolderPath)
{
	LPSHELLFOLDER lpShellDesktop;
	HRESULT hRes;
	LPSHELLFOLDER pShellFolder = NULL;
	LPITEMIDLIST pItemIdList;

	do 
	{
		hRes = SHGetDesktopFolder(&lpShellDesktop);
		if (FAILED(hRes))
			break;

		pItemIdList = PidlFromPath(hWnd,lpszSepcialFolderPath);

		if (SUCCEEDED(hRes))
		{
			hRes = lpShellDesktop->BindToObject(pItemIdList, NULL, IID_IShellFolder, (LPVOID *)&pShellFolder);
			CoTaskMemFree(pItemIdList);
		}
	} while (FALSE);

	if (lpShellDesktop)
		lpShellDesktop->Release();

	return pShellFolder;
}

void	DoDrop(LPDATAOBJECT lpDataObject, LPDROPTARGET lpDropTarget)
{
    POINTL pt = { 0, 0 };
    DWORD dwEffect;
    HRESULT hRes;

    /*
     *  The data object enters the drop target via the left button
     *  with all drop effects permitted.
     */
    dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
    hRes = lpDropTarget->DragEnter(lpDataObject, MK_LBUTTON, pt, &dwEffect);
    if (SUCCEEDED(hRes) && dwEffect) {
        /*
         *  The drop target likes the data object and the effect.
         *  Go drop it.
         */
        hRes = lpDropTarget->Drop(lpDataObject, MK_LBUTTON, pt, &dwEffect);

    } else {
        /*
         *  The drop target didn't like us.  Tell it we're leaving,
         *  sorry to bother you.
         */
        hRes = lpDropTarget->DragLeave();
    }
}

HRESULT GetUIObjectOfPath(HWND hWnd, LPCTSTR pszPath, REFIID riid, LPVOID *ppvOut)
{
    LPITEMIDLIST lpItemIdList;
    HRESULT hRes;

    /*
     *  Just for safety's sake.
     */
    *ppvOut = NULL;

    lpItemIdList = PidlFromPath(hWnd, pszPath);
    if (lpItemIdList == NULL)
        return E_FAIL;

    hRes = GetUIObjectOfAbsPidl(hWnd, lpItemIdList, riid, ppvOut);

    CoTaskMemFree(lpItemIdList);

    return hRes;
}


LPITEMIDLIST PidlFromPath(HWND hWnd, LPCTSTR lpszPath)
{
	LPITEMIDLIST lpItemIdList = NULL;
	ULONG ulEaten;
	DWORD dwAttributes;
	HRESULT hRes;
	LPSHELLFOLDER lpShellDesktopFolder;
	
	do 
	{
		hRes = SHGetDesktopFolder(&lpShellDesktopFolder);
		if (FAILED(hRes))
			break;

		CStringW strPathFileNameW(lpszPath);
		hRes = lpShellDesktopFolder->ParseDisplayName(hWnd,NULL, (LPWSTR)(LPCWSTR)strPathFileNameW, &ulEaten, &lpItemIdList, &dwAttributes);
		if (FAILED(hRes))
			break;

	} while (FALSE);

	return lpItemIdList;
}

HRESULT GetUIObjectOfAbsPidl(HWND hWnd, LPITEMIDLIST pidl, REFIID riid, LPVOID *ppvOut)
{
    /*
     *  To get the UI object of an absolute pidl, we must first bind
     *  to its parent, and then call GetUIObjectOf on the last part.
     */

   // LPITEMIDLIST pidlLast;
	LPCITEMIDLIST pidlLast;
    LPSHELLFOLDER lpShellFolder;
    HRESULT hRes;

    *ppvOut = NULL;

	/*
	 *  Bind to the parent folder of the item we are interested in.
	 */
	hRes = SHBindToParent(pidl, IID_IShellFolder, (LPVOID *)&lpShellFolder, &pidlLast);
    if (FAILED(hRes)) 
	{
        /*
         *  Couldn't even get to the parent; we have no chance of
         *  getting to the item itself.
         */
        return hRes;
    }

    /*
     *  Now ask the parent for the the UI object of the child.
     */
    hRes = lpShellFolder->GetUIObjectOf(hWnd, 1, &pidlLast,riid, NULL, ppvOut);

    /*
     *  Regardless of whether or not the GetUIObjectOf succeeded,
     *  we have no further use for the parent folder.
     */
    lpShellFolder->Release();

    return hRes;
}

BOOL GetFilePathByLink(LPCTSTR lpszLinkName,CString& strFilePath)
{
	HRESULT hRes;   
	IShellLink *pShellLink;   
	IPersistFile *pPersistFile;   
	WIN32_FIND_DATA FindData;
	WCHAR wszLinkFilePath[MAX_PATH];

	CoInitialize(NULL);

	hRes = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&pShellLink);   
	if (FAILED(hRes))
	{
		return   FALSE;
	}

	hRes = pShellLink->QueryInterface(IID_IPersistFile,(void**)&pPersistFile);   
	if (SUCCEEDED(hRes))   
	{   
		hRes = pPersistFile->Load(lpszLinkName,STGM_READ);
		if (SUCCEEDED(hRes))
		{
			hRes = pShellLink->GetPath(wszLinkFilePath,_countof(wszLinkFilePath),&FindData,0); 
			strFilePath = CString(wszLinkFilePath);
		}
		pPersistFile->Release();   
	}   

	pShellLink->Release(); 

	CoUninitialize();

	return SUCCEEDED(hRes);  
}

BOOL SaveBitmapFile(HBITMAP hBitmap,LPCTSTR lpszFileName)
{
	HDC hDC; //设备描述表
	int iBits; //当前显示分辨率下每个像素所占字节数
	WORD wBitCount; //位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数
	DWORD dwPaletteSize=0, dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP Bitmap;
	BITMAPFILEHEADER bmfHdr; //位图属性结构
	BITMAPINFOHEADER BmpInfo; //位图文件头结构
	LPBITMAPINFOHEADER lpbi; //位图信息头结构
	//指向位图信息头结构,定义文件，分配内存句柄，调色板句柄
	HANDLE hFile = INVALID_HANDLE_VALUE, hDib, hPal,hOldPal=NULL; 

	//计算位图文件每个像素所占字节数

	hDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);

	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);

	//设置位图信息头结构

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfo.biWidth = Bitmap.bmWidth;
	BmpInfo.biHeight = Bitmap.bmHeight;
	BmpInfo.biPlanes = 1;
	BmpInfo.biBitCount = wBitCount;
	BmpInfo.biCompression = BI_RGB;
	BmpInfo.biSizeImage = 0;
	BmpInfo.biXPelsPerMeter = 0;
	BmpInfo.biYPelsPerMeter = 0;
	BmpInfo.biClrUsed = 0;
	BmpInfo.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32) * 4 * Bitmap.bmHeight ;
	//为位图内容分配内存

	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = BmpInfo;

	// 处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, Bitmap.bmHeight,
		(LPBYTE)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件
	hFile = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+ (DWORD)sizeof(BITMAPINFOHEADER)+ dwPaletteSize;

	WriteFile(hFile, (LPBYTE)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(hFile, (LPBYTE)lpbi, dwDIBSize,&dwWritten, NULL);

	//消除内存分配
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(hFile);

	return TRUE;
}

char CWowFsRedirectDisableHelper::bIs64 = -1;
BOOL CWowFsRedirectDisableHelper::bInited = FALSE;
CWowFsRedirectDisableHelper::Wow64DisableWow64FsRedirection CWowFsRedirectDisableHelper::f_Wow64DisableWow64FsRedirection = NULL;
CWowFsRedirectDisableHelper::Wow64RevertWow64FsRedirection CWowFsRedirectDisableHelper::f_Wow64RevertWow64FsRedirection = NULL;  

CWowFsRedirectDisableHelper::CWowFsRedirectDisableHelper(BOOL bSet) : m_bSet(bSet), m_pOldValue(NULL)
{
	if(bIs64==-1)
	{
		bIs64 = IsWow64Process() ? 1 : 0; 
	}

	if(bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
	}
}

CWowFsRedirectDisableHelper::~CWowFsRedirectDisableHelper()
{
	RevertFsRedirect();
}

BOOL CWowFsRedirectDisableHelper::DisableFsRedirect()
{
	if(!m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
		m_bSet = TRUE;
	}

	return (m_pOldValue != NULL) ? TRUE:FALSE;
}

void CWowFsRedirectDisableHelper::RevertFsRedirect()
{
	if(m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64RevertWow64FsRedirection(&m_pOldValue);
		m_bSet = FALSE;
	}
}

BOOL CWowFsRedirectDisableHelper::Init()
{
	if(!bInited)
	{
		bInited = TRUE;

		HINSTANCE hlibrary = GetModuleHandle(_T("Kernel32.dll"));
		if(hlibrary)
		{
			f_Wow64DisableWow64FsRedirection = (Wow64DisableWow64FsRedirection) GetProcAddress(hlibrary,"Wow64DisableWow64FsRedirection");
			f_Wow64RevertWow64FsRedirection = (Wow64RevertWow64FsRedirection) GetProcAddress(hlibrary,"Wow64RevertWow64FsRedirection");
		}
	}
	return bInited;
}