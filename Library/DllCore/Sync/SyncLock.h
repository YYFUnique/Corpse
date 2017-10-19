#pragma once

//#include "base/base_export.h"
//#include "base/base_config.h"

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