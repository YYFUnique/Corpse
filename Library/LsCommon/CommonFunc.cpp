#include "stdafx.h"
#include "CommonFunc.h"
#include "OsInfo.h"
#include <shlwapi.h>
#include <WinIoCtl.h>
#include "ntdll.h"
#include "RegClass.h"
#include "Class/Process.h"
#include "NtDll.h"

//文件操作临界区
CRITICAL_SECTION g_csWriteFileMutex;	

BOOL DeleteFileByExtension(LPCTSTR lpszDirPath,LPCTSTR lpszExtension /*= NULL*/,DWORD dwKeepFile /*= 3*/)
{
	ASSERT(lpszDirPath && _tcslen(lpszDirPath));
	TCHAR szFilePath[MAX_PATH];

	CString strExtension(lpszExtension);
	if (strExtension.IsEmpty())	//如果没有传入文件后缀则使用所有文件
		strExtension = _T("*");	

	strExtension.TrimLeft(_T("."));	//去掉文件后缀的原点分隔符
	strExtension.Insert(0,_T("*."));

	PathCombine(szFilePath,lpszDirPath,strExtension);

	WIN32_FIND_DATA FindData;
	HANDLE hFindFile = FindFirstFile(szFilePath,&FindData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;

	CFileInfo FileInfo;
	do
	{
		CString strFileName(FindData.cFileName);
		if (strFileName.Compare(_T(".")) ==0 || strFileName.Compare(_T("..")) ==0)
			continue;

		TCHAR szTargetFilePath[MAX_PATH];
		PathCombine(szTargetFilePath,lpszDirPath,strFileName);
		HANDLE hFile = CreateFile(szTargetFilePath,GENERIC_ALL,FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
			continue;

		FILETIME FileCreateTime;
		GetFileTime(hFile,NULL,NULL,&FileCreateTime);
		CloseHandle(hFile);
		DeleteFileByCreateTime(&FileInfo,dwKeepFile,szTargetFilePath,FileCreateTime);

	}while(FindNextFile(hFindFile,&FindData));

	FindClose(hFindFile);
	return TRUE;
}

BOOL DeleteFileByCreateTime(CFileInfo* pFileInfo,DWORD dwKeepFile,LPCTSTR lpszFilePath,FILETIME CreateTime)
{
	CTime Time(CreateTime);
	time_t tmFileTime = Time.GetTime();
	if ((DWORD)pFileInfo->GetSize()<dwKeepFile)
		pFileInfo->SetAt(lpszFilePath,tmFileTime);
	else
	{
		POSITION pos = pFileInfo->GetStartPosition();
		BOOL bDelete = FALSE;

		//默认要删除的文件是想添加到链表的文件
		CString strDeleteFile=lpszFilePath;
		time_t tmMinFileCreatTime=tmFileTime;

		while(pos)
		{
			CString strCurrentFilePath;
			time_t tmCurrentFileCreateTime;
			pFileInfo->GetNextAssoc(pos,strCurrentFilePath,tmCurrentFileCreateTime);
			if (tmCurrentFileCreateTime<tmMinFileCreatTime)
			{
				tmMinFileCreatTime = tmCurrentFileCreateTime;
				strDeleteFile = strCurrentFilePath;
			}
		}
		//如果要删除的文件不是需要添加的文件，那么将该文件添加到链表中
		if (strDeleteFile.CompareNoCase(lpszFilePath))
		{
			pFileInfo->RemoveKey(strDeleteFile);
			pFileInfo->SetAt(lpszFilePath,tmFileTime);			
		}

		if (DeleteFile(strDeleteFile)==FALSE)
			return FALSE;
	}

	return TRUE;
}

BOOL OpenFolderAndSelectFile(LPCTSTR lpszFilePath)
{
	LPSHELLFOLDER pDesktopFolder;
	if ( SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	{  
		LPITEMIDLIST     pidl;
		ULONG             chEaten;
		ULONG             dwAttributes;
		HRESULT             hr;

		CStringW strFilePath(lpszFilePath);
		WCHAR szPath[MAX_PATH];
		wcscpy_s(szPath,_countof(szPath),strFilePath);
		hr = pDesktopFolder->ParseDisplayName(
			NULL, NULL, szPath, &chEaten, &pidl, &dwAttributes);
		if (FAILED(hr))
		{
			pDesktopFolder->Release();
			return FALSE;
		}
		LPCITEMIDLIST pidlFolder = pidl;
		CoInitialize( NULL);
		hr = SHOpenFolderAndSelectItems( pidl, 0, NULL, 0);
		pDesktopFolder->Release();
		if ( hr == S_OK)
			return TRUE;
	}
	return FALSE;
}

BOOL LookupFileProperty(LPCTSTR lpszFilePath)
{
	// TODO: 在此添加命令处理程序代码
	/*CListCtrl& ListCtrl = GetListCtrl();
	int nSelectColumn = ListCtrl.GetSelectionMark();
	TCHAR szFilePath[MAX_PATH],szModuleFilePath[MAX_PATH];
	GetIniFilePath(szModuleFilePath);
	PathRemoveFileSpec(szModuleFilePath);
	CString strFile = ListCtrl.GetItemText(nSelectColumn,1);
	PathCombine(szFilePath,szModuleFilePath,strFile);

	if (PathFileExists(szFilePath) == FALSE)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("文件%s不存在！"),szFilePath);
		MessageBox(strTipInfo,_T("提示"),MB_OK);
		return;
	}
	*/

	SHELLEXECUTEINFO ShellInfo; 
	//ZeroMemory(&i,sizeof(i)); 
	ShellInfo.fMask=SEE_MASK_INVOKEIDLIST|SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI; 
	ShellInfo.lpVerb=_T("properties"); 
	ShellInfo.lpFile=lpszFilePath; 
	ShellInfo.lpParameters=0; 
	ShellInfo.lpDirectory=0; 

	ShellInfo.lpIDList=NULL; 
	ShellInfo.cbSize=sizeof(ShellInfo); 
	return ShellExecuteEx(&ShellInfo); 
}

CString GetMainModuleFileName()
{
	static CString s_strExecAppName;
	if (s_strExecAppName.IsEmpty())
	{
		InitializeCriticalSection(&g_csWriteFileMutex);

		TCHAR szExeTitle[MAX_PATH];
		::GetModuleFileName(0,szExeTitle,MAX_PATH);
		PathRemoveExtension(szExeTitle);
		s_strExecAppName = PathFindFileName(szExeTitle);
	}
	return s_strExecAppName;
}

BOOL AdjustProcessAuthority(LPCTSTR lpszAuthorityName,BOOL bEnable /* = TRUE*/)
{
	ASSERT_STR_NOT_NULL(lpszAuthorityName);
	
	HANDLE hToken = NULL;
	LUID Value;
	TOKEN_PRIVILEGES tkp;
	BOOL bRet = FALSE;

	do 
	{
		bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY ,&hToken);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开当前进程获取Token失败"));
			break;
		}
		bRet = LookupPrivilegeValue(NULL , lpszAuthorityName , &Value);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("查询特权%s对应的LUID失败") , lpszAuthorityName);
			break;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = Value;
		if (bEnable)
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		else
			tkp.Privileges[0].Attributes = 0;

		bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0 , NULL, NULL);	
		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)	//如果当前进程本来就没有对应的特权该函数可能返回TRUE
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("调整进程特权失败"));
			bRet = FALSE;
			break;
		}

	} while (FALSE);
	
	if (hToken != NULL)
		CloseHandle(hToken);
	
	return bRet;
}

