#include "StdAfx.h"
#include "FileSystemWatcher.h"

CFileSystemWatcher::CFileSystemWatcher()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_hThread = NULL;
}

CFileSystemWatcher::~CFileSystemWatcher()
{

}

BOOL CFileSystemWatcher::Run(LPCTSTR lpszMonitorDir, BOOL bWatchSubtree, DWORD dwNotifyFilter, 
										LPDEALFUNCTION FnDeal, LPVOID lParam)
{
	Close();

	BOOL bSuccess = FALSE;

	do 
	{
		m_hFile = CreateFile(lpszMonitorDir,GENERIC_READ,
											FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,
											OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL
											);

		if (m_hFile == INVALID_HANDLE_VALUE) 
			break;

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

	BYTE bData[ 2*(sizeof(FILE_NOTIFY_INFORMATION)+2*MAX_PATH)+2 ];
	FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION *)bData;
	DWORD BytesReturned;

	while (pFileObject->m_bRequestStop == FALSE)
	{
		if (ReadDirectoryChangesW(pFileObject->m_hFile,
						pNotify, sizeof(bData)-2,
						pFileObject->m_bWatchSubtree,
						pFileObject->m_dwNotifyFilter,
						&BytesReturned,NULL, NULL)) // 无限等待，应当使用异步方式
		{
			for (FILE_NOTIFY_INFORMATION* pInfo = pNotify; pInfo; )
			{
				DWORD dwLen = pInfo->FileNameLength;
				LPWSTR lpFileName = new WCHAR[dwLen/2+1];
				if (lpFileName == NULL)
					continue;

				wmemcpy_s(lpFileName, dwLen/2, pInfo->FileName, dwLen/2);

				lpFileName[dwLen/2] = L'\0';

				pFileObject->m_DealFun((FILE_ACTION)pInfo->Action, lpFileName, pFileObject->m_DealFunParam);

				delete lpFileName;

				if (pInfo->NextEntryOffset)
					pInfo = (PFILE_NOTIFY_INFORMATION)((BYTE*)pInfo + pInfo->NextEntryOffset);
				else
					pInfo = NULL;
			}
		}
		else
		{
			pFileObject->m_DealFun((FILE_ACTION)ACTION_ERRSTOP, 0, pFileObject->m_DealFunParam );
			break;
		}
	}

	return 0;
}
