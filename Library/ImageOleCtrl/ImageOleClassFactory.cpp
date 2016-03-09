#include "stdafx.h"
#include "ImageOleCtrl.h"
#include "ImageOleClassFactory.h"

CImageOleClassFactory::CImageOleClassFactory()
:m_uRefCount(0)
{
	g_uDllLockCount++;
}

CImageOleClassFactory::~CImageOleClassFactory()
{
	g_uDllLockCount--;
}

STDMETHODIMP_(ULONG) CImageOleClassFactory::AddRef()
{
	return ++m_uRefCount;               // Increment this object's reference count.
}

STDMETHODIMP_(ULONG) CImageOleClassFactory::Release()
{
	ULONG uRet = --m_uRefCount;             // Decrement this object's reference count.

	if ( 0 == m_uRefCount )             // Releasing last reference?
		delete this;

	return uRet;
}

STDMETHODIMP CImageOleClassFactory::QueryInterface ( REFIID riid, void** ppv )
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

STDMETHODIMP CImageOleClassFactory::CreateInstance ( IUnknown* pUnkOuter,REFIID    riid,void**    ppv )
{
	// We don't support aggregation, so pUnkOuter must be NULL.
	if ( NULL != pUnkOuter )
		return CLASS_E_NOAGGREGATION;

	OutputDebugString(_T("CreateInstance###############################\r\n"));
	// Check that ppv really points to a void*.
	if ( IsBadWritePtr ( ppv, sizeof(void*) ))
		return E_POINTER;

	*ppv = NULL;
	// Create a new COM object!
	CImageOleCtrl* pImageOleCtrl = new CImageOleCtrl;

	if ( NULL == pImageOleCtrl )
		return E_OUTOFMEMORY;

	// QI the object for the interface the client is requesting.

	HRESULT hRet = pImageOleCtrl->QueryInterface ( riid, ppv );

	// If the QI failed, delete the COM object since the client isn't able
	// to use it (the client doesn't have any interface pointers on the object).

	if ( FAILED(hRet) )
		delete pImageOleCtrl;

	return hRet;
}

STDMETHODIMP CImageOleClassFactory::LockServer ( BOOL fLock )
{
	// Increase/decrease the DLL ref count, according to the fLock param.
	fLock ? g_uDllLockCount++ : g_uDllLockCount--;

	return S_OK;
}