BOOL AdjustListWidth(CListCtrl& ListCtrl)
{
	int nTotal = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i=0;i<nTotal;++i)
		ListCtrl.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;
}

BOOL InsertGroup(CListCtrl& ListCtrl , LPCTSTR lpszGroupName , int nGroupId)
{
	CStringW strTxt(lpszGroupName);
	WCHAR wchTemp[MAX_PATH]={0};
	LVGROUP lvg;
	ZeroMemory(&lvg, sizeof(lvg));
	lvg.cbSize = sizeof(lvg);
	lvg.mask =  LVGF_STATE | LVGF_HEADER | LVGF_GROUPID;
	
	//_tcscpy_s(lvg.pszHeader,_countof(lvg.pszHeader),CStringW(lpszGroupName));
	lvg.pszHeader = (LPWSTR)(LPCWSTR)strTxt;

	lvg.cchHeader =MAX_PATH;
	lvg.iGroupId = nGroupId;
	if (OsIsVistaOrLater())
		lvg.state=LVGS_COLLAPSIBLE;//分组可收起
	
	int nRet = ListCtrl.InsertGroup(nGroupId,&lvg);
	if (nRet == -1)
		SetErrorInfo(SYSTEM_ERROR,0,_T("插入分组失败"));

	return nRet != -1;
}

