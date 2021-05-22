#include "StdAfx.h"
#include "DirectoryChangeHandler.h"
#include "DllCore/Sync/SyncLock.h"

CDirectoryChangeHandler::CDirectoryChangeHandler()
	:	m_nRefCnt(1)
	,   m_pDirChangeWatcher(NULL)
	,	m_nWatcherRefCnt(0L)
{

}

CDirectoryChangeHandler::~CDirectoryChangeHandler()
{
	UnwatchDirectory();
}

LONG CDirectoryChangeHandler::AddRef()
{ 
	return InterlockedIncrement(&m_nRefCnt);	
}

LONG CDirectoryChangeHandler::Release()
{  
	LONG nRefCnt = -1;
	if ((nRefCnt = InterlockedDecrement(&m_nRefCnt)) == 0)
		delete this;

	return nRefCnt;
}

LONG CDirectoryChangeHandler::GetRef() const 
{ 
	return m_nRefCnt;
}

BOOL CDirectoryChangeHandler::UnwatchDirectory()
{
	CSingleLock lock(&m_csWatcher, TRUE);
	ASSERT(lock.IsLocked());

	if (m_pDirChangeWatcher)
		return m_pDirChangeWatcher->UnwatchDirectory(this);
	return TRUE;
}

LONG  CDirectoryChangeHandler::ReferencesWatcher(CDirectoryChangeWatcher * pDirChangeWatcher)
{
	ASSERT(pDirChangeWatcher);
	CSingleLock lock(&m_csWatcher, TRUE);
	if (m_pDirChangeWatcher &&  m_pDirChangeWatcher != pDirChangeWatcher)
	{
		if (UnwatchDirectory()) 
		{
			m_pDirChangeWatcher = pDirChangeWatcher;
			m_nWatcherRefCnt = 1; //when this reaches 0, set m_pDirChangeWatcher to NULL
			return m_nWatcherRefCnt;
		} 
		else 
		{
			ASSERT( FALSE );//shouldn't get here!
		}
	}
	else
	{
		ASSERT( !m_pDirChangeWatcher || m_pDirChangeWatcher == pDirChangeWatcher );

		m_pDirChangeWatcher = pDirChangeWatcher;	

		if (m_pDirChangeWatcher)
			return InterlockedIncrement(&m_nWatcherRefCnt);

	}
	return m_nWatcherRefCnt;
}

LONG CDirectoryChangeHandler::ReleaseReferenceToWatcher(CDirectoryChangeWatcher * pDirChangeWatcher)
{
	ASSERT( m_pDirChangeWatcher == pDirChangeWatcher );
	CSingleLock lock(&m_csWatcher, TRUE);
	LONG nRef = InterlockedDecrement(&m_nWatcherRefCnt);
	if (nRef <= 0L)
	{
		//won't call m_pDirChangeWatcher->UnwatchDirecotry(this).
		//m_pDirChangeWatcher may point to a destructed object depending on how
		//these classes are being used.
		//Setting this to NULL so that this->UnwatchDirectory() which is called in the dtor
		m_pDirChangeWatcher = NULL; 
		m_nWatcherRefCnt = 0L;
	}
	return nRef;
}

void CDirectoryChangeHandler::OnFileAdded(LPCTSTR lpszFileName)
{ 
	TRACE(_T("The following file was added: %s\n"), lpszFileName);
}

void CDirectoryChangeHandler::OnFileRemoved(LPCTSTR lpszFileName)
{
	TRACE(_T("The following file was removed: %s\n"), lpszFileName);
}

void CDirectoryChangeHandler::OnFileModified(LPCTSTR lpszFileName)
{
	TRACE(_T("The following file was modified: %s\n"), lpszFileName);
}

void CDirectoryChangeHandler::OnFileNameChanged(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName)
{
	TRACE(_T("The file %s was RENAMED to %s\n"), lpszOldFileName, lpszNewFileName);
}

void CDirectoryChangeHandler::OnReadDirectoryChangesError(DWORD dwError, LPCTSTR lpszNewFileName)
{
	TRACE(_T("WARNING!!!!!\n") );
	TRACE(_T("An error has occurred on a watched directory!\n"));
	TRACE(_T("This directory has become unwatched! -- %s \n"), lpszNewFileName);
	TRACE(_T("ReadDirectoryChangesW has failed! %d"), dwError);
	ASSERT( FALSE );//you should override this function no matter what. an error will occur someday.
}

void CDirectoryChangeHandler::OnWatchStarted(DWORD dwError, LPCTSTR lpszDirectoryName)
{	
	if( dwError == 0 )
		TRACE(_T("A watch has begun on the following directory: %s\n"), lpszDirectoryName);
	else
		TRACE(_T("A watch failed to start on the following directory: (Error: %d) %s\n"),dwError, lpszDirectoryName);
}

void CDirectoryChangeHandler::OnWatchStopped(LPCTSTR lpszDirectoryName)
{
	TRACE(_T("The watch on the following directory has stopped: %s\n"), lpszDirectoryName);
}

BOOL CDirectoryChangeHandler::OnFilterNotification(DWORD /*dwNotifyAction*/, LPCTSTR /*szFileName*/, LPCTSTR /*szNewFileName*/)
{
	return TRUE;
}

void CDirectoryChangeHandler::SetChangedDirectoryName(const CString & strChangedDirName)
{
	m_strChangedDirectoryName = strChangedDirName;
}

const CString& CDirectoryChangeHandler::GetChangedDirectoryName() const
{
	return m_strChangedDirectoryName;
}