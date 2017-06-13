#include "StdAfx.h"
#include "ShellHook.h"
#include "ShellCopyClassFactory.h"

extern ULONG      g_lLockNumber;
extern ULONG      g_lRefThisDll;

CShellCopyClassFactory::CShellCopyClassFactory()
:m_lRefCount(0)
{

}

CShellCopyClassFactory::~CShellCopyClassFactory()
{

}

HRESULT STDMETHODCALLTYPE CShellCopyClassFactory::QueryInterface (REFIID iid, void **ppvObject)
{
	if(iid == IID_IUnknown)
	{
		m_lRefCount++;
		*ppvObject = this;
	}
	else if(iid == IID_IClassFactory)
	{
		m_lRefCount++;
		*ppvObject = this;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG STDMETHODCALLTYPE CShellCopyClassFactory::AddRef ()
{
	m_lRefCount++;
	return m_lRefCount;
}

ULONG STDMETHODCALLTYPE CShellCopyClassFactory::Release ()
{
	m_lRefCount--;

	if(m_lRefCount == 0)
		delete this;

	return m_lRefCount;
}

HRESULT STDMETHODCALLTYPE CShellCopyClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
	OutputDebugString(_T("CreateInstance"));
	*ppvObject = NULL;  
	if(pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	CShellHook* pObj = new CShellHook();

	if (pObj == NULL)
		return E_OUTOFMEMORY ;

	return pObj->QueryInterface(riid, ppvObject);
}

HRESULT STDMETHODCALLTYPE CShellCopyClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		g_lLockNumber++;
	else
		g_lLockNumber--;

	return NOERROR;
}