CString CovenrtFileTimeToTimeString(FILETIME& FileTime)
{
	//ASSERT(FileTime);
	CString strFileTime;
	CTime Time(FileTime);
	strFileTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"),
						Time.GetYear(),Time.GetMonth(),Time.GetDay(),
						Time.GetHour(),Time.GetMinute(),Time.GetSecond());

	return strFileTime;
}

BOOL GetHardDriveLetter(CStringArray& strDriveLetterArray)
{
	TCHAR szDriveLetter[1024];
	if (GetLogicalDriveStrings(_countof(szDriveLetter) - 1 , szDriveLetter) == 0)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取逻辑驱动器名失败"));
		return FALSE;
	}

	LPCTSTR lpszItem = szDriveLetter;
	while(lpszItem[0])
	{
		if (GetDriveType(lpszItem) == DRIVE_FIXED)
			strDriveLetterArray.Add(lpszItem);
		lpszItem += _tcslen(lpszItem) + 1;
	}

	return TRUE;
}

CString GetOsInLogicalDrive()
{
	CString strDriver;
	TCHAR szWindowsDirectory[MAX_PATH];
	GetSystemWindowsDirectory(szWindowsDirectory,sizeof(szWindowsDirectory));

	if (PathStripToRoot(szWindowsDirectory) == FALSE)
		return NULL;

	strDriver = szWindowsDirectory;
	return strDriver;
}

BOOL GetPhysicalHardIndex(LPCTSTR lpszOsDriver,DWORD& dwHardIndex)
{
	HANDLE hFile = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		CString strVolumnName;
		strVolumnName.Format(_T("\\\\.\\%c:"),lpszOsDriver[0]);
		hFile = CreateFile(strVolumnName,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开磁盘失败"));
			break;
		}
		
		VOLUME_DISK_EXTENTS VolumeDiskExtents;
		DWORD dwBytesReturned  = 0;
		BOOL bRet = DeviceIoControl(hFile,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,NULL,0,
								&VolumeDiskExtents,sizeof(VOLUME_DISK_EXTENTS),&dwBytesReturned,NULL);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取卷%s信息失败"),strVolumnName);
			break;
		}
		dwHardIndex = VolumeDiskExtents.Extents[0].DiskNumber;
		bSuccess = TRUE;
	} while (FALSE);
	
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bSuccess;
}

