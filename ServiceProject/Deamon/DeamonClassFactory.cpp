#include "StdAfx.h"
#include "DeamonClassFactory.h"
#include "DeamonCom.h"

CDeamonClassFactory::CDeamonClassFactory()
	:m_uRefCount(0)
{

}

CDeamonClassFactory::~CDeamonClassFactory()
{

}

STDMETHODIMP_(ULONG) CDeamonClassFactory::AddRef()
{
	return ++m_uRefCount;               // Increment this object's reference count.
}

STDMETHODIMP_(ULONG) CDeamonClassFactory::Release()
{
	ULONG uRet = --m_uRefCount;             // Decrement this object's reference count.

	if ( 0 == m_uRefCount )             // Releasing last reference?
		delete this;

	return uRet;
}

STDMETHODIMP CDeamonClassFactory::QueryInterface ( REFIID riid, void** ppv )
{
	HRESULT hrRet = S_OK;
	// Check that ppv really points to a void*.

	if ( IsBadWritePtr ( ppv, sizeof(void*) ))
		return E_POINTER;
	// Standard QI initialization - set *ppv to NULL.
	*ppv = NULL;
	// If the client is requesting an interface we support, set *ppv.

	if ( InlineIsEqualGUID ( riid, IID_IUnknown ))
		*ppv = (IUnknown*) this;
	else if ( InlineIsEqualGUID ( riid, IID_IClassFactory ))
		*ppv = (IClassFactory*) this;
	else
		hrRet = E_NOINTERFACE;

	if ( S_OK == hrRet )
		((IUnknown*) *ppv)->AddRef();

	return hrRet;
}

/////////////////////////////////////////////////////////////////////////////////////
// IClassFactory methods

STDMETHODIMP CDeamonClassFactory::CreateInstance ( IUnknown* pUnkOuter,REFIID    riid,void**    ppv )
{
	// We don't support aggregation, so pUnkOuter must be NULL.
	if ( NULL != pUnkOuter )
		return CLASS_E_NOAGGREGATION;

	// Check that ppv really points to a void*.
	if ( IsBadWritePtr ( ppv, sizeof(void*) ))
		return E_POINTER;

	*ppv = NULL;
	// Create a new COM object!
	CDeamonCom* pDeamon = new CDeamonCom;

	if ( NULL == pDeamon )
		return E_OUTOFMEMORY;

	// QI the object for the interface the client is requesting.

	HRESULT hRet = pDeamon->QueryInterface ( riid, ppv );

	// If the QI failed, delete the COM object since the client isn't able
	// to use it (the client doesn't have any interface pointers on the object).

	if ( FAILED(hRet) )
		delete pDeamon;

	return hRet;
}

STDMETHODIMP CDeamonClassFactory::LockServer ( BOOL fLock )
{
	// Increase/decrease the DLL ref count, according to the fLock param.
	//fLock ? g_uDllLockCount++ : g_uDllLockCount--;

	return S_OK;
}
