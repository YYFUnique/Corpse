#include "StdAfx.h"
#include "FileSystemWatcher.h"

CFileSystemWatcher* pFileSystemWatcher = NULL;

CFileSystemWatcher::CFileSystemWatcher()
{
	pFileSystemWatcher = this;
	m_hFile = INVALID_HANDLE_VALUE;
	m_hThread = NULL;
}

CFileSystemWatcher::~CFileSystemWatcher()
{

}

BOOL CFileSystemWatcher::Run(LPCTSTR lpszMonitorPath, BOOL bWatchSubtree, DWORD dwNotifyFilter, 
										LPDEALFUNCTION FnDeal, LPVOID lParam)
{
	Close();

	BOOL bSuccess = FALSE;

	do 
	{
		m_hFile = CreateFile(lpszMonitorPath,GENERIC_READ,
											FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,
											OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL
											);

		if (m_hFile == INVALID_HANDLE_VALUE) 
			break;

		m_strMonitorPath = lpszMonitorPath;
		m_bWatchSubtree = bWatchSubtree;
		m_dwNotifyFilter = dwNotifyFilter;
		m_DealFun = FnDeal;
		m_DealFunParam = lParam;
		m_bRequestStop = FALSE;

		m_hThread = CreateThread(NULL, 0, Routine, this, 0, NULL);
		if (m_hThread == NULL)
			break;

		bSuccess = TRUE;
	} while (FALSE);


	if (m_hThread == NULL)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}

void CFileSystemWatcher::Close(DWORD dwMilliseconds /*=INFINITE*/)
{
	if (m_hThread != NULL)
	{
		if (WaitForSingleObject(m_hThread, dwMilliseconds) == WAIT_TIMEOUT)
			TerminateThread(m_hThread, 0);

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

DWORD WINAPI CFileSystemWatcher::Routine(LPVOID lParam)
{
	CFileSystemWatcher* pFileObject = (CFileSystemWatcher*)lParam;

	BYTE bData[ 20*(sizeof(FILE_NOTIFY_INFORMATION)+2*MAX_PATH)+2 ];
	FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION *)bData;
	DWORD BytesReturned;
	
	int nIndex = 0;

	while (pFileObject->m_bRequestStop == FALSE)
	{
		if (ReadDirectoryChangesW(pFileObject->m_hFile,
						pNotify, sizeof(bData)-2,
						pFileObject->m_bWatchSubtree,
						pFileObject->m_dwNotifyFilter,
						&BytesReturned, NULL, NULL)) // 无限等待，应当使用异步方式
		{
			for (FILE_NOTIFY_INFORMATION* pInfo = pNotify; pInfo; )
			{
				WCHAR c = pInfo->FileName[pInfo->FileNameLength/2];
				pInfo->FileName[pInfo->FileNameLength/2] = L'\0';

				TCHAR szFileModify[MAX_PATH];
				PathCombine(szFileModify, pFileObject->m_strMonitorPath, pInfo->FileName);
				CString strTipInfo;
				strTipInfo.Format(_T("ACTION:%d, FilePath:%s.\r\n"), pInfo->Action, szFileModify);
				OutputDebugString(strTipInfo);
				/*pFileObject->m_DealFun((FILE_ACTION)pInfo->Action, szFileModify, pFileObject->m_DealFunParam);*/

				if (pInfo->Action == FILE_ACTION_MODIFIED)
					++nIndex;

				pInfo->FileName[pInfo->FileNameLength/2] = c;

				if (pInfo->NextEntryOffset)
					pInfo = (PFILE_NOTIFY_INFORMATION)((BYTE*)pInfo + pInfo->NextEntryOffset);
				else
					pInfo = NULL;
			}

			CString strTipInfo;
			strTipInfo.Format(_T("文件数量:%d.\r\n"), nIndex);
			OutputDebugString(strTipInfo);

			/*BYTE* pData = new BYTE[_countof(bData)];
			RtlCopyMemory(pData, pNotify, sizeof(pData));

			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DispathMessage, pData, 0, NULL); 
			CloseHandle(hThread);*/
		}
		else
		{
			pFileObject->m_DealFun((FILE_ACTION)ACTION_ERRSTOP, 0, pFileObject->m_DealFunParam );
			break;
		}
	}

	return 0;
}

UINT CFileSystemWatcher::DispathMessage(LPVOID lParam)
{
	FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION *)lParam;

	for (FILE_NOTIFY_INFORMATION* pInfo = pNotify; pInfo; )
	{
		WCHAR c = pInfo->FileName[pInfo->FileNameLength/2];
		pInfo->FileName[pInfo->FileNameLength/2] = L'\0';

		TCHAR szFileModify[MAX_PATH];
		PathCombine(szFileModify, pFileSystemWatcher->m_strMonitorPath, pInfo->FileName);
		pFileSystemWatcher->m_DealFun((FILE_ACTION)pInfo->Action, szFileModify, pFileSystemWatcher->m_DealFunParam);

		pInfo->FileName[pInfo->FileNameLength/2] = c;

		if (pInfo->NextEntryOffset)
			pInfo = (PFILE_NOTIFY_INFORMATION)((BYTE*)pInfo + pInfo->NextEntryOffset);
		else
			pInfo = NULL;
	}

	return TRUE;
}