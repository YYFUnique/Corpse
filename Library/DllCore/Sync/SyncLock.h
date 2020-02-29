#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#endif
#include <assert.h>

class DLL_API CSyncObject
{
public:
	CSyncObject(LPCTSTR lpszSyncName);
	virtual ~CSyncObject();

public:
	virtual BOOL Lock(DWORD dwTimeout = INFINITE);
	virtual BOOL Unlock() = 0;
	virtual BOOL Unlock(LONG  lCount , LPLONG  lpPrevCount = NULL );

public:
	HANDLE GetSafeHandle() const;
	operator HANDLE() const;
protected:
	HANDLE  m_hSyncObject;
};

class DLL_API CSemaphore : public CSyncObject
{
public:
	CSemaphore(LONG lInitialCount = 1, LONG lMaxCount = 1, LPCTSTR pstrName=NULL, LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
	virtual ~CSemaphore();

public:
	virtual BOOL Unlock();
	virtual BOOL Unlock(LONG lCount, LPLONG lprevCount = NULL);
};

class DLL_API CCriticalSection : public CSyncObject
{
public:
	CCriticalSection();
	virtual ~CCriticalSection();

public:
	BOOL Unlock();
	BOOL Lock();
	BOOL Lock(DWORD dwTimeout);

public:
	operator CRITICAL_SECTION*();

private:
	BOOL Init();

protected:
	CRITICAL_SECTION m_CriticalSection;
};

class DLL_API CSingleLock
{
public:
	CSingleLock(CSyncObject* pSyncObject, BOOL bInitialLock = FALSE);
	virtual ~CSingleLock();
public:
	BOOL Lock(DWORD dwTimeOut = INFINITE);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	BOOL IsLocked();

protected:
	CSyncObject* m_pSyncObject;
	HANDLE			m_hSyncObject;
	BOOL				m_bAcquired;
};

class DLL_API CEvent : public CSyncObject
{
public:
	CEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	virtual ~CEvent();

public:
	BOOL SetEvent();
	BOOL PulseEvent();
	BOOL ResetEvent();
	BOOL Unlock();
};

class DLL_API NLock
{
public:
      NLock();
      ~NLock();

      // If the lock is not held, take it and return true.  If the lock is already
      // held by something else, immediately return false.
      BOOL TryEnter();

      // Take the lock, blocking until it is available if necessary.
      void Lock();

      // Release the lock.  This must only be called by the lock's holder: after
      // a successful call to Try, or a call to Lock.
      void Unlock();

      // Return the native underlying lock.  Not supported for Windows builds.
      CRITICAL_SECTION* GetCriticalSection() { return &m_CriticalSection; }

private:
      CRITICAL_SECTION m_CriticalSection;
};

class DLL_API NAutoLock
{
public:
	NAutoLock(NLock * lock)
	{
		assert(lock);
		lock_ = lock;
		lock_->Lock();
	}

	~NAutoLock()
	{
		if (lock_)
			lock_->Unlock();
	}

private:
	NLock *lock_;
};

class DLL_API NAutoUnlock
{
public:
	NAutoUnlock(NLock * lock)
	{
		assert(lock);
		lock_ = lock;
		lock_->Unlock();
	}

	~NAutoUnlock()
	{
		if (lock_)
			lock_->Lock();
	}

private:
	NLock *lock_;
};