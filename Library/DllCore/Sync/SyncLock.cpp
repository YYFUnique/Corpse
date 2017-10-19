#include "StdAfx.h"
#include "SyncLock.h"

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