BOOL GetSystemLastShutDownTime(FILETIME& SystemShutDownTime)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CRegClass RegLastShutDownTime(HKEY_LOCAL_MACHINE);
		LPCTSTR lpszLastShutDownTime = _T("SYSTEM\\CurrentControlSet\\Control\\Windows");
		if (RegLastShutDownTime.OpenKey(lpszLastShutDownTime) == FALSE)
		{
			SetErrorTitle(_T("获取系统上次关机时间，打开项失败"));
			break;
		}

		DWORD dwSize = sizeof(FILETIME);
		
		if (RegLastShutDownTime.ReadKey(_T("ShutdownTime"),(LPBYTE)&SystemShutDownTime,dwSize) == FALSE)
		{
			SetErrorTitle(_T("获取系统上次关机时间失败"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

BOOL GetOSInstallTime(DWORD& dwInstallTime)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CRegClass RegOSInstallTime;
		LPCTSTR lpszOSInstallTime = _T("SOFTWARE\\MicroSoft\\Windows NT\\CurrentVersion");
// 		if (IsWow64Process() == TRUE)
// 			RegOSInstallTime.SetKeyAccess(KEY_WOW64_64KEY);

		if (RegOSInstallTime.OpenKey(lpszOSInstallTime) == FALSE)
		{
			SetErrorTitle(_T("获取系统上次关机时间，打开项失败"));
			break;
		}

		if (RegOSInstallTime.ReadKey(_T("InstallDate"),dwInstallTime) == FALSE)
		{
			SetErrorTitle(_T("获取系统上次关机时间失败"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}
//
//BOOL CopyDataToClipboard(HWND hWnd,LPCTSTR lpszData)
//{
//	if (OpenClipboard(hWnd) == FALSE)
//	{
//		SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开系统剪贴板失败"));
//		return FALSE;
//	}
//
//	EmptyClipboard();
//
//	HGLOBAL hClipBuffer = NULL;
//	BOOL bSuccess = FALSE;
//
//	do 
//	{
//		DWORD dwSize = (_tcslen(lpszData)+1) * sizeof(TCHAR);
//		TCHAR*   Clipbufferr;
//		hClipBuffer = GlobalAlloc(GMEM_DDESHARE , dwSize);
//		if (hClipBuffer == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR,0,_T("拷贝数据时分配内存失败"));
//			break;
//		}
//
//		Clipbufferr = (TCHAR*)GlobalLock(hClipBuffer);
//		_tcscpy_s(Clipbufferr , dwSize , lpszData);
//		GlobalUnlock(hClipBuffer);
//
//		DWORD dwDataFormat = 0;
//
//#ifdef _UNICODE
//		dwDataFormat = CF_UNICODETEXT;
//#else
//		dwDataFormat = CF_TEXT;
//#endif
//
//		SetClipboardData(dwDataFormat , hClipBuffer);
//		
//		GlobalFree(hClipBuffer);
//		bSuccess = TRUE;
//	} while (FALSE);
//
//	CloseClipboard();
//
//	return bSuccess;
//}


BOOL GetSystemHandleInfo(DWORD dwPid,CSystemHandleInfoList& SystemHandleInfoList)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	BOOL bSuccess = FALSE;
	LPBYTE lpDataBuffer = NULL;

	do 
	{
		if (GetProcessHandle(dwPid,hProcess) == FALSE)
		{
			SetErrorTitle(_T("获取进程%d句柄失败"),dwPid);
			break;
		}

		DWORD dwHandleCount = 0;
		if (GetProcessHandleCount(hProcess,&dwHandleCount) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程%d句柄数失败"),dwPid);
			break;
		}

		DWORD dwBufferSize = 0;
		DWORD dwSize = 0;
		PSYSTEM_HANDLE pSystemHandle = NULL;
		HRESULT hr=0;
		int i=0;

		dwBufferSize = dwHandleCount * sizeof(SYSTEM_HANDLE_INFORMATION);
		lpDataBuffer = new BYTE[dwBufferSize];

		do 
		{				
			pSystemHandle = (PSYSTEM_HANDLE)lpDataBuffer;
			hr = NtQuerySystemInformation(SystemHandleInformation,lpDataBuffer,dwBufferSize,&dwSize);
			if (FAILED(hr))
			{
				delete[] lpDataBuffer;
				lpDataBuffer = NULL;

				dwBufferSize = dwSize;
				lpDataBuffer = new BYTE[dwBufferSize];

				if (lpDataBuffer == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("分配内存失败"));
					break;
				}
			}
		} while (hr == STATUS_INFO_LENGTH_MISMATCH);
		
		if (pSystemHandle == NULL)
			break;

		for (DWORD i=0;i<pSystemHandle->dwCount;++i)
		{
			if (pSystemHandle->shr[i].ProcessId == dwPid)
			{
				SystemHandleInfoList.AddTail(pSystemHandle->shr[i]);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (lpDataBuffer != NULL)
	{
		delete[] lpDataBuffer;
		lpDataBuffer = NULL;
	}

	if (hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(hProcess);

	return bSuccess;
}

UINT InsertIconToImageList(LPCTSTR lpszSrcFilePath,CImageList& pImageList,BOOL bEnableDirectory /* = FALSE*/)
{
	UINT uIndex = 0;
	do 
	{
		if (PathFileExists(lpszSrcFilePath) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 ,_T("文件%s不存在"),lpszSrcFilePath);
			break;
		}

		if (bEnableDirectory == FALSE && PathIsDirectory(lpszSrcFilePath) == FILE_ATTRIBUTE_DIRECTORY)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 ,_T("路径%s是一个目录"),lpszSrcFilePath);
			break;
		}

		SHFILEINFO ShFileInfo;
		memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
		DWORD dwRet = SHGetFileInfo(lpszSrcFilePath , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
		if (dwRet == 0)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取文件图标失败"));
			break;
		}

		uIndex = pImageList.Add(ShFileInfo.hIcon);
		DestroyIcon(ShFileInfo.hIcon);

	} while (FALSE);
	
	return uIndex;
}

BOOL AddSubMenuToMenu(CMenu* pMenu,LPCTSTR lpszMenuItem, UINT nIcon)
{
	//pMenu->AppendMenu(MF_STRING,)
	return TRUE;
}

