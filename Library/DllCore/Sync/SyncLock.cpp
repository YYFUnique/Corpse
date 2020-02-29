#include "StdAfx.h"
#include "SyncLock.h"

/////////////////////////////////////////////////////////////////////////////
// CSyncObject

CSyncObject::CSyncObject(LPCTSTR lpszSyncName)
{
	m_hSyncObject = NULL;
}

CSyncObject::~CSyncObject()
{
	if (m_hSyncObject != NULL)
	{
		::CloseHandle(m_hSyncObject);
		m_hSyncObject = NULL;
	}
}

BOOL CSyncObject::Lock(DWORD dwTimeout /*= INFINITE*/)
{
	DWORD dwRet = ::WaitForSingleObject(m_hSyncObject, dwTimeout);
	if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
		return TRUE;
	else
		return FALSE;
}

BOOL CSyncObject::Unlock(LONG  lCount , LPLONG  lpPrevCount /*= NULL*/)
{
	return FALSE;
}

HANDLE CSyncObject::GetSafeHandle() const
{
	return m_hSyncObject;
}

CSyncObject::operator HANDLE() const
{
	return m_hSyncObject;
}

/////////////////////////////////////////////////////////////////////////////
// CSemaphore

CSemaphore::CSemaphore(LONG lInitialCount /* = 1 */, LONG lMaxCount /* = 1 */, 
											LPCTSTR pstrName/* =NULL */, LPSECURITY_ATTRIBUTES lpsaAttributes /* = NULL */)
	: CSyncObject(pstrName)
{
	ASSERT(lMaxCount > 0);
	ASSERT(lInitialCount <= lMaxCount);

	m_hSyncObject = ::CreateSemaphore(lpsaAttributes, lInitialCount, lMaxCount, pstrName);
	if (m_hSyncObject == NULL)
		ASSERT("m_hSyncObject Is NULL");
}

CSemaphore::~CSemaphore()
{
	Unlock();
}

BOOL CSemaphore::Unlock(LONG lCount, LPLONG lpPrevCount /* = NULL */)
{
	if (m_hSyncObject == NULL)
		return FALSE;

	return ::ReleaseSemaphore(m_hSyncObject, lCount, lpPrevCount);
}

BOOL CSemaphore::Unlock()
{
	return ::ReleaseSemaphore(m_hSyncObject, 1, NULL);
}

CCriticalSection::CCriticalSection()
	: CSyncObject(NULL)
{
	HRESULT hRet = S_OK;
	//可以理解为InitializeCriticalSection，为了效率，加了一个旋转锁。
	if (InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0) == FALSE)
		hRet =  HRESULT_FROM_WIN32(GetLastError());

	if (FAILED(hRet))
		AtlThrow(hRet);	
}

CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(&m_CriticalSection);
}

BOOL CCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
	return TRUE;
}

BOOL CCriticalSection::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
	return TRUE;
}

BOOL CCriticalSection::Lock(DWORD dwTimeout)
{
	return FALSE;	
}

CCriticalSection::operator CRITICAL_SECTION*()
{
	return &m_CriticalSection;
}

CSingleLock::CSingleLock(CSyncObject* pSyncObject, BOOL bInitialLock /* = FALSE */)
{
	m_pSyncObject = pSyncObject;
	m_hSyncObject = pSyncObject->GetSafeHandle();
	m_bAcquired = FALSE;

	if (bInitialLock)
		Lock();
}

CSingleLock::~CSingleLock()
{
	Unlock();
}

BOOL CSingleLock::Lock(DWORD dwTimeOut /* = INFINITE */)
{
	ASSERT(m_pSyncObject != NULL || m_hSyncObject != NULL);
	ASSERT(!m_bAcquired);

	m_bAcquired = m_pSyncObject->Lock(dwTimeOut);
	return m_bAcquired;
}

BOOL CSingleLock::Unlock()
{
	ASSERT(m_pSyncObject != NULL);
	if (m_bAcquired)
		m_bAcquired = !m_pSyncObject->Unlock();

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}

BOOL CSingleLock::Unlock(LONG lCount, LPLONG lpPrevCount /* = NULL */)
{
	ASSERT(m_pSyncObject != NULL);
	if (m_bAcquired)
		m_bAcquired = !m_pSyncObject->Unlock(lCount, lpPrevCount);

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}

BOOL CSingleLock::IsLocked()
{
	return m_bAcquired;
}

CEvent::CEvent(BOOL bInitiallyOwn /* = FALSE */, BOOL bManualReset /* = FALSE */, 
						LPCTSTR lpszNAme /* = NULL */, LPSECURITY_ATTRIBUTES lpsaAttribute /* = NULL */)
	: CSyncObject(lpszNAme)
{
	m_hSyncObject = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, lpszNAme);
	if (m_hSyncObject == NULL)
		ASSERT("m_hSyncObject Is NULL");
}

CEvent::~CEvent()
{

}

BOOL CEvent::SetEvent()
{
	if (m_hSyncObject == NULL)
		return FALSE;

	return ::SetEvent(m_hSyncObject);
}

BOOL CEvent::PulseEvent()
{
	if (m_hSyncObject == NULL)
		return FALSE;

	return ::PulseEvent(m_hSyncObject);
}

BOOL CEvent::ResetEvent()
{
	if (m_hSyncObject == NULL)
		return FALSE;

	return ::ResetEvent(m_hSyncObject);
}

BOOL CEvent::Unlock()
{
	return TRUE;
}

NLock::NLock()
{
	// The second parameter is the spin count, for short-held locks it avoid the
	// contending thread from going to sleep which helps performance greatly.
	::InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 2000);
}

NLock::~NLock()
{
	::DeleteCriticalSection(&m_CriticalSection);
}

BOOL NLock::TryEnter()
{
	return ::TryEnterCriticalSection(&m_CriticalSection);
}

void NLock::Lock()
{
	::EnterCriticalSection(&m_CriticalSection);
}

void NLock::Unlock()
{
	::LeaveCriticalSection(&m_CriticalSection);
}