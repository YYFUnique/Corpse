#include "stdafx.h"
#include "CommonFunc.h"
#include "OsInfo.h"
#include <shlwapi.h>
#include <WinIoCtl.h>
#include "ntdll.h"
#include "RegClass.h"
#include "Class/Process.h"
#include "NtDll.h"

//�ļ������ٽ���
CRITICAL_SECTION g_csWriteFileMutex;	

BOOL DeleteFileByExtension(LPCTSTR lpszDirPath,LPCTSTR lpszExtension /*= NULL*/,DWORD dwKeepFile /*= 3*/)
{
	ASSERT(lpszDirPath && _tcslen(lpszDirPath));
	TCHAR szFilePath[MAX_PATH];

	CString strExtension(lpszExtension);
	if (strExtension.IsEmpty())	//���û�д����ļ���׺��ʹ�������ļ�
		strExtension = _T("*");	

	strExtension.TrimLeft(_T("."));	//ȥ���ļ���׺��ԭ��ָ���
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

		//Ĭ��Ҫɾ�����ļ�������ӵ�������ļ�
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
		//���Ҫɾ�����ļ�������Ҫ��ӵ��ļ�����ô�����ļ���ӵ�������
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
	// TODO: �ڴ���������������
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
		strTipInfo.Format(_T("�ļ�%s�����ڣ�"),szFilePath);
		MessageBox(strTipInfo,_T("��ʾ"),MB_OK);
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
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("�򿪵�ǰ���̻�ȡTokenʧ��"));
			break;
		}
		bRet = LookupPrivilegeValue(NULL , lpszAuthorityName , &Value);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ѯ��Ȩ%s��Ӧ��LUIDʧ��") , lpszAuthorityName);
			break;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = Value;
		if (bEnable)
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		else
			tkp.Privileges[0].Attributes = 0;

		bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0 , NULL, NULL);	
		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)	//�����ǰ���̱�����û�ж�Ӧ����Ȩ�ú������ܷ���TRUE
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("����������Ȩʧ��"));
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
		lvg.state=LVGS_COLLAPSIBLE;//���������
	
	int nRet = ListCtrl.InsertGroup(nGroupId,&lvg);
	if (nRet == -1)
		SetErrorInfo(SYSTEM_ERROR,0,_T("�������ʧ��"));

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
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ȡ�߼���������ʧ��"));
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
			SetErrorInfo(SYSTEM_ERROR,0,_T("�򿪴���ʧ��"));
			break;
		}
		
		VOLUME_DISK_EXTENTS VolumeDiskExtents;
		DWORD dwBytesReturned  = 0;
		BOOL bRet = DeviceIoControl(hFile,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,NULL,0,
								&VolumeDiskExtents,sizeof(VOLUME_DISK_EXTENTS),&dwBytesReturned,NULL);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ��%s��Ϣʧ��"),strVolumnName);
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
			SetErrorTitle(_T("��ȡϵͳ�ϴιػ�ʱ�䣬����ʧ��"));
			break;
		}

		DWORD dwSize = sizeof(FILETIME);
		
		if (RegLastShutDownTime.ReadKey(_T("ShutdownTime"),(LPBYTE)&SystemShutDownTime,dwSize) == FALSE)
		{
			SetErrorTitle(_T("��ȡϵͳ�ϴιػ�ʱ��ʧ��"));
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
			SetErrorTitle(_T("��ȡϵͳ�ϴιػ�ʱ�䣬����ʧ��"));
			break;
		}

		if (RegOSInstallTime.ReadKey(_T("InstallDate"),dwInstallTime) == FALSE)
		{
			SetErrorTitle(_T("��ȡϵͳ�ϴιػ�ʱ��ʧ��"));
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
//		SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ϵͳ������ʧ��"));
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
//			SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʱ�����ڴ�ʧ��"));
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
			SetErrorTitle(_T("��ȡ����%d���ʧ��"),dwPid);
			break;
		}

		DWORD dwHandleCount = 0;
		if (GetProcessHandleCount(hProcess,&dwHandleCount) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ����%d�����ʧ��"),dwPid);
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("�����ڴ�ʧ��"));
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
			SetErrorInfo(SYSTEM_ERROR , 0 ,_T("�ļ�%s������"),lpszSrcFilePath);
			break;
		}

		if (bEnableDirectory == FALSE && PathIsDirectory(lpszSrcFilePath) == FILE_ATTRIBUTE_DIRECTORY)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 ,_T("·��%s��һ��Ŀ¼"),lpszSrcFilePath);
			break;
		}

		SHFILEINFO ShFileInfo;
		memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
		DWORD dwRet = SHGetFileInfo(lpszSrcFilePath , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
		if (dwRet == 0)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ȡ�ļ�ͼ��ʧ��"));
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

