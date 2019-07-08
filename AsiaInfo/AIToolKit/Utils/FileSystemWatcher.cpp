#include "StdAfx.h"
#include "FileSystemWatcher.h"

CFileWatcher::CFileWatcher()
 : m_hDir(INVALID_HANDLE_VALUE)
 , m_hThread(NULL)
{

}

CFileWatcher::~CFileWatcher()
{
	Close();
}

BOOL CFileWatcher::Run(LPCTSTR lpszDir, BOOL bwatchSubTree, DWORD dwNotifyFilter, LPDEALFUNCTION lpDealFun, LPVOID lParam)
{
	Close();
	m_hDir = CreateFile(lpszDir, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
									NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,NULL);

	if (m_hDir == INVALID_HANDLE_VALUE)
		return FALSE;

	m_strMonitorPath = lpszDir;
	m_bWatchSubTree = bwatchSubTree;
	m_dwNOtifyFilter = dwNotifyFilter;
	m_pDealFun = lpDealFun;
	m_pDealFunParam = lParam;
	m_bRequestStop = FALSE;

	m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
	if (m_hThread == NULL)
	{
		CloseHandle(m_hDir);
		m_hDir = INVALID_HANDLE_VALUE;
	}

	return m_hThread != INVALID_HANDLE_VALUE;
}

void CFileWatcher::Close(DWORD dwMillisecondes /* = INFINITE */)
{
	if (m_hThread != NULL)
	{
		m_bRequestStop = TRUE;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, dwMillisecondes))
			TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hDir != NULL)
	{
		CloseHandle(m_hDir);
		m_hDir = INVALID_HANDLE_VALUE;
	}
}


DWORD CFileWatcher::ThreadProc(LPVOID lParam)
{
	CFileWatcher* pFileWatch = (CFileWatcher*)lParam;
	BYTE Buf[2 * (sizeof(FILE_NOTIFY_INFORMATION) + 2*MAX_PATH) + 2];
	FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)Buf;

	DWORD dwBytesReturned;
	while(TRUE)
	{
		if (pFileWatch->m_bRequestStop != FALSE)
			break;
		if (ReadDirectoryChangesW(pFileWatch->m_hDir, pNotify,
			sizeof(Buf) - 2, pFileWatch->m_bWatchSubTree, pFileWatch->m_dwNOtifyFilter,
			&dwBytesReturned, NULL,NULL))
		{
			for (FILE_NOTIFY_INFORMATION* pInfo = pNotify; pInfo != NULL;)
			{
				WCHAR c = pInfo->FileName[pInfo->FileNameLength/2];
				pInfo->FileName[pInfo->FileNameLength/2] = L'\0';

				TCHAR szFileModify[MAX_PATH];
				PathCombine(szFileModify, pFileWatch->m_strMonitorPath, pInfo->FileName);
				pFileWatch->m_pDealFun((ACTION)pInfo->Action, szFileModify, pFileWatch->m_pDealFunParam);

				pInfo->FileName[pInfo->FileNameLength/2] = c;
				if (pInfo->NextEntryOffset)
					pInfo = (PFILE_NOTIFY_INFORMATION)((BYTE*)pInfo + pInfo->NextEntryOffset);
				else
					pInfo = NULL;
			}
		}
		else
		{
			pFileWatch->m_pDealFun((ACTION)ACTION_ERRSTOP, 0, pFileWatch->m_pDealFunParam);
			break;
		}
	}

	return 0;
}